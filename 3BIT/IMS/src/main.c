/**
 * @brief File containing starting point of the application.
 *
 * This source code serves as submission for
 * a project of class IMS at FIT, BUT 2022/23.
 *
 * @file main.c
 * @authors Hung Do (xdohun00)
 *          Marek Dohnal (xdohna48)
 * @date 05/12/2022
 */
// defining this macro in order to enable getopt in -std=c11
#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 2
#endif

#include <ncurses.h>
#include <stdio.h>      // printf
#include <stdlib.h>     // srand
#include <time.h>       // time_t, time
#include <unistd.h>     // getopt
#include "board.h"

/**
 * @brief Process program arguments.
 */
static void load_args(struct board_t *b, int argc, char * const argv[]) {
    int opt = 0, val = 0;
    while ((opt = getopt(argc, argv, "i:n:")) != -1) {
        switch (opt) {
            case 'i':
                val = strtol(optarg, NULL, 10);
                b->max_iter = (val <= 0) ? -1 : val;
                break;
            case 'n':
                val = strtol(optarg, NULL, 10);
                b->nutrition_val = (val > 1) ? val : 1;
                break;
        }
    }
}
/**
 * @brief Set up usable colors.
 */
static void setup_colors(struct board_t *b) {
    // defining new colors
    // list of custom colors are listed in enum custom_color_t
    DEFINE_COLOR(CC_LIGHT_GRAY, 225, 225, 225);
    DEFINE_COLOR(CC_RED, 246, 65, 45);
    DEFINE_COLOR(CC_ORANGE, 255, 152, 0);
    DEFINE_COLOR(CC_YELLOW, 255, 236, 25);
    DEFINE_COLOR(CC_ALMOST_BLACK, 10, 10, 10);

    // setup color pairs
    // syntax:  COLOR_LABEL,        COLOR_FONT,  COLOR_BACKGROUND
    init_pair(CELL_DEAD+1,          COLOR_WHITE, CC_ALMOST_BLACK);
    init_pair(CELL_ALIVE+1,         COLOR_BLACK, COLOR_CYAN);
    init_pair(CELL_ALIVE_AGE2+1,    COLOR_BLACK, COLOR_CYAN);
    init_pair(CELL_ALIVE_AGE3+1,    COLOR_BLACK, COLOR_CYAN);
    init_pair(CELL_ALIVE_AGE4+1,    COLOR_BLACK, COLOR_CYAN);
    init_pair(CELL_DIFF_AGE1+1,     COLOR_BLACK, CC_YELLOW);
    init_pair(CELL_DIFF_AGE2+1,     COLOR_BLACK, CC_YELLOW);
    init_pair(CELL_DIFF_AGE3+1,     COLOR_BLACK, CC_YELLOW);
    init_pair(CELL_DIFF_AGE4+1,     COLOR_BLACK, CC_YELLOW);
    init_pair(CELL_CONIDIA_AGE1+1,  COLOR_BLACK, CC_ORANGE);
    init_pair(CELL_CONIDIA_AGE2+1,  COLOR_BLACK, CC_ORANGE);
    init_pair(CELL_CONIDIA_AGE3+1,  COLOR_BLACK, CC_ORANGE);
    init_pair(CELL_CONIDIA_AGE4+1,  COLOR_BLACK, CC_ORANGE);
    init_pair(CELL_MATURE+1,        COLOR_BLACK, CC_RED);
    init_pair(CELL_UNHABITED+1,     COLOR_BLACK, CC_ALMOST_BLACK);

    // dynamic setup
    for (int i = 0; i < b->nutrition_val; i++) {
        init_pair(CELL_AGE_START + i+1, COLOR_WHITE, CC_ALMOST_BLACK);
    }
}

/**
 * @brief Initial ncurses setup.
 */
static void init_setup(struct board_t *b) {
    cbreak();                   // disable line buffering
    noecho();                   // user typed characters are not echoed
    keypad(stdscr, TRUE);       // support F1-F12 keys
    timeout(5);                 // non-blocking reading
    curs_set(0);                // hide cursor

    // setup colors if
    if (has_colors() == TRUE) {
        start_color();
        setup_colors(b);
    }
}

int main(int argc, char * const *argv) {
    int pause = 0;          // status variable; is iteration paused?
    int ch = 0;             // received user input
    int timer = 0;          // current timer's value
    int timer_limit = 50;   // after timer hits this value, make an iteration
    time_t t1;
    int max_x, max_y;
    srand((unsigned)time(&t1));

    struct board_t b = { 0, 0, 0, 1, NULL, NULL };
    if (init_board(&b))
        goto err_board;

    initscr();
    load_args(&b, argc, argv);
    init_setup(&b);

    // print starting pos
    display_board(&b);

    do {
        ch = getch();
        getmaxyx(stdscr, max_y, max_x);
        switch (ch) {
            case KEY_UP:    // show what is above highest row
                if (pos_y + 1 <= 0) {
                    pos_y++;
                    if (!pause) {
                        display_board(&b);
                        continue;
                    }
                }
                break;
            case KEY_DOWN:  // show what is below lowest row
                if (pos_y - 1 + BOARD_HEIGHT >= max_y - 2) {
                    pos_y--;
                    if (!pause) {
                        display_board(&b);
                        continue;
                    }
                }
                break;
            case KEY_LEFT:  // show what is on the left
                if (pos_x + 1 <= 0) {
                    pos_x++;
                    if (!pause) {
                        display_board(&b);
                        continue;
                    }
                }
                break;
            case KEY_RIGHT: // show what is on the right
                if (pos_x - 1 + BOARD_WIDTH >= max_x - 2) {
                    pos_x--;
                    if (!pause) {
                        display_board(&b);
                        continue;
                    }
                }
                break;

            case '+':   // increase speed
                timer_limit = MAX(0, timer_limit-5);
                break;
            case '-':   // decrease speed
                timer_limit = MIN(100, timer_limit+5);
                break;
            case 'k':
                if (pause) {
                    update_board(&b);
                    display_board(&b);
                    timer = 0;
                }
                continue;

            case ' ':   // pause/resume iteration
                if (!pause)
                    timeout(-1);
                else
                    timeout(5);
                pause = !pause;
                break;
            default:
                break;
        }

        if (timer < timer_limit) {
            timer++;
            continue;
        }

        // update board when timer limit is reached
        update_board(&b);
        display_board(&b);
        timer = 0;
        iteration++;
    } while (ch != 'q' && (b.max_iter == -1 || iteration < b.max_iter));

    // freeze before exit
    timeout(-1);
    getch();

    endwin();
    destroy_board(&b);
    printf("Number of iterations: %d\n"
           "Number of cells alive: %d\n"
           "Number of cells uninhabitable: %d\n"
           "Alive to uninhabitable ratio: %.4f\n", iteration, b.alive_counter,
                b.uninhabitable_counter, b.alive_counter/(b.uninhabitable_counter*1.0));
    return 0;
// handle when board could not be created.
err_board:
    return 1;
}

/* main.c */

