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

struct CharPair {
    char row;
    char col;
    double probability;
    size_t correct; //start having one correct typing as an assumption
    size_t wrong;
};

// TODO Some sort of time pressure should be added which should also be
// accounted for in the function that updates the probabilies.

// Matrix whose each entry is a probability that the next typed characted will
// be correct based on how frequent they were typed correctly
class ProbabilityMatrix {
    std::vector<std::vector<CharPair>> data;
    std::string characters;
    std::map<char, int> char_map;
public:
    ProbabilityMatrix(const std::string& _characters) : characters(_characters)
    {
        auto len = characters.length();
        data.reserve(len);
        for(auto i=0ul; i != len; ++i){
            char_map[characters[i]] = i;
            std::vector<CharPair> row;
            row.reserve(len);
            for(auto j=0ul; j != len; ++j){
                CharPair chp;
                chp.row = characters[i];
                chp.col = characters[j];
                chp.probability = 1.0/std::size(characters);
                chp.correct = 1;
                chp.wrong = 0;
                row.push_back(chp);
            }
            data.push_back(row);
        }
    };

    auto to_string(){
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << "    ";
        for(auto el=characters.begin(); el != characters.end(); ++el){
            auto tmp = (el == characters.end() - 1) ? "\n"  : ", ";
            ss << std::setw(4) << *el << tmp;
        }
        for(auto i=0ul; i != std::size(data); ++i){
            auto row = data[i];
            ss << characters[i] << " | ";
            for(auto el=row.begin(); el != row.end(); ++el){
                auto tmp = (el == row.end() - 1) ? "\n"  : ", ";
                ss << el->probability << tmp;
            }
        }
        return ss.str();
    }

    auto to_json_string(){
        json js;
        for (auto& row : data){
            for(auto& el : row){
                js["ProbabilityMatrix"].push_back({{"row", el.row},
                                                   {"col", el.col},
                                                   {"probability", el.probability},
                                                   {"correct", el.correct},
                                                   {"wrong", el.wrong}});
            }
        }
        return js.dump(2);
    }

    // TODO remove this function
    void write_to_file(const std::string& filename){
        std::ofstream fs;
        fs.open(filename);
        fs << to_string();
    }

    // TODO use json to save/read to/from disk
    void read_from_json(const std::string& filename){
    }

    // TODO BUG updates when some wrong character instead of space is pressed
    void update_element(const char& predecessor,
                        const char& current_char,
                        const bool& correct){
        try {
            auto from_idx = char_map.at(predecessor);
            auto current_idx = char_map.at(current_char);
            CharPair* chp = &data[from_idx][current_idx];
            if (correct)
                chp->correct += 1;
            else
                chp->wrong += 1;

            // TODO total_typed should be separate vector variable for every
            // row?
            auto row = data[from_idx];
            double total_typed =
                std::accumulate(std::begin(row), std::end(row), 0.0,
                    [](auto& lhs, auto& rhs){
                        return lhs + rhs.correct + rhs.wrong;
                    });
            // update probabilities in a whole row
            for (auto& el : data[from_idx])
                el.probability = (el.correct + el.wrong) / total_typed;

        } catch (std::out_of_range&) {}
    }

    std::string generate_word(int word_size){
        // sum cols to determine weights, the highest sum denotes highest
        // chance to end up picking that letter in a chain
        std::vector<double> weights;
        weights.reserve(characters.length());
        for (auto& row : data)
            for (auto j=0ul; j != std::size(row); ++j)
                weights[j] += row[j].probability;

        // invert probabilities
        auto max_el = *std::max_element(std::begin(weights), std::end(weights));
        std::transform(std::begin(weights), std::end(weights), std::begin(weights),
                       [max_el](const auto& el){ return max_el - el; });

        // get first character
        char ch = '\000';
        while (ch == '\000')
            ch = *weighted_choice(characters, weights);

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

            auto row = data[ch_idx];

            // Trying out to randomize choices a bit
            std::vector<double> inverse_probs;
            inverse_probs.reserve(std::size(row));
            std::transform(std::begin(row), std::end(row), std::begin(inverse_probs),
                           [](auto& el){ return 1 - el.probability; });

            auto next = weighted_choice(characters, inverse_probs);
            ch_idx = std::distance(std::begin(characters), next);

            ch = characters[ch_idx];
        }
        return out;
    }

    std::string generate_sentence(const int num_words){
        std::string sentence = "";
        for (int i=0; i != num_words; ++i){
            sentence.append(generate_word(4));
            sentence.push_back(' ');
        }
        if (sentence.back() == ' ') sentence.pop_back();
        return sentence;
    }
};

#endif /* SIMPLE_MATRIX_H */
