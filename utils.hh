#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <random>
#include <ncurses.h>

bool is_backspace(const int ch){
    return  ch == KEY_BACKSPACE || ch == '\b' || ch == 127;
}

bool is_enter(const int ch){
    return ch == KEY_ENTER || ch == '\n' || ch == 10;
}

// print somewhere in the screen and return the cursor to original position
// print_no_move
void printnm(const int y, const int x, const std::string& str){
    int old_y, old_x;
    getyx(stdscr, old_y, old_x);
    move(y, x);
    clrtoeol();
    printw(str.c_str());
    move(old_y, old_x);
}

template <typename T, typename W>
auto weighted_choice(const T& container, W weights){
    auto sum = std::accumulate(std::begin(weights), std::end(weights), 0.0);
    std::mt19937 gen(std::random_device{}());
    auto rnd = std::uniform_real_distribution<>{0, sum}(gen);

    auto rolling_sum = 0;
    for (int i=0; i != std::size(weights); ++i){
        rolling_sum += weights[i];
        if (rolling_sum < rnd)
            return container[i];
    }
}

#endif /* UTILS_H */
