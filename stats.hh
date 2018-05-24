#ifndef STATS_H
#define STATS_H

#include <string>
#include <numeric>

// TODO check for null characters
auto missed_characters(const std::string& buffer,
                       const std::string& full_sentence){
    return buffer.length() - std::inner_product(buffer.begin(), buffer.end(),
                              full_sentence.begin(), 0,
                              std::plus<>(), std::equal_to<>());
}

#endif /* ifndef STATS_H */
