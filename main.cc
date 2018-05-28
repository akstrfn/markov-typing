#include <vector>
#include <iterator>
#include <iostream>
#include <ncurses.h>
#include "generate.hh"
#include "stats.hh"
#include "probability_matrix.hh"

std::string typed = "";
int row, col;
int y = -1, x = -1;
int mid_y, mid_x;
int ch;

//int main(int argc, char *argv[])
int main()
{
    std::string lowercase = "qwertyuiopasdfghjklzxcvbnm";
    std::string uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
    std::string symbols = "`~!@#$%^&*()-_=+{[]};:'\"\\|,<.>/?";
    std::string numbers = "0123456789";

    std::string short_chars = "asdfghjkl";
    std::string sentence = generate(short_chars, 40);
    ProbabilityMatrix m(short_chars);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_RED);

    printw(m.to_string().c_str());

    getmaxyx(stdscr, row, col);
    mid_x = (col - std::size(sentence))/2;
    mid_y = row/2;

    mvprintw(mid_y, mid_x, "%s", sentence.c_str());
    move(mid_y, mid_x);

    int score{};

    while(1){
        getyx(stdscr, y, x);
        ch = getch();

        if (typed.size() == std::size(sentence)){
            if (ch == KEY_ENTER || ch == '\n' || ch == 10){
                typed.clear();
                sentence = generate(short_chars, 40);
                move(mid_y, mid_x);
                clrtoeol();
                printw(sentence.c_str());
                move(mid_y, mid_x);
            } else if (score != 0
                       && (ch == KEY_BACKSPACE || ch == '\b' || ch == 127))
            {
                // allow backspace
            } else {
                // block any new entry of characters since we are at the end
                continue;
            }
        }

        if (ch == KEY_ENTER || ch == '\n' || ch == 10)
            continue;

        if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
            if (score == 0)
                continue;
            if (x != mid_x){
                move(y, --x);
                addch(sentence[x - mid_x]);
                move(y, x);
                typed.pop_back();
            }
        } else if (sentence[x - mid_x] == ch) {
            typed.push_back(ch);
            addch(ch | COLOR_PAIR(2));
        } else {
            typed.push_back(ch);
            if (sentence[typed.length() - 1] == ' ')
                addch(sentence[typed.length() - 1] | COLOR_PAIR(3));
            else
                addch(sentence[typed.length() - 1] | COLOR_PAIR(1));
        }

        // prob matrix update
        if (typed.length() > 1){ // update only if we have a predecessor
            bool correct = last_char_correct(typed, sentence);
            // ignore space for now
            int pos = typed.length() - 1;
            char current = typed[pos];
            char last = sentence[pos - 1];
            m.update_element(last, current, correct);
        }

        getyx(stdscr, y, x);
        score = missed_characters(typed, sentence);
        move(LINES - 4, 2);
        clrtoeol();
        printw("Errors: %i", score);
        move(LINES - 3, 2);
        clrtoeol();
        printw("Typed: %s", typed.c_str());
        mvprintw(0, 0, m.to_string().c_str());
        move(y, x);
    }
    //refresh();
    while(getch() != KEY_F(1)){ }
    endwin();
    return 0;
}
