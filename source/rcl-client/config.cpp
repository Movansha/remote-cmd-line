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
using std::string;

json config_json;

//----------

//----------

static wstring get_path_AppData() {
    PWSTR temp = NULL;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &temp);
    return wstring(temp);
}

static string get_path_LocalAppData() {
    PWSTR temp = NULL;
    string output;

    if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &temp) == S_OK) {
        wstring path = wstring(temp);

        size_t converted = 0;
        size_t buffer_size = path.length() + 1;
        char* buffer = new char[buffer_size];

        const wchar_t* wchar_path = path.c_str();
        errno_t err = wcstombs_s(&converted, buffer, buffer_size, wchar_path, path.length());

        if (err == 0) output = string(buffer) + "\\";
        else output = "";

        delete[] buffer;
        CoTaskMemFree(temp);
    }

    return output;
}

static wstring mkdir_in_AppData(const wstring& path) {
    wstring folder = get_path_AppData() + L"\\" + path;

    if (!std::filesystem::exists(folder)) std::filesystem::create_directories(folder);

    return folder;
}


static wstring config_file = mkdir_in_AppData(L"Movansha\\RCL - Client") + L"\\config.json";
string local_temp = get_path_LocalAppData() + "Temp\\output";

//----------

static void default_values() {
    config_json = {
        {"ip-adress", "example.movansha.com"},
        {"port-number", 12580},
        {"password", "R7kLqwsN8lYLOGMf"},
        {"auto-connect", true},
        {"dark-mode", false}
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
