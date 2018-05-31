#include <vector>
#include <iterator>
#include <iostream>
#include <ncurses.h>
#include "generate.hh"
#include "stats.hh"
#include "probability_matrix.hh"
#include "utils.hh"

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

    // for testing
    //lowercase = "asdfgj";
    std::string sentence = generate(lowercase, 40);
    ProbabilityMatrix m(lowercase);

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

    int current_errors{};

    while(1){
        getyx(stdscr, y, x);
        ch = getch();

        if (typed.size() == std::size(sentence)){
            if (is_enter(ch)){
                typed.clear();
                // sentence = generate(lowercase, 40);
                sentence = m.generate_sentence(8);
                printnm(mid_y, mid_x, sentence.c_str());
                move(mid_y, mid_x);
            } else if (current_errors != 0
                       && is_backspace(ch))
            {
                // allow backspace
            } else {
                // block any new entry of characters since we are at the end
                continue;
            }
        }

        if (is_enter(ch)) continue;

        if (is_backspace(ch)) {
            // disable backspace if everything is correct
            if (current_errors == 0) continue;
            if (!typed.empty()){
                move(y, --x);
                addch(sentence[typed.length() - 1]);
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
        // update only if we have a predecessor and if the movement was not
        // made with backspace
        if (typed.length() > 1 && !is_backspace(ch)){
            // TODO this still updates the probability if the character was
            // inaccurate but erased and accurate one gets typed in. This means
            // that inaccurate typings have less influence than correct ones.
            // Fix this.
            bool correct = last_char_correct(typed, sentence);
            int pos = typed.length() - 1;
            char current = typed[pos];
            char last = sentence[pos - 1];
            // ignore space for now
            if (last != ' ')
                m.update_element(last, current, correct);
        }

        current_errors = missed_characters(typed, sentence);

        printnm(LINES - 4, 2, "Errors: " + std::to_string(current_errors));
        printnm(LINES - 3, 2, "Typed " + typed);
        //printnm(0, 0, m.to_string());
        m.write_to_file("test");
    }
    //refresh();
    while(getch() != KEY_F(1)){ }
    endwin();
    return 0;
}
