#if __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>

#include <CLI/CLI.hpp>
#include <QApplication>
#include <QString>

#include "io.hh"
#include "probability_matrix.hh"
#include "sentence.hh"
#include "stats.hh"
#include "utils.hh"

#include "qt_gui/mainwindow.hh"

// using curses::Colors;

// QString lowercase = "qwertyuiopasdfghjklzxcvbnm";
// QString uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";

// TODO BUG some combinations of symbols form escape sequences so crazy stuff
// gets printed on the screen like numbers or memory address etc. At least I
// guess that's the reason.
// QString symbols = R"(`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?)";
// QString numbers = "0123456789";
// QString all_chars = lowercase + uppercase + symbols + numbers;

// TODO add timer to see how fast was typing for current sentence and save
// each sentence typing time for statistics (char/min or char/sec)
// TODO some weights for character appearances should exist. This would allow
// analysing some texts and then having relative frequencies which would be
// combined in the markov generator. It would also solve the problem that
// symbols appear far to often.
int main(int argc, char *argv[]) {

    // QString characters = "";
    // CLI::App app("Markov typing tutor.");

    // // clang-format off
    // app.add_flag("-l,--lowercase", [&characters](int) { characters +=
    // lowercase; },
    //              "Practice lowercase.");
    // app.add_flag("-U,--uppercase", [&characters](int) { characters +=
    // uppercase; },
    //              "Practice uppercase.");
    // app.add_flag("-s,--symbols", [&characters](int) { characters += symbols;
    // },
    //              "Practice symbols.");
    // app.add_flag("-n,--numbers", [&characters](int) { characters += numbers;
    // },
    //              "Practice numbers.");
    // // clang-format on

    // std::string CLIcustom;
    // app.add_option("--custom", CLIcustom,
    //                "Provide custom set of letters to practice.");

    // // TODO frequencies only take into account total occurrences of a
    // // character and don't encode the character pairs frequencies as it
    // should
    // // actually do.
    // // TODO currently character that is the most frequent actually does not
    // // appear often enough.
    // // TODO frequencies should be implemented with CLI11 subapp to get:
    // // markov-typing frequencies calculate - to calculate frequencies
    // // markov-typing frequencies list - to list frequencies
    // // markov-typing frequencies load - to load frequencies
    // std::vector<std::string> files;
    // CLI::Option *fopt = app.add_option("-f,--from-files", files,
    //                                    "Get and practice characters and their
    //                                    " "respective frequencies from
    //                                    file(s).");
    // fopt->check(CLI::ExistingFile);

    // std::string file_name;
    // CLI::Option *fname = app.add_option("--name", file_name,
    //                                     "Specify a file name for frequencies
    //                                     " "in order to reuse them later.");
    // fname->needs(fopt);
    // fopt->needs(fname);

    // std::string freq_name;
    // CLI::Option *lfreq = app.add_option("--load-frequencies", freq_name,
    //                                     "Specify the name for frequencies "
    //                                     "previously loaded and practiced.");

    // lfreq->excludes(fopt);
    // fopt->excludes(lfreq);

    // auto *opt_list_freq = app.add_flag("--list-frequencies",
    //                                    "List all available frequency files");
    // // TODO CLI is very fragile and needs to be solved better
    // CLI11_PARSE(app, argc, argv);

    // QString custom(CLIcustom.c_str());
    // if (!custom.isEmpty())
    //     characters = custom;

    // if (*opt_list_freq) {
    //     ls_frequencies();
    //     exit(1);
    // }

    // TODO maybe based on input one should be able to slice the matrix and
    // update only specific elements of it i.e. if only numbers are practiced
    // then only numbers are updated, so some sort of mutable view of full
    // matrix should be made...
    // ProbabilityMatrix matrix;
    // std::map<QChar, double> frequencies;
    // if (*lfreq) {
    //     auto opt_matrix = read_frequencies(freq_name);
    //     if (!opt_matrix) {
    //         std::cout << "There is no practice session with the name "
    //                   << freq_name << std::endl;
    //         exit(1);
    //     }
    //     matrix = std::move(opt_matrix.value());
    // } else if (*fopt) {
    //     for (auto const &f : files)
    //         for (auto const &[ch, num] : count_chars(f))
    //             frequencies[ch] += num;
    //     matrix = ProbabilityMatrix{frequencies};
    // } else if (characters.isEmpty()) {
    //     characters = all_chars;
    // }

    // // Matrix is still not constructed
    // if (!matrix.size()) {
    //     auto opt_matrix = read_json("data.json", characters.toStdString());
    //     if (!opt_matrix)
    //         matrix = ProbabilityMatrix{characters};
    //     else
    //         matrix = std::move(opt_matrix.value());
    // }

    QApplication a(argc, argv);
    a.setCursorFlashTime(0);

    auto typing = std::make_unique<MainWindow>();
    typing->show();

    // TODO this leaks like crazy disabled
    // if (*lfreq) {
    //     write_frequencies(freq_name, matrix);
    // } else if (*fopt) {
    //     write_frequencies(file_name, matrix);
    // } else {
    //     write_json("data.json", matrix);
    // }

    return a.exec();
}
