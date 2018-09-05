#ifndef SIMPLE_MATRIX_H
#define SIMPLE_MATRIX_H

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace impl {
struct CharPair {
    char row_char;
    char col_char;
    double probability{};
    size_t correct{};
    size_t wrong{};
    long typing_time{};
    double frequency = 1.0; // freq is from 0 - 1
};

} // namespace impl

class ProbabilityMatrix {
    // TODO use set for characters to get uniqueness
    std::string characters;
    std::vector<std::vector<impl::CharPair>> data;
    std::map<char, int> char_map;
    std::vector<int> typing_time; // history of full sentence typing times
    long average_typing_time{};

    friend void to_json(nlohmann::json &, ProbabilityMatrix &);
    friend void from_json(const nlohmann::json &, ProbabilityMatrix &);

public:
    ProbabilityMatrix();
    ProbabilityMatrix(std::string_view);
    ProbabilityMatrix(std::map<char, double>);

    void update_time(int);
    std::string get_characters();
    std::string to_string();
    void update_element(char const, char const, long const, bool const);
    std::string generate_sentence(int);
    double proficiency();
    std::size_t size();
};

#endif /* SIMPLE_MATRIX_H */
