#ifndef IO_HH
#define IO_HH

#include "probability_matrix.hh"
#include <optional>
#include <string_view>

// TODO practice statistics should also go into this object
struct PracticeDataJson {
    int version; // keep track of json format changes
    std::vector<ProbabilityMatrix> matrices; // all matrices
    std::vector<QString> characters; // characters from corresponding matrices

    // I could return a ref and directly expose matrix for modifications?
    std::optional<ProbabilityMatrix> get_matrix(QString chars);
    void update(ProbabilityMatrix &mat);
};

void save_to_json(std::string_view, PracticeDataJson &);
std::optional<PracticeDataJson> load_from_json(std::string_view);

#endif /* ifndef IO_HH */
