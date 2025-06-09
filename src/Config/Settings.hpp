#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <lib/json/json.hpp>
#include <Utils/Logger/Logger.hpp>

using json = nlohmann::json;

class Setting {
public:
	virtual ~Setting() = default;

	[[nodiscard]] virtual json ToJson() const = 0;

	virtual void FromJson(const json& jsonData) = 0;

	[[nodiscard]] virtual std::unique_ptr<Setting> clone() const = 0;
};

template<typename T>
class SettingType : public Setting {
public:
	SettingType(std::string name, T defaultValue)
		: name(std::move(name)), value(std::move(defaultValue)) {
	}

	[[nodiscard]] json ToJson() const override {
		return value;
	}

	void FromJson(const json& jsonData) override {
		if (!jsonData.is_primitive()) {
			Logger::error("Invalid JSON format for setting '{}'", name);
			return;
		}
		value = jsonData.get<T>();
	}

	[[nodiscard]] std::unique_ptr<Setting> clone() const override {
		return std::make_unique<SettingType<T>>(name, value);
	}

	std::string name;
	T value;
};

class Settings {
public:
	template<typename T>
	void addSetting(const std::string& name, const T& defaultValue) {
		settings.emplace(name, std::make_unique<SettingType<T>>(name, defaultValue));
	}

	void deleteSetting(const std::string& name) {
		auto it = settings.find(name);
		if (it != settings.end()) {
			settings.erase(it);
		}
		else {
			Logger::error("Setting not found: {}", name);
		}
	}

	void reset() {
		settings.clear();
	}

	void copyFrom(const Settings& from) {
		for (const auto& settingPair : from.settings) {
			settings[settingPair.first] = settingPair.second->clone();
		}
	}

	template<typename T>
	SettingType<T>* getSettingByName(const std::string& name) {
		auto it = settings.find(name);
		if (it != settings.end()) {
			return static_cast<SettingType<T> *>(it->second.get());
		}
		return nullptr;
	}

	void renameSetting(std::string oldName, std::string newName, bool clickguiColor = false) {
		if (clickguiColor) {
			renameSetting(oldName, "o_" + oldName, oldName + "_rgb", newName);
			return;
		}
		auto check = settings.find(newName);
		if (check != settings.end()) return;

		auto it = settings.find(oldName);
		if (it != settings.end()) {
			settings[newName] = std::move(it->second);
			settings.erase(it);
		}
	}

	void renameSetting(std::string oldCol, std::string oldOpacity, std::string oldRGB, std::string newName) {
		auto checkCol = settings.find(newName + "Col");
		if (checkCol == settings.end()) {
			auto itCol = settings.find(oldCol);
			if (itCol != settings.end()) {
				settings[newName + "Col"] = std::move(itCol->second);
				settings.erase(itCol);
			}
		}
		auto checkOpacity = settings.find(newName + "Opacity");
		if (checkOpacity == settings.end()) {
			auto itOpacity = settings.find(oldOpacity);
			if (itOpacity != settings.end()) {
				settings[newName + "Opacity"] = std::move(itOpacity->second);
				settings.erase(itOpacity);
			}
		}
		auto checkRGB = settings.find(newName + "RGB");
		if (checkRGB == settings.end()) {
			auto itRGB = settings.find(oldRGB);
			if (itRGB != settings.end()) {
				settings[newName + "RGB"] = std::move(itRGB->second);
				settings.erase(itRGB);
			}
		}
	}

	template<typename T>
	SettingType<T>* getOrAddSettingByName(const std::string& name, const T& defaultValue) {

		auto it = settings.find(name);
		if (it != settings.end()) return static_cast<SettingType<T> *>(it->second.get());
		addSetting(name, defaultValue);

		return getSettingByName<T>(name);
	}

	template<typename T>
	void setValue(const std::string& name, const T& value) {
		auto setting = getSettingByName<T>(name);
		if (setting) {
			setting->value = value;
		}
	}

	[[nodiscard]] std::string ToJson() const {
		json jsonData;
		for (const auto& settingPair : settings) {
			jsonData[settingPair.first] = settingPair.second->ToJson();
		}
		auto dump = jsonData.dump(4);
		return dump;
	}

	void AppendFromJson(const std::string& jsonString, bool old = false) {
		if (jsonString.empty()) {
			Logger::error("JSON string is empty");
			return;
		}

		try {
			json jsonData = json::parse(jsonString);

			if (!old && !jsonData.is_object()) return;

			if (old) for (const auto& item : jsonData) {
				std::string name = item["name"].get<std::string>();
				if (item["value"].is_number_float()) this->settings.emplace(name, std::make_unique<SettingType<float>>(name, item["value"].get<float>()));
				else if (item["value"].is_string()) this->settings.emplace(name, std::make_unique<SettingType<std::string>>(name, item["value"].get<std::string>()));
				else if (item["value"].is_boolean()) this->settings.emplace(name, std::make_unique<SettingType<bool>>(name, item["value"].get<bool>()));
				else Logger::warn("Unsupported or null value type for setting '{}'", name);
			}
			else for (const auto& [key, value] : jsonData.items()) {
				if (value.is_boolean()) this->settings[key] = std::make_unique<SettingType<bool>>(key, value.get<bool>());
				else if (value.is_number_float()) this->settings[key] = std::make_unique<SettingType<float>>(key, value.get<float>());
				else if (value.is_string()) this->settings[key] = std::make_unique<SettingType<std::string>>(key, value.get<std::string>());
				else Logger::warn("Unsupported JSON value type for setting '{}' during append. Skipping.", key);
			}
		}
		catch (const json::parse_error& e) {
			Logger::error("An error occurred while parsing settings: {}", e.what());
		}
	}

	void FromJson(const std::string& jsonString, bool old = false) {
		if (jsonString.empty()) {
			Logger::error("JSON string is empty");
			return;
		}

		try {
			json jsonData = json::parse(jsonString);

			if (!old && !jsonData.is_object()) return;

			std::unordered_map<std::string, std::unique_ptr<Setting>> newSettings;

			if (old) for (const auto& item : jsonData) {
				std::string name = item["name"].get<std::string>();
				if (item["value"].is_number_float()) newSettings.emplace(name, std::make_unique<SettingType<float>>(name, item["value"].get<float>()));
				else if (item["value"].is_string()) newSettings.emplace(name, std::make_unique<SettingType<std::string>>(name, item["value"].get<std::string>()));
				else if (item["value"].is_boolean()) newSettings.emplace(name, std::make_unique<SettingType<bool>>(name, item["value"].get<bool>()));
				else Logger::warn("Unsupported or null value type for setting '{}'", name);
			}
			else for (const auto& [key, value] : jsonData.items()) {
				if (value.is_boolean()) newSettings[key] = std::make_unique<SettingType<bool>>(key, value.get<bool>());
				else if (value.is_number_float()) newSettings[key] = std::make_unique<SettingType<float>>(key, value.get<float>());
				else if (value.is_string()) newSettings[key] = std::make_unique<SettingType<std::string>>(key, value.get<std::string>());
				else Logger::warn("Unsupported value type for setting '{}'", key);
			}

			settings = std::move(newSettings);
		}
		catch (const json::parse_error& e) {
			Logger::error("An error occurred while parsing settings: {}", e.what());
		}
	}

	std::unordered_map<std::string, std::unique_ptr<Setting>> settings;
};