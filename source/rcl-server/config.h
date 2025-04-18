#pragma once
#include "include/json.hpp"

using nlohmann::json;
using std::wstring;

//----------

extern json config_json;
extern wstring saved_ss;

void setup_json();
bool write_json();
