#include "probability_matrix.hh"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

#include "utils.hh"

using json = nlohmann::json;

namespace impl {

void to_json(json &j, const CharPair &p) {
    j = json{
            {"row", p.row},
            {"col", p.col},
            {"row_char", p.row_char},
            {"col_char", p.col_char},
            {"probability", p.probability},
            {"correct", p.correct},
            {"wrong", p.wrong},
            {"typing_time", p.typing_time},
    };
}

void from_json(const json &j, CharPair &p) {
    p.row = j.at("row").get<int>();
    p.col = j.at("col").get<int>();
    p.row_char = j.at("row_char").get<std::string>();
    p.col_char = j.at("col_char").get<std::string>();
    p.probability = j.at("probability").get<double>();
    p.correct = j.at("correct").get<size_t>();
    p.wrong = j.at("wrong").get<size_t>();
    p.typing_time = j.at("typing_time").get<long>();
}

} // namespace impl

// Matrix whose each entry is a probability that the next typed characted will
// be correct based on how frequent they were typed correctly
ProbabilityMatrix::ProbabilityMatrix() = default;

ProbabilityMatrix::ProbabilityMatrix(std::string_view sen,
                                     const decltype(data) &data_,
                                     decltype(char_map) &char_map_, long avg)
        : characters(sen), data(data_), char_map(char_map_),
          average_typing_time(avg){};

ProbabilityMatrix::ProbabilityMatrix(std::string_view _characters)
        : characters(_characters) {
    int const len = characters.length();
    data.reserve(len);
    for (int i = 0; i != len; ++i) {
        char_map[characters[i]] = i;
        std::vector<impl::CharPair> row;
        row.reserve(len);
        for (int j = 0; j != len; ++j) {
            impl::CharPair chp{i, j, characters.substr(i, 1),
                               characters.substr(j, 1)};
            row.push_back(std::move(chp));
        }
        data.push_back(std::move(row));
    }
};

std::string ProbabilityMatrix::to_string() {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << "    ";

    for (auto el = characters.begin(); el != characters.end() - 1; ++el)
        ss << std::setw(3) << *el << ", ";
    ss << std::setw(3) << characters.back() << "\n";

    for (auto i = 0ul; i != std::size(data); ++i) {
        auto const &row = data[i];
        ss << characters[i] << "| ";
        for (auto el = row.begin(); el != row.end() - 1; ++el) {
            ss << el->probability << ", ";
        }
        ss << row.back().probability << "\n";
    }
    return ss.str();
}

std::string ProbabilityMatrix::to_json_string() const {
    json js;
    js["Characters"] = characters;
    js["typing_time"] = average_typing_time;
    for (auto &row : data)
        for (auto &el : row)
            js["Matrix"].push_back(el);
    return js.dump();
}

ProbabilityMatrix
ProbabilityMatrix::read_from_json(std::string_view filename) {
    json js = json::parse(std::ifstream{filename.data()});

    auto characters = js.at("Characters").get<std::string>();
    auto average_typing_time = js.at("typing_time").get<long>();

    std::map<char, int> char_map;
    for (auto i = 0ul; i != std::size(characters); ++i)
        char_map[characters[i]] = i;

    auto const sz = std::size(characters);
    decltype(data) data(sz, std::vector<impl::CharPair>(sz));
    for (auto &d : js.at("Matrix").get<std::vector<impl::CharPair>>())
        data[d.row][d.col] = d;

    return ProbabilityMatrix{characters, data, char_map, average_typing_time};
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
        impl::CharPair &chp = data[from_idx][current_idx];

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
        chp.probability = chp.correct / total;

    } catch (std::out_of_range &) { /*TODO log this */
    }
}

std::string ProbabilityMatrix::generate_word(int word_size) {
    // sum cols to determine weights, the highest sum denotes highest
    // chance to end up picking that letter in a chain
    std::vector<double> weights(characters.length());
    for (auto &row : data)
        for (auto j = 0ul; j != std::size(row); ++j)
            weights[j] += row[j].probability;

    // invert probabilities
    invert_values(weights,
                  *std::max_element(std::begin(weights), std::end(weights)));

    // TODO BUG after inversion max el becomes zero making it impossible to
    // be chosen.

    // get first character
    char ch = *choice(characters, weights);
    int ch_idx = char_map.at(ch);

    // TODO simulate the state of matrix if the chosen charater would be
    // picked i.e. update probabilities as letters are chosen
    // so if accuracy is e.g 0.9 then chosen CharPair has chance of 0.9 to
    // increase variable correct count by 1 and 0.1 to increase variable
    // wrong. This should also update the accuracy value before simulating
    // next character. This allows simulation of a whole sentence the user
    // would type
    std::string out = "";
    while (word_size--) {
        out.push_back(ch);

        auto const row = data[ch_idx];
        std::vector<double> inverse_probs;
        inverse_probs.reserve(std::size(row));
        for (auto el : row)
            inverse_probs.push_back(1 - el.probability);

        // TODO BUG: same as above bug for choice. Here we can have
        // elements being perfect i.e. probability is 1 hence they will
        // never be chosen.
        ch = *choice(characters, inverse_probs);
        ch_idx = char_map.at(ch);
    }
    return out;
}

std::string ProbabilityMatrix::generate_sentence(const int num_words) {
    std::string sentence = "";
    for (int i = 0; i != num_words; ++i)
        sentence.append(generate_word(4) + ' ');

    sentence.pop_back(); // remove last space
    return sentence;
}

// how close are we to 1 i.e. whole matrix is perfect? albeit this is
// impossible wih time pressure
double ProbabilityMatrix::proficiency() {
    double sum = 0;
    for (auto &&row : data)
        for (auto &&el : row)
            sum += el.probability;
    return sum / std::pow(data.size(), 2);
};
