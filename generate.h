#ifndef GENERATE_H
#define GENERATE_H

#include <random>
#include <vector>
#include <string>

char choice(const std::string& sequence){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sequence.length() - 1);
    return sequence[dis(gen)];
}

std::string generate(int max_size){
    std::string out = "";
    std::string lowercase = "qwertyuiopasdfghjklzxcvbnm";
    std::string uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
    std::string symbols = "`~!@#$%^&*()-_=+{[]};:'\"\\|,<.>/?";
    std::string numbers = "0123456789";

    for (int i = 0; i != max_size; ++i){
        if ((i + 1) % 5 != 0)
            out.push_back(choice(lowercase + symbols + uppercase + numbers));
        else
            out.push_back(' ');
    }
    return out;
}
#endif /* ifndef GENERATE_H */
