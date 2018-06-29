#if __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

#include <chrono>
#include <filesystem>
#include <iterator>
#include <sstream>
#include <vector>

#include "curses_wrap.hh"
#include "probability_matrix.hh"
#include "stats.hh"
#include "utils.hh"
#include "sentence.hh"

namespace fs = std::filesystem;
namespace cr = std::chrono;
using curses::Colors;

curses::NChar ch;

// int main(int argc, char *argv[])
int main() {
    constexpr auto lowercase = "qwertyuiopasdfghjklzxcvbnm";
    constexpr auto uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
    constexpr auto symbols = R"(`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?)";
    constexpr auto numbers = "0123456789";
    const auto all_chars =
            std::string(lowercase) + uppercase + symbols + numbers;

    std::string characters = all_chars;

#if DEBUG
    characters = "asdf";
#endif

    std::string tmppath;
    tmppath = std::string(std::getenv("HOME"))
              + "/.local/share/DeliberateTyping/matrix.json";

    ProbabilityMatrix p_matrix(characters);
    // TODO: if loading failed add fallback
    // TODO: BUG: when file is loaded then matrix is not updated correctly i.e.
    // it is not updated at all many times.
    fs::path tmpfile{tmppath};
    if (fs::exists(tmpfile))
        p_matrix.read_from_json(tmppath);

    PracticeSentence psec{p_matrix.generate_sentence(8)};

    curses::initialize();
    auto const [mid_y, mid_x] = curses::get_mid(0, std::size(psec.get_sentence()) / 2);

    curses::print_begin(mid_y, mid_x, psec.get_sentence());

    std::vector<short> errors; // really just 0 or 1 is necessary here
    errors.reserve(std::size(characters));
    bool error_exist{false};

    while (!ch.is_f1()) {
        error_exist = !all_correct(psec.get_typed(), psec.get_sentence());

        auto const start = cr::high_resolution_clock::now();
        ch = curses::get_char();
        auto const end = cr::high_resolution_clock::now();
        // are nanoseconds better choice here?
        auto const duration =
                cr::duration_cast<cr::milliseconds>(end - start).count();

        if (all_chars.find(ch.data()) == std::string::npos && !ch.is_enter()
            && !ch.is_backspace() && ch.data() != ' ')
            continue;

        if (psec.get_typed().size() == std::size(psec.get_sentence())) {
            if (ch.is_enter()) {
                psec.refresh_sentence(p_matrix.generate_sentence(8));
                curses::print_begin(mid_y, mid_x, psec.get_sentence().c_str());
                continue;
            } else if (ch.is_backspace()) {
                // allow backspace
            } else {
                // block any new entry of characters since we are at the end
                continue;
            }
        }

        if (ch.is_enter())
            continue;

        if (ch.is_backspace()) {
            // disable backspace if everything is correct
            if (!error_exist)
                continue;
            if (psec.total_typed()) {
                auto pos = psec.total_typed() - 1;
                auto tmp_ch = psec.get_sentence()[pos];
                curses::backspace(tmp_ch);
                psec.backspace();
                continue;
            }
        }

        auto ch_correct = psec.update_typed(ch.data());
        if (ch_correct) {
            curses::add_char(ch.data() | Colors::GreenBlack);
        } else {
            auto pos = psec.total_typed() - 1;
            auto tmp_ch = psec.get_sentence()[pos];
            curses::add_char(tmp_ch | Colors::BlackRed);
        }

#ifdef DEBUG
            auto lns = curses::get_lines();
            std::stringstream sdbg;
            sdbg << "first: " <<  std::size(psec.get_typed()) << " second: " << psec.get_errors().size();
            curses::printnm(lns - 10, 2, sdbg.str());
#endif

        // Probability matrix update
        if (auto len = std::size(psec.get_typed());
            len > 1 // Prevent checking when typed strings is too small
            && !ch.is_backspace() // When hitting backspace dont update
            // Don't update if there is an error in the text. This works
            // because errors dont update if more errors follow uncorected
            // error. This is too complicated...
            && (psec.get_errors().size() == len)) {
            // If there was error in the past don't count as correct
            bool correct = !psec.get_errors()[len - 1];
            char current = psec.get_sentence()[len - 1];
            char last = psec.get_sentence()[len - 2];
            // Don't count space.
            if (last != ' ')
                p_matrix.update_element(last, current, duration, correct);
#ifdef DEBUG
            auto lines = curses::get_lines();
            std::stringstream sdbg;
            sdbg << "last: " << last << " current: " << current
                 << " correct: " << correct;
            curses::printnm(lines - 8, 2, sdbg.str());
#endif
        }

#ifdef DEBUG
        auto lines = curses::get_lines();

        std::stringstream ss;
        for (auto &el : errors)
            ss << el;

        curses::printnm(lines - 6, 2,
                        "Proficiency: "
                                + std::to_string(p_matrix.proficiency()));
        curses::printnm(lines - 5, 2,
                        "Typing speed: " + std::to_string(duration));
        curses::printnm(lines - 4, 2, "Error: " + ss.str());
        curses::printnm(lines - 3, 2, "Typed: " + psec.get_typed());

        std::ofstream fs;
        fs.open("matrix_console");
        fs << p_matrix.to_string();
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
    file << p_matrix.to_json_string();

    curses::end_win();
    return 0;
}
