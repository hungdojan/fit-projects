/**
 * Solution for the first assignment of the class PRL at FIT, BUT 2023/2024.
 *
 * @author  Hung Do
 * @date    03/03/2024
 * @file    pms.cpp
 */
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <queue>

#define NUM_FILE_PATH "numbers"

enum QueueTags {
    QUEUE_TAGS_TOP,    // Top queue
    QUEUE_TAGS_BOTTOM, // Bottom queue
    QUEUE_TAGS_SIZE,
    QUEUE_TAGS_CONNECTION_END = 255,
};

struct Counters {
    int top{0};
    int bottom{0};
    int total{0};
};

/**
 * Task definition for the first process.
 *
 * @param size  Total number of process.
 */
static void first_rank_task(const int size) {
    int dst_queue{QUEUE_TAGS_TOP}, data;
    const int SRC_RANK = 0;
    const int dst_rank = (size != 1) ? SRC_RANK + 1 : SRC_RANK;

    // file reading
    std::ifstream nums_file(NUM_FILE_PATH, std::ios::binary);
    // check if file can be read
    if (nums_file.fail()) {
        std::cerr << "File could not be open" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    while (nums_file) {
        data = nums_file.get();
        if (data != EOF) {
            // send data to following rank and update destination queue
            MPI_Send(&data, 1, MPI_INT, dst_rank, dst_queue, MPI_COMM_WORLD);
            dst_queue = (dst_queue + 1) % QUEUE_TAGS_SIZE;
            std::cout << +data << " ";
        } else {
            // close the connection
            MPI_Send(&data, 1, MPI_INT, dst_rank, QUEUE_TAGS_CONNECTION_END,
                     MPI_COMM_WORLD);
            std::cout << std::endl;
        }
    }
    // notify the last process that it can start printing sorted numbers
    MPI_Send(nullptr, 0, MPI_INT, size - 1, QUEUE_TAGS_CONNECTION_END,
             MPI_COMM_WORLD);
}

/**
 * Get greater value from the queue fronts.
 *
 * The value is popped from the queue and counters are updated accordingly.
 *
 * @param queue_size    Max number of files that could be popped from the queue.
 * @param top_queue     The top queue.
 * @param bottom_queue  The bottom queue.
 * @param counters      Structure containing counter values.
 * @return int          The greater value of queue fronts.
 */
static int get_greater_num(const int queue_size, std::queue<int> &top_queue,
                           std::queue<int> &bottom_queue, Counters &counters) {
    int send_data;
    if (counters.top == queue_size && counters.bottom != queue_size) {
        // flush bottom queue
        send_data = bottom_queue.front();
        bottom_queue.pop();
        counters.bottom++;
    } else if (counters.bottom == queue_size && counters.top != queue_size) {
        // flush top queue
        send_data = top_queue.front();
        top_queue.pop();
        counters.top++;
    } else if (top_queue.size() == 0 && bottom_queue.size() != 0) {
        // flush bottom queue for the last batch
        send_data = bottom_queue.front();
        bottom_queue.pop();
        counters.bottom++;
    } else if (top_queue.size() != 0 && bottom_queue.size() == 0) {
        // flush top queue for the last batch
        send_data = top_queue.front();
        top_queue.pop();
        counters.top++;
    } else {
        // compare and send greater number
        if (top_queue.front() < bottom_queue.front()) {
            send_data = top_queue.front();
            top_queue.pop();
            counters.top++;
        } else {
            send_data = bottom_queue.front();
            bottom_queue.pop();
            counters.bottom++;
        }
    }
    return send_data;
}

/**
 * Task definition for middle processes.
 *
 * @param rank  Process index.
 */
static void middle_rank_task(const int rank) {
    // top and bottom queue for a process
    std::queue<int> top_queue, bottom_queue;

    // connection variables
    int dst_queue{QUEUE_TAGS_TOP};
    int recv_data, send_data;
    MPI_Status status{.MPI_TAG = dst_queue};

    // manage counters
    Counters counters;
    // number of data that will be sorted in one batch (== 2*QUEUE_SIZE)
    const int BATCH_SIZE = 1 << rank;
    // max size for one queue
    const int QUEUE_SIZE = 1 << (rank - 1);

    do {
        // processing incoming data
        if (status.MPI_TAG != QUEUE_TAGS_CONNECTION_END) {
            MPI_Recv(&recv_data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == QUEUE_TAGS_TOP)
                top_queue.push(recv_data);
            else if (status.MPI_TAG == QUEUE_TAGS_BOTTOM)
                bottom_queue.push(recv_data);
        }

        // skip iteration if both queues are empty (there is nothing to sort)
        if (top_queue.size() == 0 && bottom_queue.size() == 0)
            continue;

        // unless all numbers were sent we need to check for the default
        // condition
        if (status.MPI_TAG != QUEUE_TAGS_CONNECTION_END) {
            if (counters.total == 0) {
                // check the condition that one of the queue must contain
                // 2**(i-1) and the second one at least 1 element
                if (!((top_queue.size() >= QUEUE_SIZE &&
                       bottom_queue.size() > 0) ||
                      (bottom_queue.size() >= QUEUE_SIZE &&
                       top_queue.size() > 0)))
                    continue;
            }
        }

        send_data =
            get_greater_num(QUEUE_SIZE, top_queue, bottom_queue, counters);

        // process outgoing data
        MPI_Send(&send_data, 1, MPI_INT, rank + 1, dst_queue, MPI_COMM_WORLD);
        counters.total++;
        // update destination queue
        if (counters.total >= BATCH_SIZE) {
            // reset counters
            counters.total = 0;
            counters.top = 0;
            counters.bottom = 0;
            dst_queue = (dst_queue + 1) % QUEUE_TAGS_SIZE;
        }

    } while (!(top_queue.size() == 0 && bottom_queue.size() == 0 &&
               status.MPI_TAG == QUEUE_TAGS_CONNECTION_END));
    // end connection
    MPI_Send(nullptr, 0, MPI_INT, rank + 1, QUEUE_TAGS_CONNECTION_END,
             MPI_COMM_WORLD);
}

/**
 * Task definition for the last process.
 *
 * @param rank  Process index.
 */
static void last_rank_task(const int rank) {
    MPI_Status status;

    // top and bottom queue for a process
    std::queue<int> top_queue, bottom_queue;

    // connection variables
    int recv_data, print_data;

    // manage counters
    Counters counters;
    // max size of one of the queue
    const int QUEUE_SIZE = 1 << (rank - 1);

    // for sorting only 2 numbers -> first rank sends data to last rank
    do {
        // processing incoming data
        MPI_Recv(&recv_data, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
                 &status);
        if (status.MPI_TAG == QUEUE_TAGS_TOP)
            top_queue.push(recv_data);
        else if (status.MPI_TAG == QUEUE_TAGS_BOTTOM)
            bottom_queue.push(recv_data);

    } while (status.MPI_TAG != QUEUE_TAGS_CONNECTION_END);

    // check if data were sent by the main process (rank 0)
    if (top_queue.size() == 0 && bottom_queue.size() == 0)
        status.MPI_TAG = 0;

    do {
        // processing incoming data
        if (status.MPI_TAG != QUEUE_TAGS_CONNECTION_END) {
            MPI_Recv(&recv_data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == QUEUE_TAGS_TOP)
                top_queue.push(recv_data);
            else if (status.MPI_TAG == QUEUE_TAGS_BOTTOM)
                bottom_queue.push(recv_data);
        }

        // skip iteration if both queues are empty (there is nothing to sort)
        if (top_queue.size() == 0 && bottom_queue.size() == 0)
            continue;

        // unless all numbers were sent we need to check for the default
        // condition
        if (status.MPI_TAG != QUEUE_TAGS_CONNECTION_END) {
            if (counters.total == 0) {
                // check the condition that one of the queue must contain
                // 2**(i-1) and the second one at least 1 element
                if (!((top_queue.size() >= QUEUE_SIZE &&
                       bottom_queue.size() > 0) ||
                      (bottom_queue.size() >= QUEUE_SIZE &&
                       top_queue.size() > 0)))
                    continue;
            }
        }

        print_data =
            get_greater_num(QUEUE_SIZE, top_queue, bottom_queue, counters);

        // process outgoing data
        std::cout << print_data << std::endl;
        counters.total++;
    } while (!(top_queue.size() == 0 && bottom_queue.size() == 0 &&
               status.MPI_TAG == QUEUE_TAGS_CONNECTION_END));
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        first_rank_task(size);
    }
    if (rank == size - 1) {
        last_rank_task(rank);
    }
    if (rank != 0 && rank != size - 1) {
        middle_rank_task(rank);
    }

    MPI_Finalize();
}
/* End of file pms.cpp */
