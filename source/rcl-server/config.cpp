#include "config.h"

#include "include/json.hpp"

#include <ShlObj.h>
#include <filesystem>

#include <fstream>

//----------

using nlohmann::json;

using std::ifstream;
using std::ofstream;

using std::wstring;

json config_json;

//----------

//----------

static wstring get_path_AppData() {
    PWSTR temp = NULL;

    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &temp);

    return wstring(temp);
}

static wstring mkdir_in_AppData(const wstring& path) {
    wstring folder = get_path_AppData() + L"\\" + path;

    if (!std::filesystem::exists(folder)) std::filesystem::create_directories(folder);

    return folder;
}


static wstring config_file = mkdir_in_AppData(L"Movansha\\Remote Command Line") + L"\\config.json";
wstring saved_ss = mkdir_in_AppData(L"Movansha\\Remote Command Line\\Screenshots");

//----------

static void default_values() {
    config_json = {
        {"port-number", 12580},
        {"password", "R7kLqwsN8lYLOGMf"},
        {"dark-theme", false},
        {"beep-freq", 650},
        {"beep-duration", 1050}
    };

    ofstream file(config_file);
    file << config_json.dump(4);
    file.close();
}

void setup_json() {
    try {
        ifstream file(config_file);
        file >> config_json;
        file.close();
    }

    catch (...) {
        default_values();
    }
}

bool write_json() {
    try {
        std::ofstream file(config_file);
        file << config_json.dump(4);
        file.close();

        return true;
    }
    catch (...) {
        default_values();

        return false;
    }
}
