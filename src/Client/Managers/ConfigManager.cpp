#include "ConfigManager.hpp"
#include "ClientSettingsManager.hpp"
#include <Client/Client.hpp>
#include <Utils/Utils.hpp>
#include <Utils/Logger/Logger.hpp>
#include <fstream>
#include <algorithm>

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager()
    : path_(Utils::getConfigsPath() + "\\default.json"),
      legacyPath_(Utils::getConfigsPath() + "\\main.flarial"),
      legacyDir_(Utils::getConfigsPath() + "\\Legacy") {
}

void ConfigManager::createConfig(const std::string& name) {
    if (name.empty()) return;

    std::ofstream file(Utils::getConfigsPath() + "\\" + name, std::ios::app);
    if (!file) {
        LOG_ERROR("Failed to create new config file '{}'", name);
    } else {
        switchConfig(name);
    }
}

void ConfigManager::switchConfig(const std::string& name, bool deleting) {
    std::string configName = name;
    if (configName.empty()) {
        configName = "default.json";
    }

    if (!std::filesystem::exists(Utils::getConfigsPath() + "\\" + configName)) {
        createConfig(configName);
        return;
    }

    auto* setting = Client::settings.getSettingByName<std::string>("currentConfig");
    if (setting) {
        setting->value = configName;
    }
    activeConfig_ = configName;
    Client::activeConfig = configName;

    auto& settingsMgr = ClientSettingsManager::instance();
    if (settingsMgr.hasLegacySettings()) {
        std::string legacyName = configName;
        legacyName.resize(legacyName.length() - 5); // remove .json extension
        auto* legacySetting = Client::legacySettings.getSettingByName<std::string>("currentConfig");
        if (legacySetting) {
            legacySetting->value = legacyName;
        }
    }

    if (!deleting) {
        Client::SaveSettings();
    }
    Client::SavePrivate();
    Client::LoadPrivate();
}

void ConfigManager::deleteConfig(const std::string& name) {
    switchConfig("default.json", true);
    std::string pathToDelete = Utils::getConfigsPath() + "\\" + name;
    if (std::filesystem::exists(pathToDelete)) {
        std::filesystem::remove_all(pathToDelete);
    }
}

void ConfigManager::loadAvailableConfigs() {
    const std::string directoryPath = Utils::getConfigsPath();

    if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            if (entry.path().extension() == ".json" && !entry.path().filename().empty()) {
                availableConfigs_.push_back(entry.path().filename().string());
            }
        }
    }

    auto& settingsMgr = ClientSettingsManager::instance();
    if (settingsMgr.hasLegacySettings()) {
        if (!std::filesystem::exists(legacyDir_) || !std::filesystem::is_directory(legacyDir_)) {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(legacyDir_)) {
            if (is_directory(entry.path())) {
                std::string configName = entry.path().filename().string() + ".json";
                if (configName[0] != '!' &&
                    std::find(availableConfigs_.begin(), availableConfigs_.end(), configName) == availableConfigs_.end()) {
                    availableConfigs_.push_back(configName);
                }
            }
        }
    }
}
