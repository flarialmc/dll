#include "ClientSettingsManager.hpp"
#include "ConfigManager.hpp"
#include <Client/Client.hpp>
#include <Config/Settings.hpp>
#include <Utils/Utils.hpp>
#include <Utils/Logger/Logger.hpp>
#include <Client/Module/Manager.hpp>
#include <Scripting/ScriptManager.hpp>
#include <lib/json/json.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

ClientSettingsManager& ClientSettingsManager::instance() {
    static ClientSettingsManager instance;
    return instance;
}

void ClientSettingsManager::loadLegacySettings() {
    auto& configMgr = ConfigManager::instance();

    if (fs::exists(configMgr.getLegacyDir()) && fs::is_directory(configMgr.getLegacyDir())) {
        Client::legacySettings.addSetting("currentConfig", std::string("default"));
        hasLegacySettings_ = true;
        softLoadLegacy_ = true;
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "Legacy dir already exists... aborting");
        return;
    }

    if (!fs::exists(configMgr.getLegacyPath())) {
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "No legacy settings found");
        return;
    }

    hasLegacySettings_ = true;
    Logger::custom(fg(fmt::color::dark_magenta), "Config", "Legacy settings found");

    std::error_code ec_rename;
    fs::rename(Utils::getConfigsPath(), Utils::getClientPath() + "\\Legacy", ec_rename);

    if (ec_rename) {
        LOG_ERROR("Failed to rename Config directory to Legacy: {}", ec_rename.message());
        return;
    }
    Logger::custom(fg(fmt::color::dark_magenta), "Config", "Renamed Config directory to Legacy");

    if (fs::create_directory(Utils::getConfigsPath())) {
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "Config directory successfully created");
    } else {
        LOG_ERROR("Failed to create Config directory");
        return;
    }

    std::string legacyDir = configMgr.getLegacyDir();
    fs::rename(Utils::getClientPath() + "\\Legacy", legacyDir, ec_rename);
    if (ec_rename) {
        LOG_ERROR("Failed to move Legacy folder to Config: {}", ec_rename.message());
        return;
    }
    Logger::custom(fg(fmt::color::dark_magenta), "Config", "Moved Legacy dir to Config dir");

    std::string legacyPath = legacyDir + "\\main.flarial";
    configMgr.setLegacyPath(legacyPath);

    std::ifstream legacyFile(legacyPath);
    if (!legacyFile) {
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "No legacy settings found");
        return;
    }

    std::stringstream ss;
    ss << legacyFile.rdbuf();
    legacyFile.close();
    Client::legacySettings.FromJson(ss.str(), true);
    Client::settings.AppendFromJson(ss.str(), true);

    if (Client::legacySettings.getSettingByName<std::string>("currentConfig") == nullptr) {
        Client::legacySettings.addSetting("currentConfig", std::string("default"));
    } else if (Client::legacySettings.getSettingByName<std::string>("currentConfig")->value.empty()) {
        Client::legacySettings.setValue("currentConfig", std::string("default"));
    }

    Logger::custom(fg(fmt::color::dark_magenta), "Config",
                   "Set config to {}", Client::legacySettings.getSettingByName<std::string>("currentConfig")->value);
}

void ClientSettingsManager::savePrivate() {
    if (savingPrivate_) return;
    savingPrivate_ = true;

    try {
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saving PRIVATE");

        if (!Client::settings.getSettingByName<std::string>("currentConfig")) {
            Logger::warn("No Client Settings to save to PRIVATE");
            savingPrivate_ = false;
            return;
        }

        if (privatePath_.empty()) {
            privatePath_ = Utils::getConfigsPath() + "\\PRIVATE";
        }

        DWORD originalAttributes = GetFileAttributesA(privatePath_.c_str());
        if (originalAttributes == INVALID_FILE_ATTRIBUTES) {
            LOG_ERROR("Failed to get attributes of PRIVATE: {}", GetLastError());
            savingPrivate_ = false;
            return;
        }

        DWORD attributesToClear = 0;
        if (originalAttributes & FILE_ATTRIBUTE_HIDDEN) {
            attributesToClear |= FILE_ATTRIBUTE_HIDDEN;
        }
        if (attributesToClear != 0) {
            if (!SetFileAttributesA(privatePath_.c_str(), originalAttributes & ~attributesToClear)) {
                LOG_ERROR("Failed to remove temporary attributes from PRIVATE: {}", GetLastError());
                savingPrivate_ = false;
                return;
            }
        }

        std::ofstream pCls(privatePath_, std::ofstream::out | std::ofstream::trunc);
        if (!pCls.is_open()) {
            LOG_ERROR("Could not clear the contents of PRIVATE: {}", GetLastError());
            savingPrivate_ = false;
            return;
        }
        pCls.close();

        std::ofstream pFile(privatePath_, std::ios::app);
        pFile << Client::settings.ToJson();
        pFile.close();

        Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saved PRIVATE");
        savingPrivate_ = false;
    } catch (const std::exception& e) {
        LOG_ERROR("An error occurred while trying to save settings: {}", e.what());
        savingPrivate_ = false;
    }
}

