#ifndef IO_HH
#define IO_HH

#include "probability_matrix.hh"
#include <string_view>

void write_json(std::string_view, ProbabilityMatrix const &);
void read_json(std::string_view, ProbabilityMatrix &);

#endif /* ifndef IO_HH */
