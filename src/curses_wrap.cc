#include <utility>
#include "curses_wrap.hh"
#include <curses.h>

namespace curses {


    void initialize() {
        initscr();
        raw();
        // cbreak(); // use to enable ctrl-c
        noecho();
        keypad(stdscr, 1);
        start_color();

        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_RED, COLOR_RED);
    }

    bool is_backspace(const int ch){
        return  ch == KEY_BACKSPACE || ch == '\b' || ch == 127;
    }

    bool is_enter(const int ch){
        return ch == KEY_ENTER || ch == '\n' || ch == 10;
    }

    std::pair<int, int> get_mid(int y_offset, int x_offset){
        int y, x;
        getmaxyx(stdscr, y, x);
        return {(y / 2) - y_offset, (x / 2) - x_offset};
    }

    void printnm(const int y, const int x, const std::string_view& str){
        int old_y, old_x;
        getyx(stdscr, old_y, old_x);
        move(y, x);
        clrtoeol();
        printw(str.data());
        move(old_y, old_x);
    }

    void print_begin(const int y, const int x, const std::string_view& str){
        move(y, x);
        clrtoeol();
        printw(str.data());
        move(y, x);
    }

} /* curses */
