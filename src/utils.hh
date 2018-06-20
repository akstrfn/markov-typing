#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <curses.h>

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

// from: http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template<class T>
typename std::enable_if_t<!std::numeric_limits<T>::is_integer, bool>
almost_equal(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
    // unless the result is subnormal
           || std::abs(x-y) < std::numeric_limits<T>::min();
}

template <typename Seq>
auto choice(Seq& sequence){
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, std::size(sequence) - 1);

    auto it = std::begin(sequence);
    std::advance(it, dis(gen));
    return it;
}

template <typename Seq, typename W>
auto choice(Seq& sequence, W& weights){

    static_assert(std::is_floating_point_v<typename W::value_type>);
    assert(std::size(sequence) == std::size(weights));
    assert(std::size(sequence) != 0);

    // TODO implement inverting of weights
    std::mt19937 gen(std::random_device{}());
    auto idx = std::discrete_distribution<>{std::begin(weights), std::end(weights)}(gen);

    auto it = std::begin(sequence);
    std::advance(it, idx);
    return it;
}

template <typename F, typename S>
auto zip(const F& lhs, const S& rhs){
    // TODO This should be implemented without coping i.e. only over iterators...
    std::vector<std::pair<typename F::value_type, typename S::value_type>> zipped;
    zipped.reserve(std::size(lhs));

    auto lhs_iter = std::begin(lhs);
    auto rhs_iter = std::begin(rhs);
    while(lhs_iter != std::end(lhs) || rhs_iter != std::end(rhs))
        zipped.emplace_back(*lhs_iter++, *rhs_iter++);

    return zipped;
}

template <typename Seq, typename Number>
void invert_values(Seq& seq, Number max_value){
    static_assert(std::is_arithmetic_v<Number>);
    static_assert(std::is_arithmetic_v<typename Seq::value_type>);
    for (auto& el: seq)
        el = max_value - el;
}

template <typename T, typename E>
bool is_in(const T& container, const E& el){
    return std::end(container) != std::find(std::begin(container), std::end(container), el);
}

#endif /* UTILS_H */
