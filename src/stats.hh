#ifndef STATS_H
#define STATS_H

#include <numeric>
#include <string>

// TODO check for null characters
int missed_characters(const std::string &buffer,
                      const std::string &full_sentence) {
    if (buffer.length() == 0)
        return 0;
    return buffer.length() - std::inner_product(buffer.begin(), buffer.end(),
                                                full_sentence.begin(), 0,
                                                std::plus<>(),
                                                std::equal_to<>());
}

bool last_char_correct(const std::string &buffer,
                       const std::string &full_sentence) {
    if (buffer.length() == 0)
        throw;

    int pos = buffer.length() - 1;
    if (buffer[pos] == full_sentence[pos])
        return true;
    return false;
}

#endif /* ifndef STATS_H */
