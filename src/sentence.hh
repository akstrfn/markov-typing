#ifndef SENTENCE_HH
#define SENTENCE_HH

#include <string>
#include <type_traits>
#include <vector>

#include "stats.hh"

class PracticeSentence {
    std::string sentence;
    std::string typed;
    std::vector<short> errors; // to keep track of past errors
    bool error_exist{false};

public:
    PracticeSentence(std::string_view sentence_) : sentence{sentence_} {
        typed.reserve(sentence.size());
        errors.reserve(sentence.size());
    }

    auto update_typed(char ch) {
        // Caller must handle if characters are not suitable. Perhaps add all
        // allowed characters and then check when adding?
        typed.push_back(ch);
        bool const last_correct = sentence[typed.size() - 1] == ch;

        if (last_correct) {
            // update error vector only if there are no errors
            // TODO BUG: when backspace clears text it sets errors exist to 0
            // but in the past there was an error on that character so don't
            // change the size of the errors vector
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
        if (error_exist && !typed.empty())
            typed.pop_back();

        error_exist = !all_correct(typed, sentence);
    }

    auto refresh_sentence(std::string_view sentence) {
        sentence = sentence;
        typed.clear(); // most string implementations do not change capacity
        errors.clear();
    }

    auto total_typed() {
        return typed.size();
    }

    auto get_error_exists() { return error_exist; }
    auto& get_typed() { return typed; }
    auto& get_sentence() { return sentence; }
    auto& get_errors() { return errors; }
};

#endif /* ifndef SENTENCE_HH */
