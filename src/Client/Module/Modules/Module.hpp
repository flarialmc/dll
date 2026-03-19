#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <any>
#include <filesystem>
#include <map>
#include <array>
#include "../../../Config/Settings.hpp"
#include "../IModule.hpp"

// Minimal includes for D2D/DWrite types
#include <d2d1.h>
#include <dwrite.h>

class Module : public IModule {

public:
	virtual ~Module() = default;

	std::string name;
	std::string tooltip;
	std::string description;
	int icon;
	bool isScriptingModule;
	std::string defaultKeybind;
	std::vector<std::string> aliases;
	Settings settings;
	std::filesystem::path settingspath;
	std::vector<std::function<std::any(std::vector<std::any>)>> keybindActions;
	int totalKeybinds = 0;
	int totalWaypoints = 0;
	int totalmaps = 0;
	std::filesystem::path legacySettingsPath;

	std::map<std::string, DWRITE_TEXT_ALIGNMENT> alignments;

	// Constructor declaration only
	Module(const std::string& ename, const std::string& edescription, int eicon, const std::string& ekey, bool isScripting = false, const std::vector<std::string>& ealiases = {});
	Module(const std::string& ename, const std::string& etooltip, const std::string& edescription, int eicon, const std::string& ekey, bool isScripting = false, const std::vector<std::string>& ealiases = {});

	void postConstructInitialize();

	bool active = false;
	bool enabledState = false;
	bool restricted = false;
	bool delayDisable = false;
	bool terminating = false;

	float padding = 0;
	float lastPadding = 0;  // Store padding from previous frame for scroll calculations
	int toggleIndex = 0;

	int sliderIndex = 0;
	std::vector<float> conditionalSliderAnims;
	std::vector<float> conditionalToggleAnims;
	std::vector<float> conditionalDropdownAnims;
	std::vector<float> conditionalTextBoxAnims;
	std::vector<float> conditionalColorPickerAnims;
	std::vector<float> conditionalKeybindAnims;
	std::vector<float> conditionalButtonAnims;

	std::vector<DWRITE_TEXT_ALIGNMENT> prevAlignments;

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

	/// Creates a setting with a default value, or retrieves it if it already exists.
	template <typename T>
	void setDef(std::string setting, T value) {
		this->settings.getOrAddSettingByName<T>(setting, value);
	}

	void setDef(const std::string &setting, const std::string &col, float opac, bool rgb);
	void forceDef(const std::string& setting, const std::string& col, float opac, bool rgb);

	/// Retrieves a module setting value by name. Commonly used to read config options.
	template <typename T>
	T& getOps(std::string setting) {
		return this->settings.getSettingByName<T>(setting)->value;
	}

	/// Gets a D2D color from this module's settings, respecting RGB mode and opacity.
	D2D_COLOR_F getColor(const std::string &text);

	/// Gets a D2D color from a specific module's settings by module name.
	static D2D_COLOR_F getColor(const std::string& text, const std::string& mod);

	/// Initializes the scrollable settings panel layout for this module in the ClickGUI.
	void initSettingsPage();

	void resetPadding();
	void extraPadding(float percent = 0.04f);
	void addElementText(const std::string& text, const std::string& subtext = "");
	void addElementTextWithBackground(const std::string& text, const std::string& subtext = "");
	void addHeader(const std::string& text);

	void addButton(const std::string& text, const std::string& subtext, const std::string& buttonText,
		std::function<void()> action);

	void addConditionalButton(bool condition, const std::string& text, const std::string& subtext, const std::string& buttonText,
		std::function<void()> action);

	void addConditionalTextBox(bool condition, const std::string& text, const std::string& subtext, std::string& value, int limit = 16);
	void addConditionalTextBox(bool condition, const std::string& text, const std::string& subtext, int limit, const std::string& settingName);

	void addConditionalColorPicker(bool condition, const std::string& text, const std::string& subtext, std::string& value, float& opacity, bool& rgb);
	void addConditionalColorPicker(bool condition, const std::string& text, const std::string& subtext, const std::string& settingName);

	void addConditionalDropdown(bool condition, const std::string& text, const std::string& subtext, const std::vector<std::string>& options, const std::string& settingName, bool resettable);

