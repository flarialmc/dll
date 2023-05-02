#include "Config.h"
#include "../Manager/Manager.h"
#include "../Manager/Module/Module.h"

/* std::string const Config::dir = Utils::getRoamPath() + "\\Flarial\\Configurations";

bool Config::check_cfg_folder() 
{
	std::filesystem::path file = std::filesystem::path(Config::dir);

	if (!std::filesystem::exists(file))
	{
		std::filesystem::create_directories(file);
	}

	return true;

}

json Config::load_cfg(std::string name)
{	
	Config::check_cfg_folder();

	std::filesystem::path cfg_file = std::filesystem::path(Config::dir + "\\" + name + ".json");

	if (std::filesystem::exists(cfg_file))
	{
		// do not make auto or else break
		const std::filesystem::path::value_type* text = cfg_file.c_str();
		json data = json::parse(std::ifstream(text));

		delete text;
		return data;
	}

	return NULL;
}

bool Config::save_cfg(std::string name, json data)
{
	Config::check_cfg_folder();

	const std::string target = Config::dir + "\\" + name + ".flarial";

	// add base json
	json base;
		
	base["flarial"] = {
		{ "version", "0.2" },
		{ "parser",  "0.1" }
	};
	base["modules"] = data;
	
	// save cfg to file
	std::ofstream file(target, std::ios::trunc);

	// this writes the data to the file
	// json.dump takes in the amount of spaces to indent
	file << base.dump(4) << std::endl;
	file.flush();
	file.close();

	return true;
}

json Config::get_modules_json(Manager* cfgmgr)
{
	json allmodulesdata;

	// This goes through every module so we can get its settings
	for (Module* module : Manager::modules)
	{
		json ModuleData;

		ModuleData["enabled"] = module->enabled;

		// Go through every BooleanSetting and append it to the ModuleData
		for (auto boolsetting : module->booleans)
		{
			ModuleData[boolsetting->getName()] = boolsetting->getStatus();
		}

		// go through every slider setting and append it to moduledata
		for (auto slidersetting : module->sliders)
		{
			ModuleData[slidersetting->getName()] = *slidersetting->getCurrent();
		}

		// Append the module to the json
		allmodulesdata[module->name] = ModuleData;
	}

	return allmodulesdata;
} */

/*
!!! CAUTION: TERRIBLE CODE
	- I am not responsible for any vision-loss, brain-impairment, or handicap you may recieve while reading this code.
	- This code was written to do the job, not be pretty.
*/
/* void Config::json_to_settings(json data, Manager* mgr)
{
	for (auto& it : data.items())
	{
		Module* target = nullptr;

		for (Module* mod : Manager::modules)
		{
			if (mod->name == it.key())
			{
				target = mod;
			}
		}

		if (!target) // incase we didn't find the module
			return;

		for (auto& setting : it.value())
		{

			if (setting.is_boolean()) // now we know its a BooleanSetting
			{
				if (setting == "enabled")
				{
					target->enabled = setting.get<bool>();
					continue;
				}

				for (auto mod_setting : target->booleans)
				{
					if (setting == mod_setting->getName())
					{
						mod_setting->setStatus(setting.get<bool>());
					}
				}
			}

			if (setting.is_number_float()) // slidersetting
			{
				for (auto mod_setting : target->sliders)
				{
					if (setting == mod_setting->getName())
					{
						mod_setting->setCurrent(setting.get<float>());
					}
				}
			}
		}

		delete target;
	}
} */