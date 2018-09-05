#if __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>

#include <CLI/CLI.hpp>

#include "curses_wrap.hh"
#include "io.hh"
#include "probability_matrix.hh"
#include "sentence.hh"
#include "stats.hh"
#include "utils.hh"

using curses::Colors;

constexpr auto lowercase = u8"qwertyuiopasdfghjklzxcvbnm";
constexpr auto uppercase = u8"QWERTYUIOPASDFGHJKLZXCVBNM";

// TODO BUG some combinations of symbols form escape sequences so crazy stuff
// gets printed on the screen like numbers or memory address etc. At least I
// guess that's the reason.
constexpr auto symbols = u8R"(`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?)";
constexpr auto numbers = u8"0123456789";
const auto all_chars = std::string(lowercase) + uppercase + symbols + numbers;

// TODO add timer to see how fast was typing for current sentence and save
// each sentence typing time for statistics (char/min or char/sec)
// TODO some weights for character appearances should exist. This would allow
// analysing some texts and then having relative frequencies which would be
// combined in the markov generator. It would also solve the problem that
// symbols appear far to often.
int main(int argc, char *argv[]) {

    std::string characters = "";
    CLI::App app("Markov typing tutor.");

    // clang-format off
    app.add_flag("-l,--lowercase", [&characters](int) { characters += lowercase; },
                 "Practice lowercase.");
    app.add_flag("-U,--uppercase", [&characters](int) { characters += uppercase; },
                 "Practice uppercase.");
    app.add_flag("-s,--symbols", [&characters](int) { characters += symbols; },
                 "Practice symbols.");
    app.add_flag("-n,--numbers", [&characters](int) { characters += numbers; },
                 "Practice numbers.");
    // clang-format on

    std::string custom;
    app.add_option("--custom", custom,
                   "Provide custom set of letters to practice.");

    // TODO frequencies only take into account total occurrences of a
    // character and don't encode the character pairs frequencies as it should
    // actually do.
    // TODO currently character that is the most frequent actually does not
    // appear often enough.
    // TODO frequencies should be implemented with CLI11 subapp to get:
    // markov-typing frequencies calculate - to calculate frequencies
    // markov-typing frequencies list - to list frequencies
    // markov-typing frequencies load - to load frequencies
    std::vector<std::string> files;
    CLI::Option *fopt = app.add_option("-f,--from-files", files,
                                       "Get and practice characters and their "
                                       "respective frequencies from file(s).");
    fopt->check(CLI::ExistingFile);

    std::string file_name;
    CLI::Option *fname = app.add_option("--name", file_name,
                                        "Specify a file name for frequencies "
                                        "in order to reuse them later.");
    fname->needs(fopt);
    fopt->needs(fname);

    std::string freq_name;
    CLI::Option *lfreq = app.add_option("--load-frequencies", freq_name,
                                        "Specify the name for frequencies "
                                        "previously loaded and practiced.");

    lfreq->excludes(fopt);
    fopt->excludes(lfreq);

    auto *opt_list_freq = app.add_flag("--list-frequencies",
                                       "List all available frequency files");
    // TODO CLI is very fragile and needs to be solved better
    CLI11_PARSE(app, argc, argv);

    if (!custom.empty())
        characters = custom;

    if (*opt_list_freq) {
        ls_frequencies();
        exit(1);
    }

    // TODO maybe based on input one should be able to slice the matrix and
    // update only specific elements of it i.e. if only numbers are practiced
    // then only numbers are updated, so some sort of mutable view of full
    // matrix should be made...
    ProbabilityMatrix matrix;
    std::map<char, double> frequencies;
    if (*lfreq) {
        auto opt_matrix = read_frequencies(freq_name);
        if (!opt_matrix) {
            std::cout << "There is no practice session with the name "
                      << freq_name << std::endl;
            exit(1);
        }
        matrix = std::move(opt_matrix.value());
    } else if (*fopt) {
        for (auto const &f : files)
            for (auto const &[ch, num] : count_chars(f))
                frequencies[ch] += num;
        matrix = ProbabilityMatrix{frequencies};
    } else if (characters.empty()) {
        characters = all_chars;
    }

    // Matrix is still not constructed
    if (!matrix.size()) {
        auto opt_matrix = read_string("data.json", characters);
        if (!opt_matrix)
            matrix = ProbabilityMatrix{characters};
        else
            matrix = std::move(opt_matrix.value());
    }

    PracticeSentence psec{matrix.generate_sentence(40)};

    auto runtime = curses::CursesRuntime();
    auto [mid_y, mid_x] = curses::get_mid(0, psec.get_sentence().size() / 2);

    curses::print_begin(mid_y, mid_x, psec.get_sentence());

    Timer char_timer;
    Timer sentence_timer;
    Timer total_time;
    total_time.start();
    long sentence_duration{};
    long char_duration{};
    curses::NChar ch;
    while (!ch.is_f4()) {

        // sentence paint procedure
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
        curses::move(mid_y, mid_x + psec.get_typed().size());

        // paints stats and info
        auto lines = curses::lines();
        auto cols = curses::cols();
        curses::printnm(lines - 2, 2,
                        "Proficiency: " + std::to_string(matrix.proficiency()));
        curses::printnm(lines - 1, 2,
                        "Speed (ms):  " + std::to_string(char_duration));
        // clang-format off
        curses::printnm(1, 2, "Sentence typing time: "
                              + std::to_string(sentence_duration / 1000) + "s");
        // clang-format on

        constexpr char exit_msg[] = "Press F4 to exit.";
        curses::printnm(lines - 1, cols - std::size(exit_msg) - 2, exit_msg);
        if (psec.total_typed() == 1) // start when the first char is typed
            sentence_timer.start();

        auto dur = total_time.duration() / 1000;
        curses::printnm(
                2, 2, "Practice time: " + std::to_string(dur / 60.0) + " min");

        char_timer.start();
        ch = curses::get_char();

        // handle resize
        if (ch.is_resize()) {
            curses::erase();
            // not a very good solution
            auto [mid_y_, mid_x_] =
                    curses::get_mid(0, psec.get_sentence().size() / 2);
            mid_y = mid_y_;
            mid_x = mid_x_;
            curses::move(mid_y, mid_x + psec.get_typed().size() - 1);
            continue;
        }

        char_duration = char_timer.duration();

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
                matrix.update_time(sentence_duration / 1000);
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

        // handle Probability matrix update
        assert(!ch.is_backspace());
        assert(!ch.is_enter());

        auto len = psec.total_typed();
        // If there was error in the past don't count as correct
        bool correct = !psec.full_error_check();
        char current = psec.get_sentence()[len - 1];
        char last = psec.get_sentence()[len - 2];
        // TODO this is updated even when there is an error in the sentence.
        // This might be ok but is probably unwanted behaviour.
        matrix.update_element(last, current, char_duration, correct);
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

    if (*lfreq) {
        write_frequencies(freq_name, matrix);
    } else if (*fopt) {
        write_frequencies(file_name, matrix);
    } else {
        write_string("data.json", matrix);
    }

    return 0;
}
