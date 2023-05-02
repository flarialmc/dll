#include "Module.h"

Module::Module(std::string n, std::string d, std::string ic, uint64_t k) {
	this->name = n;
	this->description = d;

	this->icon = ic;
	this->keybind = k;
};

void Module::addBool(std::string name, std::string tooltip, bool* var, bool visible) {
	this->settings.push_back(new BooleanSetting(name, tooltip, visible, var));
}

void Module::addFloat(std::string name, std::string tooltip, float min, float max, float* current, bool visible) {
	this->settings.push_back(new FloatSetting(name, tooltip, visible, min, max, current));
}

void Module::addColor(std::string name, std::string tooltip, float* color, bool visible) {
	this->settings.push_back(new ColorSetting(name, tooltip, visible, color));
}