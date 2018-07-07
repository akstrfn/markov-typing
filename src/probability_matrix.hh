#ifndef SIMPLE_MATRIX_H
#define SIMPLE_MATRIX_H

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace impl {
struct CharPair {
    int row;
    int col;
    std::string row_char;
    std::string col_char;
    double probability{};
    size_t correct{};
    size_t wrong{};
    long typing_time{}; // TODO should I make all time units unsigned long?
};

} // namespace impl

class ProbabilityMatrix {
    std::string characters;
    std::vector<std::vector<impl::CharPair>> data;
    std::map<char, int> char_map;
    long average_typing_time{};

    friend void to_json(nlohmann::json &, ProbabilityMatrix &);
    friend void from_json(const nlohmann::json &, ProbabilityMatrix &);

    std::string generate_word(int);

public:
    ProbabilityMatrix();
    ProbabilityMatrix(std::string_view);
    ProbabilityMatrix(std::string_view, const decltype(data) &,
                      decltype(char_map) &, long);

    std::string to_string();
    void update_element(char const, char const, long const, bool const);
    std::string generate_sentence(const int);
    double proficiency();
};

#endif /* SIMPLE_MATRIX_H */
