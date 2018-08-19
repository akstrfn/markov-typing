#include <filesystem>
#include <fstream>
#include <optional>

#include <nlohmann/json.hpp>

#include "io.hh"
#include "probability_matrix.hh"
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

void write_string(string_view file_name, ProbabilityMatrix &mat) {

    // Save progress
    // TODO check these paths before starting to avoid exercising and then not
    // being able to save
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

    fs::create_directories(fpath);
    fpath /= string(file_name); // boost cant handle string_view atm

    if (fs::exists(fpath)) {
        auto istr = ifstream(fpath.c_str());
        vector<ProbabilityMatrix> matrices = json::parse(istr);

        string chars = mat.get_characters();
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

    } else {
        // TODO for now just break everything if you cant save
        ofstream file{fpath};
        if (!file.is_open())
            throw ios_base::failure("Failed to open a file.");
        file << json{mat}; // write as array, strange constructor trick
    }
}

optional<ProbabilityMatrix> read_string(string_view file_name, string chars) {
    // TODO: if loading failed add fallback
    // TODO check on both spaces and prefer xdg?
    using namespace literals::string_literals;
    fs::path fpath;
    auto home = getenv("XDG_DATA_HOME");
    if (home) {
        fpath /= string(home) + "/MarkovTyping/" + file_name.data();
    } else {
        home = getenv("HOME");
        fpath /= home; // this will throw is home is not found
        fpath /= ".local/share/MarkovTyping/"s + file_name.data();
    }

    sort_uniq(chars);

    if (fs::exists(fpath)) {
        // TODO if json cant load file it will throw should this be handled?
        // c_str() because of boost
        // TODO copy constructor works but vector<matr>(json::parse) does not?
        vector<ProbabilityMatrix> mats = json::parse(ifstream{fpath.c_str()});
        auto res = find_if(mats.begin(), mats.end(), [&chars](auto &val) {
            string tmp = val.get_characters();

            // Failsafe if there was something wrong save in the json. This
            // should probably issue some warning or something like that.
            sort_uniq(tmp);

            return tmp == chars;
        });
        if (res != mats.end())
            return *res;
    }
    return nullopt;
}
