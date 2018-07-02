#include <filesystem>

#include "io.hh"
#include "probability_matrix.hh"

namespace fs = std::filesystem;

// TODO add tests
void write_json(std::string_view file_name, ProbabilityMatrix const &mat){

    // Save progress
    // TODO check these paths before starting to avoid exercising and then not
    // being able to save
    fs::path fpath;
    auto home = std::getenv("XDG_DATA_HOME");
    if (home) {
        fpath /= home;
        fpath /= "DeliberateTyping";
    } else {
        home = std::getenv("HOME");
        fpath /= home; // this will throw is home is not found
        fpath /= ".local/share/DeliberateTyping";
    }

    fs::create_directories(fpath);
    fpath /= file_name;

    // TODO for now just break everything if you cant save
    std::ofstream file{fpath};
    if (!file.is_open())
        throw;
    file << mat.to_json_string();
}

// TODO add tests
void read_json(std::string_view file_name, ProbabilityMatrix &mat){
    // TODO: if loading failed add fallback
    // TODO check on both spaces and prefer xdg?
    using namespace std::literals::string_literals;
    fs::path fpath;
    auto home = std::getenv("XDG_DATA_HOME");
    if (home) {
        fpath /= std::string(home) + "/DeliberateTyping/" + file_name.data();
    } else {
        home = std::getenv("HOME");
        fpath /= home; // this will throw is home is not found
        fpath /= ".local/share/DeliberateTyping/"s + file_name.data();
    }

    if (fs::exists(fpath))
        mat.read_from_json(fpath);
}
