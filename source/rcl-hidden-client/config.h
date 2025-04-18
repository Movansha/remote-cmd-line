#pragma once
#include "include/json.hpp"

using nlohmann::json;
using std::wstring;
using std::string;

//----------

extern json config_json;
extern string local_temp;

void setup_json();
