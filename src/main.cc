#if __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>

#include <CLI11.hpp>

#include "curses_wrap.hh"
#include "io.hh"
#include "probability_matrix.hh"
#include "sentence.hh"
#include "stats.hh"
#include "utils.hh"

using curses::Colors;

constexpr auto lowercase = "qwertyuiopasdfghjklzxcvbnm";
constexpr auto uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";

// TODO BUG some combinations of symbols form escape sequences so crazy stuff
// gets printed on the screen like numbers or memory address etc. At least I
// guess thats the reason.
constexpr auto symbols = R"(`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?)";
constexpr auto numbers = "0123456789";
const auto all_chars = std::string(lowercase) + uppercase + symbols + numbers;

// TODO add timer to see how fast was typing for current sentence and save
// each sentence typing time for statistics (char/min or char/sec)
// TODO some weights for character appearances should exist. This would allow
// analysing some texts and then having relative frequencies which would be
// combined in the markov generator. It would also solve the problem that
// symbols appear far to often.
int main(int argc, char *argv[]) {

    CLI::App app("Markov typing tutor");

    CLI::Option *lc = app.add_flag("--lowercase", "Use lowercase.");
    CLI::Option *uc = app.add_flag("--uppercase", "Use uppercase.");
    CLI::Option *sy = app.add_flag("--symbols", "Use symbols.");
    CLI::Option *num = app.add_flag("--num", "Use numbers.");

    std::string custom;
    app.add_option("--custom", custom, "Provide custom set of letters");

    CLI11_PARSE(app, argc, argv);

    std::string characters{};
    if (custom.empty()) {

        if (*lc)
            characters += lowercase;
        if (*uc)
            characters += uppercase;
        if (*sy)
            characters += symbols;
        if (*num)
            characters += numbers;

        if (characters.empty())
            characters = all_chars;
    } else {
        // TODO utf does not work
        characters = custom;
    }

    // TODO maybe based on input one should be able to slice the matrix and
    // update only specific elements of it i.e. if only numbers are practiced
    // then only numbers are updated, so some sort of mutable view of full
    // matrix should be made...
    ProbabilityMatrix matrix;
    auto opt_matrix = read_string("matrix.json", characters);
    if (!opt_matrix)
        matrix = ProbabilityMatrix{characters};
    else
        matrix = std::move(opt_matrix.value());

    PracticeSentence psec{matrix.generate_sentence(40)};

    curses::initialize();
    auto [mid_y, mid_x] = curses::get_mid(0, psec.get_sentence().size() / 2);

    curses::print_begin(mid_y, mid_x, psec.get_sentence());

    Timer char_timer;
    Timer sentence_timer;
    long sentence_duration{};
    curses::NChar ch;
    while (!ch.is_f4()) {
        if (psec.total_typed() == 1) // start when the first char is typed
            sentence_timer.start();

        auto [mid_y, mid_x] =
                curses::get_mid(0, psec.get_sentence().size() / 2);

        char_timer.start();
        ch = curses::get_char();
        auto const duration = char_timer.duration();

        // only predefined characters are allowed
        if (!is_in(all_chars, ch.data()) && !ch.is_enter() && !ch.is_backspace()
            && ch.data() != ' ')
            continue;

        bool at_the_end = psec.total_typed() == psec.get_sentence().size();

        // handle disable enter if we are not at the end
        if (ch.is_enter()) {
            if (at_the_end) {
                psec.refresh_sentence(matrix.generate_sentence(40));
                curses::print_begin(mid_y, mid_x, psec.get_sentence().c_str());
                sentence_duration = sentence_timer.duration();
                int d = sentence_duration / 1000.0;
                // clang-format off
                curses::printnm(1, 2, "Sentence typing time: "
                                      + std::to_string(d) + "s");
                // clang-format on
                matrix.update_time(d);
            }
            continue;
        }

        // handle backspace
        if (ch.is_backspace()) {
            // there are errors and typed is bigger than 0
            if (psec.get_error_exists() && psec.total_typed() > 0) {
                auto pos = psec.total_typed() - 1;
                auto tmp_ch = psec.get_sentence()[pos];
                curses::backspace(tmp_ch);
                psec.backspace();
            }
            continue;
        }

        // we are at the end but backspace and enter were not pressed
        if (at_the_end)
            continue;

        psec.update_typed(ch.data());

        // sentence paint procedure
        // save current cursor position
        auto [y, x] = curses::get_pos();
        // advance for one typing point...
        ++x;
        // go to mid_y and mid_x
        curses::move(mid_y, mid_x);
        // iterate over psec and paint characters
        for (auto &&[ch_sen, ch_typed] : psec) {
            if (ch_typed) { // not nullopt
                if (ch_typed == ch_sen)
                    curses::add_char(*ch_typed, Colors::GreenBlack);
                else
                    curses::add_char(ch_sen, Colors::BlackRed);
            } else { // typed is smaller than sentence
                curses::add_char(ch_sen);
            }
        }
        // restore position
        curses::move(y, x);

        // handle Probability matrix update
        // Prevent checking when typed strings is too small
        assert(!ch.is_backspace());
        assert(!ch.is_enter());

        auto len = psec.total_typed();
        // If there was error in the past don't count as correct
        bool correct = !psec.full_error_check();
        char current = psec.get_sentence()[len - 1];
        char last = psec.get_sentence()[len - 2];
        matrix.update_element(last, current, duration, correct);

        auto lines = curses::lines();
        auto cols = curses::cols();
        curses::printnm(lines - 2, 2,
                        "Proficiency: " + std::to_string(matrix.proficiency()));
        curses::printnm(lines - 1, 2,
                        "Speed (ms):  " + std::to_string(duration));

        constexpr char exit_msg[] = "Press F4 to exit.";
        curses::printnm(lines - 1, cols - std::size(exit_msg) - 2, exit_msg);
#ifdef DEBUG
        // clang-format off
        curses::printnm(1, cols - 12, "DEBUG MODE");

        std::stringstream ss;
        ss << "last: " << last
           << " current: " << current
           << " correct: " << correct;
        curses::printnm(lines - 8, 2, ss.str());

        ss.str("");
        for (auto el : psec.get_errors())
            ss << el;

        curses::printnm(lines - 4, 2, "Error: " + ss.str());
        curses::printnm(lines - 3, 2, "Typed: " + psec.get_typed());

        std::ofstream fs("matrix_console");
        fs << matrix.to_string();
        // clang-format on
#endif
    }

    write_string("matrix.json", matrix);

    curses::end_win();
    return 0;
}
