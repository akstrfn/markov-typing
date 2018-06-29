#ifndef SENTENCE_HH
#define SENTENCE_HH

#include <string>
#include <type_traits>
#include <vector>

#include "stats.hh"

class PracticeSentence {
    std::string full_sentence;
    std::string typed;
    std::vector<short> errors; // to keep track of past errors
    bool error_exist{false};

public:
    PracticeSentence(std::string_view sentence_) : full_sentence{sentence_} {
        typed.reserve(full_sentence.size());
        errors.reserve(full_sentence.size());
    }

    auto update_typed(char ch) {
        // Caller must handle if characters are not suitable. Perhaps add all
        // allowed characters and then check when adding?
        typed.push_back(ch);
        bool const last_correct = full_sentence[typed.size() - 1] == ch;

        if (last_correct) {
            // update error vector only if there are no errors
            if (!error_exist)
                errors.push_back(0);
        } else { // last was wrong
            // update error vector only if there are no errors
            if (!error_exist)
                errors.push_back(1);
            error_exist = true;
        }
        return last_correct;
    }

    auto backspace() {
        if (error_exist)
            typed.pop_back();

        error_exist = !all_correct(typed, full_sentence);
    }

    auto refresh_sentence(std::string_view sentence) {
        full_sentence = sentence;
        typed.clear(); // most string implementations do not change capacity
    }

    auto get_error_exists() { return error_exist; }
};

#endif /* ifndef SENTENCE_HH */
