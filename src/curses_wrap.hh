#include <string>
#include <array>

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

enum Colors {
    RedBlack = COLOR_PAIR(1),
    GreenBlack = COLOR_PAIR(2),
    RedRed = COLOR_PAIR(3),
};

bool is_backspace(const int ch){
    return  ch == KEY_BACKSPACE || ch == '\b' || ch == 127;
}

bool is_enter(const int ch){
    return ch == KEY_ENTER || ch == '\n' || ch == 10;
}

auto get_mid(WINDOW* src, int y_offset=0, int x_offset=0){
    int y, x;
    getmaxyx(src, y, x);
    return std::pair{(y / 2) - y_offset, (x / 2) - x_offset};
}

// print somewhere in the screen and return the cursor to the original position
// print_no_move
void printnm(const int y, const int x, const std::string_view& str){
    int old_y, old_x;
    getyx(stdscr, old_y, old_x);
    move(y, x);
    clrtoeol();
    printw(str.data());
    move(old_y, old_x);
}

// print and go to position from which you started printing
void print_begin(const int y, const int x, const std::string_view& str){
    move(y, x);
    clrtoeol();
    printw(str.data());
    move(y, x);
}

} /* curses */ 
