#ifndef STATS_H
#define STATS_H

#include <cassert>
#include <numeric>
#include <string>

bool all_correct(std::string_view buffer, std::string_view full_sentence) {
    assert(buffer.size() <= full_sentence.size());
    return buffer == full_sentence.substr(0, buffer.length());
}

bool last_char_correct(std::string_view buffer,
                       std::string_view full_sentence) {
    if (buffer.length() == 0)
        throw;

    int pos = buffer.length() - 1;
    if (buffer[pos] == full_sentence[pos])
        return true;
    return false;
}

#endif /* ifndef STATS_H */
