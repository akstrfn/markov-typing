#include "curses_wrap.hh"
#include <curses.h>

namespace curses {

NChar::NChar() = default;
NChar::NChar(int ch_) : ch{ch_} {};

bool NChar::is_backspace() {
    return ch == KEY_BACKSPACE || ch == '\b' || ch == 127;
}

bool NChar::is_enter() { return ch == KEY_ENTER || ch == '\n' || ch == 10; }

bool NChar::is_f4() { return ch == KEY_F(4); }

NChar &NChar::operator=(NChar other) {
    ch = other.ch;
    return *this;
}

NChar &NChar::operator=(int other) {
    ch = other;
    return *this;
}

int NChar::data() { return ch; }

void backspace(int replacement_char) {
    // TODO KEY_BACKSPACE is inconsistent so for now do it manually...  check
    // terminfo and terminal configuration since something is wrong there
    int y = getcury(stdscr);
    int x = getcurx(stdscr);
    mvaddch(y, --x, replacement_char);
    ::move(y, x);
}

void initialize() {
    ::setlocale(LC_ALL, "");
    ::initscr();
    ::raw();
    // cbreak(); // use to enable ctrl-c
    ::noecho();
    ::keypad(stdscr, 1);
    ::start_color();

    // separate into new function?
    ::init_pair(1, COLOR_RED, COLOR_BLACK);
    ::init_pair(2, COLOR_GREEN, COLOR_BLACK);
    ::init_pair(3, COLOR_RED, COLOR_RED);
    ::init_pair(4, COLOR_BLACK, COLOR_RED);

    ::init_pair(30, COLOR_WHITE, COLOR_BLACK);
    ::bkgd(COLOR_PAIR(30));
}

std::array<int, 2> get_mid(int y_offset, int x_offset) {
    return {(LINES / 2) - y_offset, (COLS / 2) - x_offset};
}

std::array<int, 2> get_pos() {
    int y = getcury(stdscr);
    int x = getcurx(stdscr);
    return {y, x};
}

void printnm(const int y, const int x, const std::string_view &str) {
    // sve old position
    int old_y = getcury(stdscr);
    int old_x = getcurx(stdscr);
    // clear the line and print on it
    ::move(y, x);
    ::clrtoeol();
    ::printw(str.data());
    // return to old position
    ::move(old_y, old_x);
}

void print_begin(const int y, const int x, const std::string_view &str) {
    ::move(y, x);
    ::clrtoeol();
    ::printw(str.data());
    ::move(y, x);
}

int get_char() { return ::getch(); }

void move(int y, int x) { ::move(y, x); }

void add_char(int ch) { ::addch(ch); }

void add_char(int ch, int attr) {
    ::attron(attr);
    ::addch(ch);
    ::attroff(attr);
}

int lines() { return LINES; }

int cols() { return COLS; }

void end_win() { endwin(); }

} // namespace curses
