#include <filesystem>
#include <fstream>

#include "io.hh"

#if BOOST_FILESYSTEM

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#else

#include <filesystem>
namespace fs = std::filesystem;

#endif

void write_string(std::string_view file_name, ProbabilityMatrix const &mat) {

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
    fpath /= std::string(file_name); // boost cant handle string_view atm

    // TODO for now just break everything if you cant save
    std::ofstream file{fpath};
    if (!file.is_open())
        throw;
    file << mat.to_json_string();
}

bool read_string(std::string_view file_name, ProbabilityMatrix &mat) {
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

    if (fs::exists(fpath)) {
        // TODO if json cant load file it will throw should this be handled?
    return false;
        return ProbabilityMatrix::read_from_json(
                fpath.c_str()); // c_str because of boost
}