void ClientSettingsManager::loadPrivate() {
    Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loading PRIVATE");

    if (privatePath_.empty()) {
        privatePath_ = Utils::getConfigsPath() + "\\PRIVATE";
    }

    std::ifstream privateFile(privatePath_);
    if (!privateFile) {
        Logger::warn("Client PRIVATE not found");
        return;
    }

    std::stringstream pSS;
    pSS << privateFile.rdbuf();
    privateFile.close();

    std::string pStr = pSS.str();
    if (pStr.empty() || pStr == "null") {
        Logger::warn("Empty or null PRIVATE");
        return;
    }

    try {
        Client::settings.FromJson(pStr);
    } catch (const nlohmann::json::parse_error& e) {
        LOG_ERROR("Failed to parse JSON: {}", e.what());
    }

    auto& configMgr = ConfigManager::instance();
    auto* setting = Client::settings.getSettingByName<std::string>("currentConfig");
    if (setting) {
        configMgr.setConfigPath(Utils::getConfigsPath() + "\\" + setting->value);
        Client::path = configMgr.getConfigPath();
    }

    Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loaded PRIVATE");
}

void ClientSettingsManager::saveSettings() {
    if (savingSettings_) return;
    savingSettings_ = true;

    auto& configMgr = ConfigManager::instance();
    std::string path = configMgr.getConfigPath();

    if (path.empty()) {
        auto* setting = Client::settings.getSettingByName<std::string>("currentConfig");
        if (setting) {
            path = Utils::getConfigsPath() + "\\" + setting->value;
            configMgr.setConfigPath(path);
            Client::path = path;
        }
    }

    auto* configSetting = Client::settings.getSettingByName<std::string>("currentConfig");
    std::string configName;
    if (configSetting) {
        configName = configSetting->value;  // Copy the value to avoid dangling pointer issues
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saving {}", configName);
    }

    try {
        nlohmann::json configJson;

        for (const auto& [key, module] : ModuleManager::moduleMap) {
            if (module == nullptr) continue;

            try {
                std::string moduleSettingsStr = module->settings.ToJson();
                if (!moduleSettingsStr.empty()) {
                    nlohmann::json moduleSettings = nlohmann::json::parse(moduleSettingsStr);
                    configJson[module->name] = moduleSettings;
                }
            } catch (const nlohmann::json::parse_error& e) {
                LOG_ERROR("Failed to parse settings for module '{}': {}", module->name, e.what());
                continue;
            }
        }

        std::string tempPath = path + ".tmp";
        std::ofstream tempFile(tempPath, std::ofstream::out | std::ofstream::trunc);
        if (!tempFile.is_open()) {
            LOG_ERROR("Could not open temporary config file for writing");
            savingSettings_ = false;
            return;
        }

        tempFile << configJson.dump(2);
        tempFile.close();

        std::error_code ec;
        std::filesystem::rename(tempPath, path, ec);
        if (ec) {
            LOG_ERROR("Failed to rename temporary config file: {}", ec.message());
            std::filesystem::remove(tempPath, ec);
            savingSettings_ = false;
            return;
        }

        if (!configName.empty()) {
            Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saved {}", configName);
        }
        savingSettings_ = false;
    } catch (const std::exception& e) {
        LOG_ERROR("An error occurred while trying to save settings: {}", e.what());
        savingSettings_ = false;
    }

    ScriptManager::saveSettings();
}

void ClientSettingsManager::loadSettings() {
    if (!Client::settings.getSettingByName<std::string>("currentConfig")) {
        LOG_ERROR("No PRIVATE settings found");
        return;
    }

    auto* configSetting = Client::settings.getSettingByName<std::string>("currentConfig");
    std::string configName;
    if (configSetting) {
        configName = configSetting->value;  // Copy the value to avoid dangling pointer issues
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loading {}", configName);
    }

    auto& configMgr = ConfigManager::instance();
    std::string path = configMgr.getConfigPath();

    if (path.empty() && !configName.empty()) {
        path = Utils::getConfigsPath() + "\\" + configName;
        configMgr.setConfigPath(path);
        Client::path = path;
    }

    std::ifstream inputFile(path);
    if (!inputFile) {
        LOG_ERROR("Config file could not be loaded {}", GetLastError());
        return;
    }

    std::stringstream ss;
    ss << inputFile.rdbuf();
    inputFile.close();
    std::string str = ss.str();

    if (str.empty()) {
        Logger::warn("Settings String is empty");
        Client::globalSettings.clear();
        return;
    }

    try {
        Client::globalSettings = nlohmann::json::parse(str);
    } catch (const nlohmann::json::parse_error& e) {
        LOG_ERROR("Failed to parse JSON: {}", e.what());
    }

    if (!configName.empty()) {
        Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loaded {}", configName);
    }
}

void ClientSettingsManager::checkSettingsFile() {
    if (!fs::exists(Utils::getConfigsPath())) {
        fs::create_directory(Utils::getConfigsPath());
    }

    auto& configMgr = ConfigManager::instance();
    std::string path = configMgr.getConfigPath();

    if (!fs::exists(path)) {
        std::ofstream def(path, std::ios::app);
        if (!def) {
            LOG_ERROR("Failed to create config: {}", GetLastError());
        } else {
            def.close();
        }
    }

    if (privatePath_.empty()) {
        privatePath_ = Utils::getConfigsPath() + "\\PRIVATE";
    }

    if (!fs::exists(privatePath_)) {
        std::ofstream priv(privatePath_, std::ios::app);
        if (!priv) {
            LOG_ERROR("Failed to create PRIVATE: {}", GetLastError());
        } else {
            priv.close();
        }
    } else {
        privateInit_ = true;
    }
}
