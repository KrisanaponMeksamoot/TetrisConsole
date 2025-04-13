// SPDX-License-Identifier: MIT

#include <unistd.h>
#include <ncurses.h>
#include "utils.hpp"
#include "blocks.hpp"
#include <cmath>
#include <ctime>
#include <cstdlib>

enum CP {
    CP_WB = 10,
    CP_BW = 11,
    CP_RB = 12,
    CP_C = 1,
    CP_B = 2,
    CP_O = 3,
    CP_Y = 4,
    CP_L = 5,
    CP_P = 6,
    CP_R = 7
};

void init_padlette() {
    start_color();

    init_pair(CP_WB, COLOR_WHITE, COLOR_BLACK);
    init_pair(CP_BW, COLOR_BLACK, COLOR_WHITE);
    init_pair(CP_RB, COLOR_RED, COLOR_BLACK);
    init_pair(CP_C, 0, COLOR_CYAN);
    init_pair(CP_B, 0, COLOR_BLUE);
    init_pair(CP_O, 0, COLOR_WHITE);
    init_pair(CP_Y, 0, COLOR_YELLOW);
    init_pair(CP_L, 0, COLOR_GREEN);
    init_pair(CP_P, 0, COLOR_MAGENTA);
    init_pair(CP_R, 0, COLOR_RED);
}


class NcursesWindowCanvas : public Canvas {
    protected:
    WINDOW *window;
    public:
    NcursesWindowCanvas(WINDOW * window) {
        this->window = window;
    }
    void drawpixel(int y, int x, int color) {
        wattron(this->window, COLOR_PAIR(color));
        mvwaddch(this->window, y, x, ' ');
        wattroff(this->window, COLOR_PAIR(color));
    }
};

int main() {
    initscr();
    nodelay(stdscr, true);
    noecho();
    cbreak();

    int mx, my;
    getmaxyx(stdscr, my, mx);

    curs_set(0);

    int winsp_x = (mx - 12)/2, winsp_y = std::max((my - 22)/2, 0);
    WINDOW *win_space = newwin(22, 12, winsp_y, winsp_x);
    WINDOW *win_next = newwin(17, 6, winsp_y, winsp_x+12);

    refresh();

    init_padlette();

    wattron(win_next, COLOR_PAIR(CP_WB));
    box(win_next, 0, 0);
    mvwprintw(win_next, 0, 1, "Next");

    wattron(win_space, COLOR_PAIR(CP_WB));
    box(win_space, 0, 0);
    mvwprintw(win_space, 0, 1, "Tetris");
    
    NcursesWindowCanvas canvas_space(win_space);
    NcursesWindowCanvas canvas_next(win_next);

    char buf_space[20][10]{};

    srand(time(NULL));
    int next_blocks[4];
    for (int i=0;i<4;i++)
        next_blocks[i] = rand() % TBlock::n_B;
    for (int i=1;i<16;i++)
        mvwprintw(win_next, i, 1, "    ");
    for (int i=0;i<4;i++) {
        TBlock::draw(&canvas_next, i*4+1, 1, next_blocks[i], 0);
    }
    wrefresh(win_next);

    int next_block = rand() % TBlock::n_B;
    int next_block_ori = 0;
    int next_block_x = 4, next_block_y = 0;

    int time_cnt = 0;

    bool running = true;
    while (running) {
        int ch = getch();

        if (ch == CH_ESC) {
            ch = getch();
            if (ch == '[') {
                ch = getch();
                switch (ch) {
                    case 'A': {
                        int tmp = mod(next_block_ori+1, TBlock::max_ori(next_block));
                        if (!TBlock::is_collide(buf_space, next_block_y, next_block_x, next_block, tmp))
                            next_block_ori = tmp;
                    } break;
                    case 'B': {
                        time_cnt = 1000;
                    } break;
                    case 'C': {
                        if (!TBlock::is_collide(buf_space, next_block_y, next_block_x+1, next_block, next_block_ori))
                            next_block_x++;
                    } break;
                    case 'D': {
                        if (!TBlock::is_collide(buf_space, next_block_y, next_block_x-1, next_block, next_block_ori))
                            next_block_x--;
                    } break;
                }
            } else {
                running = false;
            }
        }

        if (time_cnt >= 1000) {
            if (TBlock::is_collide(buf_space, next_block_y+1, next_block_x, next_block, next_block_ori)) {
                if (next_block_y == 0) {
                    wattron(win_space, COLOR_PAIR(CP_RB));
                    mvwprintw(win_space, 9, 1, "You died!");
                    wattroff(win_space, COLOR_PAIR(CP_RB));
                    wrefresh(win_space);
                    nodelay(stdscr, false);
                    while (true) {
                        int ch = getch();
                        if (ch == CH_ESC) {
                            nodelay(stdscr, true);
                            if (getch() == ERR) {
                                running = false;
                                break;
                            }
                            nodelay(stdscr, false);
                        } else if (ch == CH_SPACE || ch == CH_ENTER) {
                            // reset
                            next_block = rand() % TBlock::n_B;
                            next_block_ori = 0;
                            next_block_x = 4;
                            next_block_y = 0;
                            for (int i=0;i<4;i++)
                                next_blocks[i] = rand() % TBlock::n_B;
                            for (int i=0;i<20;i++) {
                                for (int j=0;j<10;j++) {
                                    buf_space[i][j] = 0;
                                }
                            }

                            for (int i=1;i<16;i++)
                                mvwprintw(win_next, i, 1, "    ");
                            for (int i=0;i<4;i++) {
                                TBlock::draw(&canvas_next, i*4+1, 1, next_blocks[i], 0);
                            }
                            wrefresh(win_next);
                            break;
                        }
                    }
                    nodelay(stdscr, true);
                } else {
                    TBlock::draw(buf_space, next_block_y, next_block_x, next_block, next_block_ori);
                    for (int i=19;i>=0;i--) {
                        bool a = true;
                        for (int j=0;j<10;j++) {
                            if (!buf_space[i][j]) {
                                a = false;
                                break;
                            }
                        }
                        if (a) {
                            for (int j=0;j<10;j++) {
                                for (int k=i-1;k>=0;k--) {
                                    buf_space[k+1][j] = buf_space[k][j];
                                }
                                buf_space[0][j] = 0;
                            }
                            i++;
                        }
                    }

                    next_block = next_blocks[0];
                    next_blocks[0] = next_blocks[1];
                    next_blocks[1] = next_blocks[2];
                    next_blocks[2] = next_blocks[3];
                    next_blocks[3] = rand() % TBlock::n_B;
                    next_block_ori = 0;
                    next_block_x = 4;
                    next_block_y = 0;
                    for (int i=1;i<16;i++)
                        mvwprintw(win_next, i, 1, "    ");
                    for (int i=0;i<4;i++) {
                        TBlock::draw(&canvas_next, i*4+1, 1, next_blocks[i], 0);
                    }
                    wrefresh(win_next);
                }
            } else {
                next_block_y++;
            }

            time_cnt = 0;
        }

        { // draw space
            for (int i=0;i<20;i++) {
                for (int j=0;j<10;j++) {
                    canvas_space.drawpixel(i+1, j+1, buf_space[i][j]);
                }
            }
        }

        TBlock::draw(&canvas_space, next_block_y+1, next_block_x+1, next_block, next_block_ori);

        wrefresh(win_space);
        usleep(1000);
        time_cnt++;
    }

    delwin(win_space);
    delwin(win_next);
    refresh();
    endwin();

    return 0;
}