#pragma clang diagnostic ignored "-Wc++98-compat"

#include <vector>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <curses.h>

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
    std::string symbols = R"(`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?)";
    std::string numbers = "0123456789";

    std::string characters = lowercase + uppercase + symbols + numbers;
    std::string all_chars = characters;

#if DEBUG
    characters = "asdf";
#endif

    std::string tmppath;
    tmppath = std::string(std::getenv("HOME")) + "/.local/share/DeliberateTyping/matrix.csv";


    ProbabilityMatrix ProbMatrix(characters);
    // TODO: if loading failed add fallback
    std::filesystem::path tmpfile{tmppath};
    if (std::filesystem::exists(tmpfile))
        ProbMatrix = ProbabilityMatrix::read_from_json(tmppath);

    std::string sentence = ProbMatrix.generate_sentence(8);

    initscr();
    raw();
    // cbreak(); // use to enable ctrl-c
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

    std::vector<short> errors; // really just 0 or 1 is necessary here
    errors.reserve(50);
    int current_errors{};

    while(ch != KEY_F(1)){
        getyx(stdscr, y, x);
        ch = getch();
        // TODO BUG: this disallows characters that are not on the list
        // entirely which is not desired i.e. if characters = 'a' then only 'a'
        // can be typed!
        auto res = std::find(std::begin(all_chars), std::end(all_chars), ch);
        if (res == std::end(characters) && !is_enter(ch) && !is_backspace(ch) && ch != ' ')
            continue;

        if (typed.size() == std::size(sentence)) {
            if (is_enter(ch)) {
                typed.clear();
                errors.clear();
                // sentence = generate(characters, 40);
                sentence = ProbMatrix.generate_sentence(8);
                printnm(mid_y, mid_x, sentence.c_str());
                move(mid_y, mid_x);
            } else if (current_errors != 0 && is_backspace(ch)) { 
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
        } else if (sentence[x - mid_x] == ch) { // correct one
            typed.push_back(ch);
            if (std::size(typed) > std::size(errors))
                errors.push_back(0);
            addch(ch | COLOR_PAIR(2));
        } else { // wrong one
            typed.push_back(ch);
            if (std::size(typed) > std::size(errors))
                errors.push_back(1);
            if (sentence[typed.length() - 1] == ' ') //space
                addch(sentence[typed.length() - 1] | COLOR_PAIR(3));
            else //all others
                addch(sentence[typed.length() - 1] | COLOR_PAIR(1));
        }

        // Probability matrix update
        if (auto len=std::size(typed);
                len > 1  // prevent going past the begining
                && !is_backspace(ch) // when hitting backspace dont update
                && !errors[len] // when last character was error dont update it anymore
                && current_errors == 0){ // don't update if all errors are not cleared
            bool correct = last_char_correct(typed, sentence);
            int pos = typed.length() - 1;
            char current = typed[pos];
            char last = sentence[pos - 1];
            // Ignore space entirely for now
            // TODO (this means that even when mistakes with space are made it
            // is not counted)
            if (last != ' ')
                ProbMatrix.update_element(last, current, correct);
        }

        current_errors = missed_characters(typed, sentence);

#ifdef DEBUG
        printnm(LINES - 5, 2, "Errors: " + std::to_string(current_errors));

        std::stringstream ss;
        for (auto& el : errors)
            ss << el;

        printnm(LINES - 4, 2, "Error: " + ss.str());
        printnm(LINES - 3, 2, "Typed: " + typed);

        std::ofstream fs;
        fs.open("matrix_console");
        fs << ProbMatrix.to_string();
#endif
    }
    // Save progress
    // TODO check these paths before starting to avoid exercising and then not
    // being able to save
    std::string fpath;
    auto path = std::getenv("XDG_DATA_HOME");
    if (path)
        fpath = std::string(path) + "DeliberateTyping/matrix.csv";
    else
        fpath = std::string(std::getenv("HOME")) + "/.local/share/DeliberateTyping";
    std::filesystem::create_directories(fpath);
    fpath += "/matrix.csv";

    // TODO what if there is not even home defined?
    std::ofstream file{fpath};
    file << ProbMatrix.to_json_string();

    endwin();
    return 0;
}
