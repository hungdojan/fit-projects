/**
 * Solution for the second assignment of the class PRL at FIT, BUT 2023/2024.
 *
 * @author  Hung Do
 * @date    06/04/2024
 * @file    life.cpp
 */
#include <cstring> // memcpy
#include <fstream> // ifstream
#include <iostream>
#include <mpi.h>
#include <string> // stoi

#define MAX(x, y) ((x > y) ? (x) : (y))
#define MIN(x, y) ((x < y) ? (x) : (y))
#define TO_TOP_PADD 0
#define TO_BOTTOM_PADD 1

class Board {
  private:
    /** The playing field height. */
    int h_;
    /** The playing field width. */
    int w_;
    /** Main board with paddings. */
    char *board_;
    /** Temporary buffer to store next iteration results. */
    char *temp_board_;

    /**
     * @brief Calculate number of living cells in the row.
     *
     * @param col   Column index.
     * @param row   Row index.
     * @returns Number of living cells.
     */
    int calculate_row(const int col, const int row) {
        int count = 0;
        for (int i = MAX(0, col - 1); i <= MIN(w_ - 1, col + 1); i++) {
            count += board_[w_ * (row + 1) + i] - '0';
        }
        return count;
    }

    /**
     * @brief Calculate number of living cells in the neighborhood.
     *
     * @param col   Column index.
     * @param row   Row index.
     * @returns Number of living cells.
     */
    int calculate_neighbors(const int col, const int row) {
        int count = 0;
        // calculate top row
        count += calculate_row(col, row - 1);

        // calculate middle row
        count += calculate_row(col, row);
        count -= board_[w_ * (row + 1) + col] - '0';

        // calculate bottom row
        count += calculate_row(col, row + 1);

        return count;
    }

  public:

    /**
     * @brief Class constructor.
     *
     * @param w     Board width.
     * @param h     Board height.
     */
    Board(const int w, const int h) : h_(h), w_(w) {

        // init board
        board_ = new char[w * (h + 2)];
        temp_board_ = new char[w * h];
        std::memset(board_, '0', w * (h + 2));
        std::memset(temp_board_, 0, w * h);
    }

    /** Return board width. */
    int w() { return w_; }
    /** Return board height. */
    int h() { return h_; }

    /** Return address of the first line of the board (excluding padding). */
    char *board_address_first_line() { return board_ + w_; }

    /** Return address of the last line of the board (excluding padding). */
    char *board_address_last_line() { return board_ + h_ * w_; }

    /** Return address of the start of board's top padding. */
    char *board_address_top_padding() { return board_; }

    /** Return address of the start of board's bottom padding. */
    char *board_address_bottom_padding() { return board_ + (h_ + 1) * w_; }

    /** Calculate and update board's content. */
    void update_board() {
        for (int i = 0; i < h_; i++) {
            for (int j = 0; j < w_; j++) {
                int neighbors = calculate_neighbors(j, i);
                // alive cell
                if (board_[w_ * (i + 1) + j] - '0') {
                    temp_board_[w_ * i + j] =
                        (neighbors < 2 || neighbors > 3) ? '0' : '1';
                } else {
                    temp_board_[w_ * i + j] = neighbors == 3 ? '1' : '0';
                }
            } // for (int j = 0; ...)
        } // for (int i = 0; ...)

        // since new values were calculated and stored in the temporary buffer
        // we move them back to the main board.
        std::memcpy(board_address_first_line(), temp_board_, w_ * h_);
    }

    /** Class destructor. */
    ~Board() {
        delete[] board_;
        delete[] temp_board_;
    }
};

/** Print program usage. */
void usage() {
    std::cout << "Usage:\n"
                 "./life <filename> <iterations>"
              << std::endl;
}

/**
 * @brief Define first process task.
 *
 * @param iters     Number of iterations.
 * @param w         Width of the board.
 * @param h_total   Height of the whole board.
 * @param h_proc    Height of the chunk board.
 * @param input     Content of the file given.
 */
