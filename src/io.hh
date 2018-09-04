#ifndef IO_HH
#define IO_HH

#include "probability_matrix.hh"
#include <iostream>
#include <string_view>

void write_string(std::string_view, ProbabilityMatrix &);
std::optional<ProbabilityMatrix> read_string(std::string_view, std::string);

void write_frequencies(std::string_view, ProbabilityMatrix &);
std::optional<ProbabilityMatrix> read_frequencies(std::string_view);

#endif /* ifndef IO_HH */
