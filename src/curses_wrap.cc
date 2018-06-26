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

        // separate into new function?
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

    bool is_f1(const int ch){
        return ch == KEY_F(1);
    }

    std::pair<int, int> get_mid(int y_offset, int x_offset){
        int y, x;
        getmaxyx(stdscr, y, x);
        return {(y / 2) - y_offset, (x / 2) - x_offset};
    }

    std::pair<int, int> get_pos(){
        int y, x;
        getyx(stdscr, y, x);
        return {y, x};
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

    int get_char(){
        return getch();
    }

    void move_to(int y, int x){
        move(y, x);
    }

    void add_char(int ch){
        addch(ch);
    }

    int get_lines(){
        return LINES;
    }

    void end_win(){
        endwin();
    }

} /* curses */
