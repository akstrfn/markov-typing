#include <filesystem>
#include <fstream>
#include <optional>

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

std::optional<ProbabilityMatrix> PracticeDataJson::get_matrix(QString chars) {
    sort_uniq(chars);
    auto res = std::find(characters.begin(), characters.end(), chars);
    if (res != characters.end())
        return matrices[std::distance(std::begin(characters), res)];
    return std::nullopt;
}

void PracticeDataJson::update(ProbabilityMatrix &mat) {
    auto chars = mat.get_characters();
    auto res = std::find(characters.begin(), characters.end(), chars);
    if (res != characters.end()) {
        matrices[std::distance(std::begin(characters), res)] = mat;
    } else {
        matrices.push_back(mat);
        characters.push_back(chars);
    }
}

static void to_json(json &j, const PracticeDataJson &pd) {
    j = json{{"version", pd.version}, {"matrices", pd.matrices}};
}

static void from_json(const json &j, PracticeDataJson &pd) {
    pd.version = j.at("version").get<int>();
    pd.matrices = j.at("matrices").get<vector<ProbabilityMatrix>>();
    for (auto &&m : pd.matrices) {
        pd.characters.push_back(m.get_characters());
    }
}

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

void save_to_json(string_view file_name, PracticeDataJson &pd) {
    fs::path fpath = prepare_data_dir();
    fpath /= file_name.data(); // boost cant handle string_view atm

    if (fs::exists(fpath)) {
        // TODO write backup
    }

    ofstream file{fpath};
    if (!file.is_open())
        throw ios_base::failure("Failed to open a file.");
    json j = pd;
    file << j;
}

optional<PracticeDataJson> load_from_json(string_view file_name) {
    fs::path fpath = prepare_data_dir();
    fpath /= file_name.data(); // boost cant handle string_view atm

    if (fs::exists(fpath)) {
        PracticeDataJson pd = json::parse(ifstream(fpath.c_str()));
        return std::move(pd);
    }
    return nullopt;
}