	void addConditionalToggle(bool condition, const std::string& text, const std::string& subtext, bool& value);
	void addConditionalToggle(bool condition, const std::string& text, const std::string& subtext, const std::string& settingName);

	void addConditionalSlider(bool condition, const std::string& text, const std::string& subtext, const std::string& settingName, float maxVal = 100.f, float minVal = 0.f, bool zerosafe = true);
	void addConditionalSlider(bool condition, const std::string& text, const std::string& subtext, float& value, float maxVal = 100.0f, float minVal = 0.f, bool zerosafe = true);

	void addConditionalSliderInt(bool condition, const std::string& text, const std::string& subtext, const std::string& settingName, int maxVal = 100, int minVal = 0);

	void addConditionalKeybind(bool condition, const std::string& text, const std::string& subtext, std::string& keybind);
	void addConditionalKeybind(bool condition, const std::string& text, const std::string& subtext, const std::string& settingName, bool resettable);

	void addSlider(const std::string& text, const std::string& subtext, float& value, float maxVal = 100.0f, float minVal = 0.f, bool zerosafe = true);
	void addSlider(const std::string& text, const std::string& subtext, const std::string& settingName, float maxVal = 100.0f, float minVal = 0.f, bool zerosafe = true);

	void addSliderInt(const std::string& text, const std::string& subtext, const std::string& settingName, int maxVal = 100, int minVal = 0);

	void addRangeSlider(const std::string& text, const std::string& subtext, const std::string& minSettingName, const std::string& maxSettingName, float maxVal = 100.0f, float minVal = 0.f);

	void addToggle(const std::string& text, const std::string& subtext, bool& value);
	void addToggle(const std::string& text, const std::string& subtext, const std::string& settingName);

	void addKeybind(const std::string& text, const std::string& subtext, std::string& keybind);
	void addKeybind(const std::string& text, const std::string& subtext, const std::string& settingName, bool resettable);

	void addTextBox(const std::string& text, const std::string& subtext, std::string& value, int limit = 16);
	void addTextBox(const std::string& text, const std::string& subtext, int limit, const std::string& settingName);

	void addDropdown(const std::string& text, const std::string& subtext, const std::vector<std::string>& options, std::string& value);
	void addDropdown(const std::string& text, const std::string& subtext, const std::vector<std::string>& options, const std::string& settingName, bool resettable);

	void addColorPicker(const std::string& text, const std::string& subtext, std::string& value, float& opacity, bool& rgb);
	void addColorPicker(const std::string& text, const std::string& subtext, const std::string& settingName);

	virtual void postLoad(bool softLoad = false);

	virtual void loadLegacySettings();
	virtual void loadSettings(bool softLoad = false);

	virtual void toggle();
	virtual void setup();
	virtual void onSetup();
	virtual void onEnable();
	virtual void onDisable();
	virtual void terminate();
	// IModule interface implementation
	bool isEnabled() const override;
	bool isActive() const override { return active; }
	const std::string& getName() const override { return name; }

	void setEnabled(bool enabled);
	void setKeybind(const std::string& newKeybind);
	std::string& getKeybind(int keybindCount, bool whoCaresIfItsZeroOrNotTf);
	std::string& getKeybind(int keybindCount = 0);
	virtual void defaultConfig(std::string type);
	virtual void defaultConfig();
	virtual void defaultAddSettings(std::string type);
	virtual void settingsRender(float settingsOffset) {}
	/// Returns true if the given key state matches this module's keybind (supports multi-key combos).
	bool isKeybind(const std::array<bool, 256>& keys, int keybindCount = 0);
	/// Checks if a key state matches a specific additional keybind string.
	[[nodiscard]] bool isAdditionalKeybind(const std::array<bool, 256>& keys, const std::string& bind) const;
	/// Returns true if keyCode is any part of this module's keybind combination.
	bool isKeyPartOfKeybind(int keyCode, int keybindCount = 0);
	static bool isKeyPartOfAdditionalKeybind(int keyCode, const std::string& bind);
	virtual void normalRenderCore(int index, std::string& text);
	virtual void normalRender(int index, std::string& value);
	/// Detects right-click within the given HUD element bounds and opens this module's settings panel.
	void checkForRightClickAndOpenSettings(float x, float y, float width, float height);
	bool isScripting() const { return isScriptingModule; }
};
