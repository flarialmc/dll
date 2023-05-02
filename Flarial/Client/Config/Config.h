#pragma once

#include "json/json.hpp"
#include "../Utils/Utils.h"
#include "../Utils/Logger.h"

#include <string>
#include <filesystem>
#include <fstream>

/* class Manager;
using json = nlohmann::json;

class Config {
public:
	// config path
	static std::string const dir;
public:
	// check if the config folder exists, if not it will create the folder
	static bool check_cfg_folder();
public:
	// load a config
	static json load_cfg(std::string);
	// save cfg
	static bool save_cfg(std::string, json);
public:
	// module stuff
	static json get_modules_json(Manager*);
	static void json_to_settings(json data, Manager* mgr);
}; */