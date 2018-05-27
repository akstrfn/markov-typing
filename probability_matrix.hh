#ifndef SIMPLE_MATRIX_H
#define SIMPLE_MATRIX_H

#include <vector>
#include <string>
#include <map>
#include <sstream>

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
        for(auto& row : data){
            for(auto el=row.begin(); el != row.end(); ++el){
                char tmp = (el == row.end() - 1) ? '\n'  : ',';
                ss << *el << tmp;
            }
        }
        return ss.str();
    }

    void update_element(const char& predecessor,
                        const char& current_char, const bool& correct){
        auto from_idx = char_map[predecessor];
        auto current_idx = char_map[current_char];
        data[current_idx][from_idx] = (data[current_idx][from_idx] + correct) / 2;
    }

private:
    std::vector<std::vector<double>> data;
    std::string characters;
    std::map<char, int> char_map;
};

#endif /* SIMPLE_MATRIX_H */
