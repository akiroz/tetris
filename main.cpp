#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include "game.cpp"

int frameDelay = 1'000'000/60;
int tickDelay = 1'000'000/frameDelay;

void draw(WINDOW* win) {
    wclear(win);
    for(char i = 0; i < 4; i++) {
        for(char p = 0; p < 4; p++) {
            if(i == 0) {
                Tetris::XY c = Tetris::piece[i].pos[p];
                if(c.y >= 0) mvwaddch(win, c.y, c.x, '@');
            } else {
                char y = (i-1) * 3 + 1;
                Tetris::XY c = Tetris::piece[i].pos[p];
                mvwaddch(win, y + c.y, 10 + c.x, '@');
            }
        }
    }
    for(char y = 0; y < Tetris::fieldHeight; y++) {
        for(char x = 0; x < Tetris::fieldWidth; x++) {
            if(Tetris::field[y][x]) mvwaddch(win, y, x, '#');
            
        }
    }
    wmove(win, 0, 14);
    wrefresh(win);
}

int main() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    signal(SIGTERM, [](int signum) {
        endwin();
        exit(0);
    });

    WINDOW* win = newwin(Tetris::fieldHeight, 15, 0, 0);
    Tetris::init(time(NULL));
    draw(win);

    bool run = true;
    while(run) {
        bool redraw = true;
        int in = getch();

             if(in == 'a') Tetris::slide(true);
        else if(in == 'd') Tetris::slide(false);
        else if(in == 'w') run = Tetris::drop();
        else if(in == 's') run = Tetris::tick();
        else if(in == 'q') Tetris::rotate(true);
        else if(in == 'e') Tetris::rotate(false);
        else if(--tickDelay <= 0) {
            tickDelay = 1'000'000/frameDelay;
            run = Tetris::tick();
        } else {
            redraw = false;
        }

        if(redraw) draw(win);
        usleep(frameDelay);
    }

    puts("Game Over\n");
    return 0;
}