void first_rank_task(const int iters, const int w, const int h_total,
                     const int h_proc, const char *input) {
    Board board(w, h_proc);
    char *print_board = new char[w * h_total]();
    char *line = new char[w + 1]();
    std::memset(line, 0, w + 1);

    // distribute data
    MPI_Scatter(input, w * h_proc, MPI_CHAR, board.board_address_first_line(),
                w * h_proc, MPI_CHAR, 0, MPI_COMM_WORLD);

    MPI_Status status;

    // main loop
    for (int iter = 0; iter < iters; iter++) {
        // send data to previous and next ranks
        MPI_Send(board.board_address_last_line(), board.w(), MPI_CHAR, 1,
                 TO_TOP_PADD, MPI_COMM_WORLD);

        // receive data from previous and next rank
        MPI_Recv(board.board_address_bottom_padding(), board.w(), MPI_CHAR, 1,
                 TO_BOTTOM_PADD, MPI_COMM_WORLD, &status);

        // calculate
        board.update_board();

        // gather from all ranks
        // moved into the main loop for debugging purposes
        MPI_Gather(board.board_address_first_line(), w * h_proc, MPI_CHAR,
                   print_board, w * h_proc, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    // print buffer
    int offset = 0;
    for (int i = 0; i < h_total; i++, offset += w) {
        std::memcpy(line, print_board + offset, w);
        std::cout << i / h_proc << ": " << line << std::endl;
    }

    delete[] line;
    delete[] print_board;
}

/**
 * @brief Define middle process task.
 *
 * @param rank      Process rank number.
 * @param iters     Number of iterations.
 */
void middle_rank_task(const int rank, const int iters) {
    // get dimensions
    int h, w;
    MPI_Bcast(&h, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&w, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int buffer_size = w * h;

    Board board(w, h);

    // load board
    MPI_Scatter(nullptr, buffer_size, MPI_CHAR,
                board.board_address_first_line(), buffer_size, MPI_CHAR, 0,
                MPI_COMM_WORLD);

    MPI_Status status;

    // main loop
    for (int iter = 0; iter < iters; iter++) {
        // send data to previous and next ranks
        MPI_Send(board.board_address_first_line(), board.w(), MPI_CHAR,
                 rank - 1, TO_BOTTOM_PADD, MPI_COMM_WORLD);
        MPI_Send(board.board_address_last_line(), board.w(), MPI_CHAR, rank + 1,
                 TO_TOP_PADD, MPI_COMM_WORLD);

        // receive data from previous and next rank
        MPI_Recv(board.board_address_top_padding(), board.w(), MPI_CHAR,
                 rank - 1, TO_TOP_PADD, MPI_COMM_WORLD, &status);
        MPI_Recv(board.board_address_bottom_padding(), board.w(), MPI_CHAR,
                 rank + 1, TO_BOTTOM_PADD, MPI_COMM_WORLD, &status);

        // calculate
        board.update_board();

        // send data to rank 0
        MPI_Gather(board.board_address_first_line(), buffer_size, MPI_CHAR,
                   nullptr, buffer_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
}

/**
 * @brief Define last process task.
 *
 * @param rank      Process rank number.
 * @param iters     Number of iterations.
 */
void last_rank_task(const int rank, const int iters) {
    // get dimensions
    int h, w;
    MPI_Bcast(&h, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&w, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int buffer_size = w * h;

    Board board(w, h);

    // load board
    MPI_Scatter(nullptr, buffer_size, MPI_CHAR,
                board.board_address_first_line(), buffer_size, MPI_CHAR, 0,
                MPI_COMM_WORLD);

    MPI_Status status;

    // main loop
    for (int iter = 0; iter < iters; iter++) {
        // send data to previous and next ranks
        MPI_Send(board.board_address_first_line(), board.w(), MPI_CHAR,
                 rank - 1, TO_BOTTOM_PADD, MPI_COMM_WORLD);

        // receive data from previous and next rank
        MPI_Recv(board.board_address_top_padding(), board.w(), MPI_CHAR,
                 rank - 1, TO_TOP_PADD, MPI_COMM_WORLD, &status);

        // calculate
        board.update_board();

        // send data to rank 0
        MPI_Gather(board.board_address_first_line(), buffer_size, MPI_CHAR,
                   nullptr, buffer_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[]) {
    // check number of arguments
    if (argc < 3) {
        std::cerr << "Missing arguments!" << std::endl;
        usage();
        return EXIT_FAILURE;
    }

    int rank, size, iters = 0;
    // get number of iterations
    try {
        iters = std::stoi(argv[2]);
    } catch (...) {
        std::cerr << "Second argument must be a number";
        return EXIT_FAILURE;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int w = 0, h = 0, h_proc;
    std::string input_content("");

    // read file and distribute board dimensions
    if (rank == 0) {
        std::ifstream file(argv[1]);
        std::string temp;
        for (int i = 0; getline(file, temp); i++) {
            if (i == 0) {
                w = h = temp.size();
            }
            input_content.append(temp);
        }
        // calculate number of lines per process
        h_proc = h / MIN(size, h);

        // broadcast dimensions
        MPI_Bcast(&h_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&w, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // TODO: calculate for small files or 
    //       when data cannot be evenly distributed (last rank will have more/less data than others)
    if (rank == 0) {
        first_rank_task(iters, w, h, h_proc, input_content.c_str());
    } else if (rank == size - 1) {
        last_rank_task(rank, iters);
    } else {
        // calculate last rank height (if misaligned)
        middle_rank_task(rank, iters);
    }
    MPI_Finalize();
}
/* life.cpp */
