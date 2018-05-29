#ifndef SIMPLE_MATRIX_H
#define SIMPLE_MATRIX_H

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

// Matrix whose each entry is a probability that the next typed characted will
// be correct
class ProbabilityMatrix {
public:
    ProbabilityMatrix(const std::string& _characters) : characters(_characters)
    {
        auto len = characters.length();
        data.reserve(len);
        for(auto i=0ul; i != len; ++i){
            // ini matrix with all 0.0
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

    void write_to_file(const std::string& filename){
        std::ofstream fs;
        fs.open(filename);
        fs << to_string();
    }

    void update_element(const char& predecessor,
                        const char& current_char, const bool& correct){
        auto from_idx = char_map[predecessor];
        auto current_idx = char_map[current_char];
        data[from_idx][current_idx] = (data[from_idx][current_idx] + correct) / 2;
    }

    std::string generate_sentence(){
        //sum all rows and use them as weighted probabilites to chose a
        //starting character after that just chain 4 keys in that row that were
        //the worst i.e. have the smallest probabilities
        throw std::runtime_error("Not implemented");
        std::vector<double> weights(characters.length());
        for (auto i=0ul; i != characters.length(); ++i){
            weights[i] = std::accumulate(std::begin(data[i]), std::end(data[i]), 0.0);
        }
        char ch = weighted_choice(characters, weights);
    }

private:
    std::vector<std::vector<double>> data;
    std::string characters;
    std::map<char, int> char_map;
};

#endif /* SIMPLE_MATRIX_H */
