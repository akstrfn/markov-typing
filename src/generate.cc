#include "generate.hh"
#include <random>
#include <vector>
#include <string>

char choice(const std::string& sequence){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sequence.length() - 1);
    return sequence[dis(gen)];
}

std::string generate(const std::string& characters, const int max_size){
    std::string out = "";

    for (int i = 0; i != max_size; ++i){
        if ((i + 1) % 5 != 0)
            out.push_back(choice(characters));
        else
            out.push_back(' ');
    }
    // strip one possible whitespace at the end
    if (out.back() == ' ') out.pop_back();
    return out;
}
