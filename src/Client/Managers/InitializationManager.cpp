#include "InitializationManager.hpp"
#include "ConfigManager.hpp"
#include "ClientSettingsManager.hpp"
#include "WindowManager.hpp"
#include <Client/Client.hpp>
#include <Config/Settings.hpp>
#include <Utils/Logger/Logger.hpp>
#include <filesystem>

namespace fs = std::filesystem;

InitializationManager& InitializationManager::instance() {
    static InitializationManager instance;
    return instance;
}

InitializationManager::InitializationManager()
    : currentCommit_(COMMIT_HASH) {
}

void InitializationManager::performPostLegacySetup() {
    auto& settingsMgr = ClientSettingsManager::instance();
    auto& configMgr = ConfigManager::instance();

    if (settingsMgr.hasLegacySettings()) {
        fs::path pathToCheck;
        auto* legacyConfigSetting = Client::legacySettings.getSettingByName<std::string>("currentConfig");

        if (legacyConfigSetting && legacyConfigSetting->value != "default") {
            pathToCheck = fs::path(configMgr.getLegacyDir()) /
                          (legacyConfigSetting->value);
        } else {
            pathToCheck = configMgr.getLegacyDir();
        }

        if (fs::exists(pathToCheck) && fs::is_directory(pathToCheck) && pathToCheck.filename().string()[0] != '!') {
            if (legacyConfigSetting && legacyConfigSetting->value == "default") {
                std::string defaultPath = configMgr.getLegacyDir() + "\\!default";
                if (!fs::exists(defaultPath)) {
                    if (!fs::create_directory(defaultPath)) {
                        LOG_ERROR("Failed to create new default directory for Legacy");
                        return;
                    }
                }

                for (const auto& entry : fs::directory_iterator(configMgr.getLegacyDir())) {
                    if (entry.is_regular_file() && entry.path().extension() == ".flarial") {
                        try {
                            fs::rename(entry.path(), defaultPath / entry.path().filename());
                        } catch (fs::filesystem_error& e) {
                            LOG_ERROR("Failed to move file {} to legacy default folder",
                                      entry.path().filename().string());
                            return;
                        }
                    }
                }
            } else {
                std::error_code ec_rename;
                fs::rename(pathToCheck, pathToCheck.parent_path().string() + "\\!" + pathToCheck.filename().string(),
                           ec_rename);

                if (ec_rename) {
                    LOG_ERROR("Failed to exclude legacy setting: {}", ec_rename.message());
                    return;
                }
            }
        }
    }
}

void InitializationManager::shutdown() {
    disable_ = true;
    init_ = false;
}
