#if __clang__
    #pragma clang diagnostic ignored "-Wc++98-compat"
#endif

#include <vector>
#include <iterator>
#include <filesystem>
#include <sstream>
#include <chrono>

#include <curses.h>

#include "stats.hh"
#include "probability_matrix.hh"
#include "utils.hh"

namespace fs = std::filesystem;
namespace cr = std::chrono;

std::string typed = "";
int row, col;
int y = -1, x = -1;
int mid_y, mid_x;
int ch;

//int main(int argc, char *argv[])
int main()
{
    const std::string lowercase = "qwertyuiopasdfghjklzxcvbnm";
    const std::string uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
    const std::string symbols = R"(`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?)";
    const std::string numbers = "0123456789";
    const std::string all_chars = lowercase + uppercase + symbols + numbers;

    std::string characters = all_chars;

#if DEBUG
    characters = "asdf";
#endif

    std::string tmppath;
    tmppath = std::string(std::getenv("HOME")) + "/.local/share/DeliberateTyping/matrix.json";


    ProbabilityMatrix ProbMatrix(characters);
    // TODO: if loading failed add fallback
    // TODO: BUG: when file is loaded then matrix is not updated correctly i.e.
    // it is not updated at all many times.
    fs::path tmpfile{tmppath};
    if (fs::exists(tmpfile))
        ProbMatrix.read_from_json(tmppath);

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
    errors.reserve(std::size(characters));
    int current_errors{};

    while(ch != KEY_F(1)){
        getyx(stdscr, y, x);

        auto const start = cr::high_resolution_clock::now();
        ch = getch();
        auto const end = cr::high_resolution_clock::now();
        // are nanoseconds better choice here?
        auto const duration = cr::duration_cast<cr::milliseconds>(end - start).count();

        auto res = std::find(std::begin(all_chars), std::end(all_chars), ch);
        if (res == std::end(all_chars) && !is_enter(ch) && !is_backspace(ch)
                && ch != ' ')
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
                // prevent checking when typed strings is too small
                len > 1
                // when hitting backspace dont update
                && !is_backspace(ch)
                // don't update if all errors are not cleared
                && current_errors == 0){
            bool correct = last_char_correct(typed, sentence);

            char current = sentence[len - 1];
            char last = sentence[len - 2];
            // this convoluted logic is used to catch when we made a mistake in
            // the past and removed it with backspace
            if (!correct && last != ' '){
                // Ignore space entirely? Even don't count mistakes when space
                // is used?
                ProbMatrix.update_element(last, current, duration, correct);
            } else if (!errors[len - 1]){
                ProbMatrix.update_element(last, current, duration, correct);
            }
        }

        current_errors = missed_characters(typed, sentence);

#ifdef DEBUG
        printnm(LINES - 5, 2, "Errors: " + std::to_string(current_errors));

        std::stringstream ss;
        for (auto& el : errors)
            ss << el;

        printnm(LINES - 7, 2, "Proficiency: " + std::to_string(ProbMatrix.proficiency()));
        printnm(LINES - 6, 2, "Typing speed: " + std::to_string(duration));
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
        fpath = std::string(path) + "DeliberateTyping/matrix.json";
    else
        fpath = std::string(std::getenv("HOME"))
                + "/.local/share/DeliberateTyping";
    fs::create_directories(fpath);
    fpath += "/matrix.json";

    // TODO what if there is not even home defined?
    std::ofstream file{fpath};
    file << ProbMatrix.to_json_string();

    endwin();
    return 0;
}
