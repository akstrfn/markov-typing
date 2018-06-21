#ifndef SIMPLE_MATRIX_H
#define SIMPLE_MATRIX_H

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "utils.hh"

using json = nlohmann::json;

namespace impl
{
    struct CharPair {
        int row;
        int col;
        std::string row_char;
        std::string col_char;
        double probability{};
        long long correct{};
        unsigned long long wrong{};
    };

    void to_json(json& j, const impl::CharPair& p) {
        j = json{{"row", p.row},
                 {"col", p.col},
                 {"probability", p.probability},
                 {"correct", p.correct},
                 {"wrong", p.wrong},
                 {"row_char", p.row_char},
                 {"col_char", p.col_char}};
    }

    void from_json(const json& j, impl::CharPair& p) {
        p.row = j.at("row").get<int>();
        p.col = j.at("col").get<int>();
        p.probability = j.at("probability").get<double>();
        p.correct = j.at("correct").get<size_t>();
        p.wrong = j.at("wrong").get<size_t>();
        p.row_char = j.at("row_char").get<std::string>();
        p.col_char = j.at("col_char").get<std::string>();
    }

} /* impl */


// TODO Some sort of time pressure should be added which should also be
// accounted for in the function that updates the probabilies.

// Matrix whose each entry is a probability that the next typed characted will
// be correct based on how frequent they were typed correctly
class ProbabilityMatrix {
    std::vector<std::vector<impl::CharPair>> data;
    std::string characters;
    std::map<char, int> char_map;
public:
    ProbabilityMatrix(const std::string& _characters) : characters(_characters)
    {
        int const len = characters.length();
        data.reserve(len);
        for(int i=0; i != len; ++i){
            char_map[characters[i]] = i;
            std::vector<impl::CharPair> row;
            row.reserve(len);
            for(int j=0; j != len; ++j){
                impl::CharPair chp{i, j, characters.substr(i, 1), characters.substr(j, 1)};
                row.push_back(std::move(chp));
            }
            data.push_back(std::move(row));
        }
    };

    auto to_string(){
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << "    ";

        for (auto el=characters.begin(); el != characters.end() - 1; ++el)
            ss << std::setw(4) << *el << ", ";
        ss << std::setw(4) << characters.back() << "\n";

        for (auto i=0ul; i != std::size(data); ++i){
            auto const &row = data[i];
            ss << characters[i] << "| ";
            for (auto el=row.begin(); el != row.end() - 1; ++el){
                ss << el->probability << ", ";
            }
            ss << row.back().probability << "\n";
        }
        return ss.str();
    }

    auto to_json_string(){
        // lot of info is redundant but its not important for now
        json js;
        js["Characters"] = characters;
        for (auto& row : data)
            for(auto& el : row)
                js["Matrix"].push_back(el);
        return js.dump();
    }

    // TODO test this
    auto read_from_json(const std::string& filename){
        json js = json::parse(std::ifstream{filename});
        std::string characters = js.at("Characters").get<std::string>();

        for(auto i=0ul; i != std::size(characters); ++i)
            char_map[characters[i]] = i;

        auto sz = std::size(characters);
        std::vector<std::vector<impl::CharPair>> tmpdata(sz, std::vector<impl::CharPair>(sz));
        for (auto& d: js.at("Matrix").get<std::vector<impl::CharPair>>())
            tmpdata[d.row][d.col] = d;
        data = tmpdata;
    }

    // TODO BUG updates when some wrong character instead of space is pressed
    void update_element(const char& predecessor,
                        const char& current_char,
                        const bool& correct){
        try {
            auto const from_idx = char_map.at(predecessor);
            auto const current_idx = char_map.at(current_char);
            impl::CharPair& chp = data[from_idx][current_idx];
            if (correct)
                chp.correct += 1;
            else
                chp.wrong += 1;

            double const total = std::abs(chp.correct) + chp.wrong;
            chp.probability = std::abs(chp.correct) / total;

        } catch (std::out_of_range&) {}
    }

    std::string generate_word(int word_size){
        // sum cols to determine weights, the highest sum denotes highest
        // chance to end up picking that letter in a chain
        std::vector<double> weights(characters.length());
        for (auto& row : data)
            for (auto j=0ul; j != std::size(row); ++j)
                weights[j] += row[j].probability;

        // invert probabilities
        invert_values(weights, *std::max_element(std::begin(weights), std::end(weights)));

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
        while (word_size--){
            out.push_back(ch);

            auto const row = data[ch_idx];
            std::vector<double> inverse_probs;
            inverse_probs.reserve(std::size(row));
            for (auto el: row)
                inverse_probs.push_back(1 - el.probability);

            ch = *choice(characters, inverse_probs);
            ch_idx = char_map.at(ch);
        }
        return out;
    }

    std::string generate_sentence(const int num_words){
        std::string sentence = "";
        for (int i=0; i != num_words; ++i){
            sentence.append(generate_word(4) + ' ');
        }
        if (sentence.back() == ' ') sentence.pop_back();
        return sentence;
    }
};

#endif /* SIMPLE_MATRIX_H */
