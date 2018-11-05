#include "probability_matrix.hh"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "utils.hh"

using namespace std;
using namespace impl;

using json = nlohmann::json;

namespace impl {

void to_json(json &j, const CharPair &p) {
    j = json{{"row_char", p.row_char.unicode()},
             {"col_char", p.col_char.unicode()},
             {"probability", p.probability},
             {"correct", p.correct},
             {"wrong", p.wrong},
             {"typing_time", p.typing_time},
             {"frequency", p.frequency}};
}

void from_json(const json &j, CharPair &p) {
    p.row_char = j.at("row_char").get<char>();
    p.col_char = j.at("col_char").get<char>();
    p.probability = j.at("probability").get<double>();
    p.correct = j.at("correct").get<size_t>();
    p.wrong = j.at("wrong").get<size_t>();
    p.typing_time = j.at("typing_time").get<long>();
    p.frequency = j.at("frequency").get<double>();
}

} // namespace impl

void to_json(json &js, const ProbabilityMatrix &pm) {
    js["Characters"] = pm.characters.toUtf8();
    js["average_typing_time"] = pm.average_typing_time;
    js["typing_time"] = pm.typing_time;
    js["data"] = pm.data;
}

void from_json(const json &js, ProbabilityMatrix &pm) {

    pm.characters = QString(js.at("Characters").get<string>().c_str());
    pm.average_typing_time = js.at("average_typing_time").get<long>();
    pm.typing_time = js.at("typing_time").get<vector<int>>();
    pm.data = js.at("data").get<vector<vector<CharPair>>>();

    for (int i = 0; i != pm.characters.size(); ++i)
        pm.char_map[pm.characters[static_cast<int>(i)].unicode()] = i;
}

// Matrix whose each entry is a probability that the next typed characted will
// be correct based on how frequent they were typed correctly
ProbabilityMatrix::ProbabilityMatrix() = default;

ProbabilityMatrix::ProbabilityMatrix(QString const _characters)
        : characters(_characters) {

    // std::set does this...
    sort_uniq(characters); // make sure characters are unique and sorted
    assert(characters.size() > 0);

    int const len = characters.length();
    data.reserve(len);
    for (int i = 0; i != len; ++i) {
        char_map[characters[i]] = i;
        vector<CharPair> row;
        row.reserve(len);
        for (int j = 0; j != len; ++j) {
            CharPair chp{characters[i], characters[j]};
            row.push_back(move(chp));
        }
        data.push_back(move(row));
    }
}

ProbabilityMatrix::ProbabilityMatrix(map<QChar, double> char_frequency_map) {

    characters = "";
    for (auto const &pair : char_frequency_map) {
        characters += pair.first;
    }
    assert(characters.size() > 0);

    int const len = characters.length();
    data.reserve(len);
    for (int i = 0; i != len; ++i) {
        QChar const first = characters[i];
        char_map[first] = i;
        vector<CharPair> row;
        row.reserve(len);
        for (int j = 0; j != len; ++j) {
            CharPair chp{first, characters[j]};
            chp.frequency = char_frequency_map[first];
            row.push_back(move(chp));
        }
        data.push_back(move(row));
    }
}

string ProbabilityMatrix::to_string() {
    stringstream ss;
    ss << fixed << setprecision(2) << "    ";

    for (auto el = characters.begin(); el != characters.end() - 1; ++el)
        ss << setw(3) << (*el).unicode() << ", ";
    ss << setw(3) << characters.back().unicode() << "\n";

    for (auto i = 0ul; i != std::size(data); ++i) {
        auto const &row = data[i];
        ss << characters[static_cast<int>(i)].unicode() << "| ";
        for (auto el = row.begin(); el != row.end() - 1; ++el) {
            ss << el->probability << ", ";
        }
        ss << row.back().probability << "\n";
    }
    return ss.str();
}

// TODO BUG updates when some wrong character instead of space is pressed
void ProbabilityMatrix::update_element(char const predecessor,
                                       char const current_char,
                                       long const typing_time,
                                       bool const correct) {
    // Doesn't count space.
    try {
        auto const from_idx = char_map.at(predecessor);
        auto const current_idx = char_map.at(current_char);
        CharPair &chp = data[from_idx][current_idx];

        // Using exponential moving average update average typing time.
        // Using different aplpha's for CharPair and global average can be
        // useful to adjust the level of dificulty i.e. one is updated more
        // agresivelly than the other.
        double alpha = 0.7;
        double diff = typing_time - average_typing_time;
        // truncation will happen long/double but it does not matter
        average_typing_time = average_typing_time + alpha * diff;

        // also update the CharPair
        diff = typing_time - chp.typing_time;
        chp.typing_time = chp.typing_time + alpha * diff;

        // if the CharPair average is below global average count as wrong
        // even if it was correct
        if (correct && (average_typing_time < chp.typing_time))
            chp.correct += 1;
        else
            chp.wrong += 1;

        double const total = chp.correct + chp.wrong;
        // frequency is used to account for how frequent is the pair
        chp.probability = (chp.correct / total) * chp.frequency;

    } catch (out_of_range &) { /*TODO log this */
    }
}

QString ProbabilityMatrix::generate_sentence(int word_size) {
    // get first character
    QChar ch = *choice(characters);
    int ch_idx = char_map.at(ch);

    // TODO simulate the state of matrix if the chosen charater would be
    // picked i.e. update probabilities as letters are chosen
    // so if accuracy is e.g 0.9 then chosen CharPair has chance of 0.9 to
    // increase variable correct count by 1 and 0.1 to increase variable
    // wrong. This should also update the accuracy value before simulating
    // next character. This allows simulation of a whole sentence the user
    // would type
    QString out = "";
    while (word_size--) {
        if (word_size % 5 == 0) { // size of one word is 4
            out.push_back(' ');
            continue;
        }

        out.push_back(ch);

        auto const row = data[ch_idx];
        vector<double> inverse_probs;
        inverse_probs.reserve(std::size(row));
        for (auto el : row)
            inverse_probs.push_back(2 - el.probability);

        ch = *choice(characters, inverse_probs);
        ch_idx = char_map.at(ch);
    }

    if (out.back() == ' ')
        out.chop(1); // remove possible last space
    return out;
}

// how close are we to 1 i.e. whole matrix is perfect? albeit this is
// impossible wih time pressure
double ProbabilityMatrix::proficiency() {
    double sum = 0;
    for (auto &&row : data)
        for (auto &&el : row)
            sum += el.probability;
    return sum / pow(data.size(), 2);
}

QString ProbabilityMatrix::get_characters() { return characters; }

void ProbabilityMatrix::update_time(int t) { typing_time.push_back(t); }

std::size_t ProbabilityMatrix::size() { return data.size(); }
