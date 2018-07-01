#ifndef SENTENCE_HH
#define SENTENCE_HH

#include <string>
#include <type_traits>
#include <vector>

#include "stats.hh"
#include "utils.hh"

class PracticeSentence {
    std::string sentence;
    std::string typed;
    std::vector<char *> errors; // to keep track of past errors
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
        auto *const s_char = &sentence[typed.size() - 1];
        bool const last_correct = *s_char == ch;

        // update error vector only if there are no previous errors
        if (!last_correct) {
            if (!error_exist && !is_in(errors, s_char))
                errors.push_back(s_char);
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

    // check if error was there in the past as well as now
    auto full_error_check() {
        return error_exist || is_in(errors, &sentence[typed.size() - 1]);
    }

    auto total_typed() { return typed.size(); }
    auto get_error_exists() { return error_exist; }
    auto &get_typed() { return typed; }
    auto &get_sentence() { return sentence; }

    // convert pointers to vector of 0 and 1 to have better overview of errors
    auto get_errors() {
        std::vector<short> v(typed.size(), 0);

        char *begin = &sentence[0];
        for (auto ptr : errors)
            v[ptr - begin] = 1;

        return v;
    }
};

#endif /* ifndef SENTENCE_HH */
