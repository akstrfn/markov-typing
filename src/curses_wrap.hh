#ifndef CURSES_WRAP_HH
#define CURSES_WRAP_HH

#include <string>


namespace curses {
        
#include <curses.h>

    // TODO how to isolate this one and curses header from this header and move
    // it to curses_wrap.cc?
    enum Colors {
        RedBlack = COLOR_PAIR(1),
        GreenBlack = COLOR_PAIR(2),
        RedRed = COLOR_PAIR(3),
    };

    void initialize();


    bool is_backspace(const int ch);
    bool is_enter(const int ch);

    std::pair<int, int> get_mid(int y_offset, int x_offset);

    // print somewhere in the screen and return the cursor to the original
    // position print_no_move
    void printnm(const int y, const int x, const std::string_view& str);

    // print and go to position from which you started printing
    void print_begin(const int y, const int x, const std::string_view& str);

} /* curses */ 

#endif /* ifndef CURSES_WRAP_HH */
