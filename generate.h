#ifndef GENERATE_H
#define GENERATE_H

#include <random>
#include <vector>
#include <string>

char choice(const std::string& sequence){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sequence.length());
    return sequence[dis(gen)];
}

std::string generate(int max_size){
    std::string out = "";
    std::string lowercase = "qwertyuiopasdfghjklzxcvbnm";

    for (int size = 0; size != max_size; ++size){
        if (size == 0 || size % 5 != 0)
            out.push_back(choice(lowercase));
        else
            out.push_back(' ');
    }
    return out;
}
#endif /* ifndef GENERATE_H */
