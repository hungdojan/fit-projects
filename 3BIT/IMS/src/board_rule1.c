/**
 * @brief Module defining rules for Model 1.
 *
 * This source code serves as submission for
 * a project of class IMS at FIT, BUT 2022/23.
 *
 * @file board_rule1.c
 * @authors Hung Do (xdohun00)
 *          Marek Dohnal (xdohna48)
 * @date 05/12/2022
 */
#include "board.h"
#include <stdlib.h>
#include <time.h>

static int nof_neighbors_row(struct board_t *b, int y, int x) {
    int count = 0;
    for (int i = MAX(0, x-1); i <= MIN(x+1, BOARD_WIDTH - 1); i++) {
        if (b->front[y * BOARD_WIDTH + i] == CELL_ALIVE)
            count++;
    }
    return count;
}

static int nof_neighbors(struct board_t *b, int y, int x) {
    int count = 0;
    // upper row
    if (y > 0) {
        count += nof_neighbors_row(b, y-1, x);
    }

    // middle row
    count += nof_neighbors_row(b, y, x);
    if (b->front[y * BOARD_WIDTH + x] == CELL_ALIVE)
        count--;

    // lower row
    if (y < BOARD_HEIGHT - 1) {
        count += nof_neighbors_row(b, y+1, x);
    }
    return count;
}

void apply_rules(struct board_t *b) {
    // height
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        // column
        for (int j = 0; j < BOARD_WIDTH; j++) {
            int pos = i * BOARD_WIDTH + j;
            int alive_neighbors = nof_neighbors(b, i, j);

            // UNOCCUPIED CELL
            if (b->front[pos] == CELL_DEAD) {
                switch(alive_neighbors) {
                    case 1:
                        b->back[pos] = (RAND(1000) < 125) ? CELL_ALIVE : b->front[pos];
                        if (b->back[i * BOARD_WIDTH + j] == CELL_ALIVE)
                            b->alive_counter++;
                        break;
                    case 2:
                        // b->back[i * BOARD_WIDTH + j] = (r < 250) ? CELL_ALIVE : CELL_DEAD;
                        // break;
                    case 3:
                        // b->back[i * BOARD_WIDTH + j] = (r < 500) ? CELL_ALIVE : CELL_DEAD;
                        // break;
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                        b->back[pos] = b->front[pos];
                        break;
                }
            } else if (b->front[pos] == CELL_ALIVE) {
                b->back[pos] = b->front[pos];
            } // if b->front[x] == CELL_DEAD
        } // for (int j ...)
    } // for (int i ...)
}

/* board_rule1.c */
