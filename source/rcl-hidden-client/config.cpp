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

static string config_file = "config.json";
string local_temp = get_path_LocalAppData() + "Temp\\output";

//----------

static void default_values() {
    config_json = {
        {"_comment", "If you have changed this file incorrectly, you may delete the file and restart the app."},
        {"ip-adress", "example.movansha.com"},
        {"port-number", 12580},
        {"password", "R7kLqwsN8lYLOGMf"},
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
