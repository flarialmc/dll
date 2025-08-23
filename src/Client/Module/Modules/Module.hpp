#pragma once

#include "../../../Utils/Utils.hpp"
#include <string>
#include "../../../Config/Settings.hpp"
#include <fstream>
#include <iostream>
#include <windows.h>
#include <unknwn.h>
#include <filesystem>
#include <sstream>
#include <utility>
#include <vector>
#include "../../GUI/Engine/Engine.hpp"
#include "../../GUI/Engine/Constraints.hpp"
#include "../../../SDK/SDK.hpp"
#include "../../../Assets/Assets.hpp"
#include "../../Events/Input/KeyEvent.hpp"

class Module {

public:
	virtual ~Module() = default;

	std::string name;
	std::string description;
	int icon;
	bool isScriptingModule;
	std::string defaultKeybind;
	Settings settings;
	std::filesystem::path settingspath;
	std::vector<std::function<std::any(std::vector<std::any>)>> keybindActions;
	int totalKeybinds = 0;
	int totalWaypoints = 0;
	int totalmaps = 0;
	std::filesystem::path legacySettingsPath;

	std::map<std::string, DWRITE_TEXT_ALIGNMENT> alignments = {
		{"Left", DWRITE_TEXT_ALIGNMENT_LEADING},
		{"Center", DWRITE_TEXT_ALIGNMENT_CENTER},
		{"Right", DWRITE_TEXT_ALIGNMENT_TRAILING}
	};

	Module(const std::string& ename, const std::string& edescription, int eicon, const std::string& ekey, bool isScripting = false) {
		name = ename;
		description = edescription;
		icon = eicon;
		defaultKeybind = ekey;
		isScriptingModule = isScripting;
		settings = Settings();

		settingspath = isScripting ? Utils::getClientPath() + "\\Scripts\\Configs\\" + name + ".json" : "this is unused for non scripting modules";
	}

	void postConstructInitialize() {
		this->loadSettings();
	}

	bool active = false;
	bool enabledState = false;
	bool restricted = false;
	bool delayDisable = false;
	bool terminating = false;

	float padding = 0;
	int toggleIndex = 0;

	int sliderIndex = 0;
	std::vector<float> conditionalSliderAnims = std::vector<float>(100, Constraints::RelativeConstraint(0.05f, "height", true));
	std::vector<float> conditionalToggleAnims = std::vector<float>(100, Constraints::RelativeConstraint(0.05f, "height", true));
	std::vector<float> conditionalDropdownAnims = std::vector<float>(100, Constraints::RelativeConstraint(0.05f, "height", true));
	std::vector<float> conditionalTextBoxAnims = std::vector<float>(100, Constraints::RelativeConstraint(0.05f, "height", true));
	std::vector<float> conditionalColorPickerAnims = std::vector<float>(100, Constraints::RelativeConstraint(0.05f, "height", true));

	std::vector<DWRITE_TEXT_ALIGNMENT> prevAlignments = std::vector<DWRITE_TEXT_ALIGNMENT>(100, DWRITE_TEXT_ALIGNMENT_JUSTIFIED);

	int dropdownIndex = 0;
	int textboxIndex = 300;
	int keybindIndex = 0;
	int colorPickerIndex = 100;
	int buttonIndex = 3;

	struct ColorPickerStruct {
		std::string* value;
		float* opacity;
		bool* rgb;
	};

	std::unordered_map<int, std::string> color_pickers;
	std::unordered_map<int, ColorPickerStruct> color_pickers2;

	template <typename T>
	void setDef(std::string setting, T value) {
		this->settings.getOrAddSettingByName<T>(setting, value);
	}

	void setDef(std::string setting, std::string col, float opac, bool rgb) {
		this->settings.getOrAddSettingByName<std::string>(setting + "Col", col);
		this->settings.getOrAddSettingByName<float>(setting + "Opacity", opac);
		this->settings.getOrAddSettingByName<bool>(setting + "RGB", rgb);
	}

	void forceDef(std::string setting, std::string col, float opac, bool rgb) {
		if (this->settings.getSettingByName<std::string>(setting + "Col") != nullptr) this->settings.getSettingByName<std::string>(setting + "Col")->value = col;
		else this->settings.addSetting(setting + "Col", col);
		if (this->settings.getSettingByName<float>(setting + "Opacity") != nullptr)this->settings.getSettingByName<float>(setting + "Opacity")->value = opac;
		else this->settings.addSetting(setting + "Opacity", opac);
		if (this->settings.getSettingByName<bool>(setting + "RGB") != nullptr)this->settings.getSettingByName<bool>(setting + "RGB")->value = rgb;
		else this->settings.addSetting(setting + "RGB", rgb);
	}

