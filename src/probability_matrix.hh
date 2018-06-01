#ifndef SIMPLE_MATRIX_H
#define SIMPLE_MATRIX_H

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "utils.hh"

// TODO in this matrix history should be accounted for to lower the oscillation
// of key probabilities e.g. if guess = 1, miss = 0 then [0, 1, 0, 1, 1] is an
// array of past performance for the key then some function should be used to
// map this into 0-1 range. log maybe? linear? weighted average?

// TODO when probabilites are rouded at two decimals if characters are typed
// correctly the probabiblities after a while become all 1 and then it is not
// useful to generate anything from the matrix anymore. Some sort of time
// pressure should be added which should also be accounted for in the function
// that updates the probabilies.

// Matrix whose each entry is a probability that the next typed characted will
// be correct based on how frequent they were typed correctly
class ProbabilityMatrix {
    std::vector<std::vector<double>> data;
    std::string characters;
    std::map<char, int> char_map;
public:
    ProbabilityMatrix(const std::string& _characters) : characters(_characters)
    {
        auto len = characters.length();
        data.reserve(len);
        for(auto i=0ul; i != len; ++i){
            data.push_back(std::vector<double>(len));
            char_map[characters[i]] = i;
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
                ss << *el << tmp;
            }
        }
        return ss.str();
    }

    auto to_csv_string(){
        std::stringstream ss;
        ss << std::setprecision(4);
        for(auto i=0ul; i != std::size(data); ++i){
            auto row = data[i];
            for(auto el=row.begin(); el != row.end(); ++el){
                auto tmp = (el == row.end() - 1) ? "\n"  : ",";
                ss << *el << tmp;
            }
        }
        return ss.str();
    }

    void write_to_file(const std::string& filename){
        std::ofstream fs;
        fs.open(filename);
        fs << to_string();
    }

    // TODO finish me
    void read_from_csv(const std::string& filename){
        std::ifstream f;
        f.open(filename);
        std::string s;
        std::getline(f, s, ',');
    }

    void update_element(const char& predecessor,
                        const char& current_char, const bool& correct){
        try {
            auto from_idx = char_map.at(predecessor);
            auto current_idx = char_map.at(current_char);
            data[from_idx][current_idx] = (data[from_idx][current_idx] + correct) / 2;
        } catch (std::out_of_range) {}
    }

    // TODO since always the same element is picked deterministically then
    // sometimes weird combinations come up like buch of same characters one
    // after another like ffff or gfff etc. This means that it should be
    // randomized a bit. Probably weighted choice would do as well.
    std::string generate_word(const int word_size){
        //sum all rows and use them as weighted probabilites to chose a
        //starting character after that just chain 4 keys in that row that were
        //the worst i.e. have the smallest probabilities
        std::vector<double> weights(characters.length());
        for (auto i=0ul; i != characters.length(); ++i){
            weights[i] = std::accumulate(std::begin(data[i]), std::end(data[i]), 0.0);
        }
        char ch = weighted_choice(characters, weights);
        // TODO can avoiding the null character be solved better?
        while (1){
            if (ch == '\000')
                ch = weighted_choice(characters, weights);
            else
                break;
        }
        int ch_idx = char_map.at(ch);

        std::string out = "";
        for (int i=0; i != word_size; ++i){
            out.push_back(ch);

            auto row = data[ch_idx];
            auto next = std::min_element(std::begin(row), std::end(row));

            ch_idx = std::distance(std::begin(row), next);
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
