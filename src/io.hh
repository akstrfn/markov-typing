#ifndef IO_HH
#define IO_HH

#include "probability_matrix.hh"
#include <string_view>

void write_string(std::string_view, ProbabilityMatrix &);
std::optional<ProbabilityMatrix> read_string(std::string_view);

#endif /* ifndef IO_HH */
