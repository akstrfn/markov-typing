#include <vector>
#include <iterator>
#include <iostream>
#include <ncurses.h>

std::string sentence = "This is a test sentence";
std::string typed = "";
int row, col;
int y = -1, x = -1;
int mid_y, mid_x;
int ch;

//int main(int argc, char *argv[])
int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_RED);

    getmaxyx(stdscr, row, col);
    mid_x = (col - std::size(sentence))/2;
    mid_y = row/2;

    mvprintw(mid_y, mid_x, "%s", sentence.c_str());

    move(mid_y, mid_x);
    int old_x, old_y;
    while(typed.size() != std::size(sentence)){
        getyx(stdscr, y, x);
        // for debugging
        old_x = x;
        old_y = y;
        move(LINES - 2, 2);
        clrtobot();
        printw("Typed: %s", typed.c_str());
        move(old_y, old_x);

        ch = getch();
        if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127 || ch == KEY_DC) {
            if (x != mid_x){
                move(y, --x);
                addch(sentence[x - mid_x]);
                move(y, x);
                typed.pop_back();
            }
        } else if (sentence[x - mid_x] == ch) {
            addch(ch | COLOR_PAIR(2));
            typed.push_back((int)ch);
        } else {
            if (ch == ' ') 
                addch(ch | COLOR_PAIR(3));
            else 
                addch(ch | COLOR_PAIR(1));
            typed.push_back((int)ch);
        }
    }
    //refresh();
    while(getch() != KEY_F(1)){ }
    endwin();
    return 0;
}