	template <typename T>
	T& getOps(std::string setting) {
		return this->settings.getSettingByName<T>(setting)->value;
	}

	D2D_COLOR_F getColor(std::string text);
	D2D_COLOR_F getColor(std::string text, std::string mod);

	static void initSettingsPage();

	void resetPadding();
	void extraPadding();
	void addElementText(std::string text, std::string subtext = "");
	void addHeader(std::string text);

	void addButton(const std::string& text, const std::string& subtext, const std::string& buttonText,
		std::function<void()> action);

	void addConditionalTextBox(bool condition, std::string text, std::string subtext, std::string& value, int limit = 16);

	void addConditionalColorPicker(bool condition, std::string text, std::string subtext, std::string& value, float& opacity, bool& rgb);
	void addConditionalColorPicker(bool condition, std::string text, std::string subtext, std::string settingName);


	void addConditionalDropdown(bool condition, std::string text, std::string subtext, const std::vector<std::string>& options, std::string settingName, bool resettable);

	void addConditionalToggle(bool condition, std::string text, std::string subtext, bool& value);
	void addConditionalToggle(bool condition, std::string text, std::string subtext, std::string settingName);

	void addConditionalSlider(bool condition, std::string text, std::string subtext, std::string settingName, float maxVal = 100.f, float minVal = 0.f, bool zerosafe = true);
	void addConditionalSlider(bool condition, std::string text, std::string subtext, float& value, float maxVal = 100.0f, float minVal = 0.f, bool zerosafe = true);

	void addConditionalSliderInt(bool condition, std::string text, std::string subtext, std::string settingName, int maxVal = 100, int minVal = 0);

	void addSlider(std::string text, std::string subtext, float& value, float maxVal = 100.0f, float minVal = 0.f, bool zerosafe = true);
	void addSlider(std::string text, std::string subtext, std::string settingName, float maxVal = 100.0f, float minVal = 0.f, bool zerosafe = true);

	void addSliderInt(std::string text, std::string subtext, std::string settingName, int maxVal = 100, int minVal = 0);

	void addToggle(std::string text, std::string subtext, bool& value);
	void addToggle(std::string text, std::string subtext, std::string settingName);

	void addKeybind(std::string text, std::string subtext, std::string& keybind);
	void addKeybind(std::string text, std::string subtext, std::string settingName, bool resettable);

	void addTextBox(std::string text, std::string subtext, std::string& value, int limit = 16);
	void addTextBox(std::string text, std::string subtext, int limit, std::string settingName);

	void addDropdown(std::string text, std::string subtext, const std::vector<std::string>& options, std::string& value);
	void addDropdown(std::string text, std::string subtext, const std::vector<std::string>& options, std::string settingName, bool resettable);

	void addColorPicker(std::string text, std::string subtext, std::string& value, float& opacity, bool& rgb);
	void addColorPicker(std::string text, std::string subtext, std::string settingName);

	virtual void postLoad(bool softLoad = false);

	virtual void loadLegacySettings();
	virtual void loadSettings(bool softLoad = false);

	virtual void toggle();
	virtual void setup();
	virtual void onSetup();
	virtual void onEnable();
	virtual void onDisable();
	virtual void terminate();
	bool isEnabled();
	void setEnabled(bool enabled);
	void setKeybind(const std::string& newKeybind);
	std::string& getKeybind(const int keybindCount, bool whoCaresIfItsZeroOrNotTf);
	std::string& getKeybind(const int keybindCount = 0);
	virtual void defaultConfig(std::string type);
	virtual void defaultConfig();
	virtual void defaultAddSettings(std::string type);
	virtual void settingsRender(float settingsOffset) {}
	bool isKeybind(const std::array<bool, 256>& keys, const int keybindCount = 0);
	[[nodiscard]] bool isAdditionalKeybind(const std::array<bool, 256>& keys, const std::string& bind) const;
	bool isKeyPartOfKeybind(int keyCode, const int keybindCount = 0);
	static bool isKeyPartOfAdditionalKeybind(int keyCode, const std::string& bind);
	virtual void normalRenderCore(int index, std::string& text);
	virtual void normalRender(int index, std::string& value);
	void checkForRightClickAndOpenSettings(float x, float y, float width, float height);
	bool isScripting() const { return isScriptingModule; }
};
