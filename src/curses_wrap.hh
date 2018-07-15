#ifndef CURSES_WRAP_HH
#define CURSES_WRAP_HH

#include <array>
#include <string>

// TODO I should wrap WINDOW into its own class... and char...
namespace curses {

class CursesRuntime {
    void initialize_curses();
    void end_curses();

public:
    CursesRuntime();
    ~CursesRuntime();
};

class NChar {
    int ch;
    int attribute;

public:
    uint8_t color;

    NChar();
    NChar(int);
    bool is_backspace();
    bool is_enter();
    bool is_f4();
    NChar &operator=(NChar);
    NChar &operator=(int);
    int data();
};

// TODO Colors could be more generalized like this but I'm not sure if its
// worth it.
enum Colors {
    // COLOR_PAIR from ncurses source translates to this:
    // COLOR_PAIR(color_number) == static_cast<unsigned>(color_number) << 8 &
    // A_COLOR
    // where A_COLOR == static_cast<unsigned>(((1U << 8) - 1U)) << 8
    RedBlack = 1u << 8 & 0b1111111100000000,
    GreenBlack = 2u << 8 & 0b1111111100000000,
    RedRed = 3u << 8 & 0b1111111100000000,
    BlackRed = 4u << 8 & 0b1111111100000000,
};

void backspace(int);
std::array<int, 2> get_mid(int y_offset, int x_offset);
std::array<int, 2> get_pos();
int get_char();

// print somewhere in the screen and return the cursor to the original
// position print_no_move
void printnm(const int y, const int x, const std::string_view &str);

// print and go to position from which you started printing
void print_begin(const int y, const int x, const std::string_view &str);

void move(int y, int x);

void add_char(int ch);
void add_char(int ch, int attribute);

int lines();
int cols();

} // namespace curses

#endif /* ifndef CURSES_WRAP_HH */
