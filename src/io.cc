#include <filesystem>
#include <fstream>
#include <optional>

#include <iostream>
#include <nlohmann/json.hpp>

#include "io.hh"
#include "utils.hh"

#if BOOST_FILESYSTEM

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#else

#include <filesystem>
namespace fs = std::filesystem;

#endif

using namespace std;
using json = nlohmann::json;

static fs::path prepare_data_dir() {
    fs::path fpath;
    auto home = getenv("XDG_DATA_HOME");
    if (home) {
        fpath /= home;
        fpath /= "MarkovTyping";
    } else {
        home = getenv("HOME");
        fpath /= home; // this will throw is home is not found
        fpath /= ".local/share/MarkovTyping";
    }
    if (!fs::exists(fpath))
        fs::create_directories(fpath);

    return fpath;
}

void write_json(string_view file_name, ProbabilityMatrix &mat) {
    // TODO check these paths before starting to avoid exercising and then not
    // being able to save
    fs::path fpath = prepare_data_dir();
    fpath /= file_name.data(); // boost cant handle string_view atm

    if (fs::exists(fpath)) {
        auto istr = ifstream(fpath.c_str());
        vector<ProbabilityMatrix> matrices = json::parse(istr);

        QString chars = mat.get_characters();
        auto comp = [&chars](auto &val) {
            return chars == val.get_characters();
        };

        auto res = find_if(matrices.begin(), matrices.end(), comp);
        if (res != matrices.end())
            *res = mat;
        else
            matrices.push_back(mat);

        ofstream file{fpath};
        if (!file.is_open())
            throw ios_base::failure("Failed to open a file.");

        json j;
        for (auto &el : matrices)
            j.push_back(el);

        file << j;

        // File does not exist so create one
    } else {
        // TODO for now just break everything if you cant save
        ofstream file{fpath};
        if (!file.is_open())
            throw ios_base::failure("Failed to open a file.");
        file << json{mat}; // write as an array, strange constructor trick
    }
}

optional<ProbabilityMatrix> read_json(string_view file_name, string chars) {
    fs::path fpath = prepare_data_dir();
    fpath /= file_name.data();

    sort_uniq(chars);

    if (fs::exists(fpath)) {
        // TODO if json cant load file it will throw should this be handled?
        // c_str() is used because of boost::fs
        vector<ProbabilityMatrix> mats = json::parse(ifstream{fpath.c_str()});
        auto res = find_if(mats.begin(), mats.end(), [&chars](auto &val) {
            QString tmp = val.get_characters();

            // Failsafe if there was something wrong save in the json. This
            // should probably issue some warning or something like that.
            sort_uniq(tmp);

            return tmp == QString(chars.c_str());
        });
        if (res != mats.end())
            return *res;
    }
    return nullopt;
}

void write_frequencies(string_view file_name, ProbabilityMatrix &mat) {
    // Save progress
    // TODO check these paths before starting to avoid exercising and then not
    // being able to save
    fs::path fpath = prepare_data_dir();
    fpath /= file_name.data(); // boost cant handle string_view atm

    // TODO for now just break everything if you cant save
    ofstream file{fpath};
    if (!file.is_open())
        throw ios_base::failure("Failed to open a file.");
    json j = mat;
    file << j; // write as array, strange constructor trick
}

optional<ProbabilityMatrix> read_frequencies(string_view file_name) {
    fs::path fpath = prepare_data_dir();
    fpath /= file_name.data();

    if (fs::exists(fpath)) {
        // TODO if json cant load file it will throw should this be handled?
        ProbabilityMatrix mat = json::parse(ifstream{fpath.c_str()});
        return std::move(mat);
    }
    return nullopt;
}

void ls_frequencies() {
    std::cout << "Available frequencies for practice:\n";
    fs::path fpath = prepare_data_dir();
    for (auto &&p : fs::directory_iterator(fpath)) {
        if (p.path().extension() == "")
            std::cout << p.path().filename() << "\n";
    }
}
