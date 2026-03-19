#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <lib/json/json_fwd.hpp>
#include <windows.h>

// Forward declarations
class Settings;
class ConfigManager;
class ClientSettingsManager;
class WindowManager;
class InitializationManager;

#define ADD_SETTING(setting, value) \
if (Client::settings.getSettingByName<decltype(value)>(setting) == nullptr) \
Client::settings.addSetting(setting, value);

namespace fs = std::filesystem;

// Legacy Client class - forwards to new managers for backward compatibility
// New code should use the manager classes directly:
//   ConfigManager::instance()
//   ClientSettingsManager::instance()
//   WindowManager::instance()
//   InitializationManager::instance()
class Client {
public:
    // Thread ID - forwards to WindowManager
    static std::thread::id g_mainThreadId;

    // Version info - forwards to InitializationManager
    static std::string current_commit;
    static float elapsed;
    static uint64_t start;
    static std::string version;

    // Config - forwards to ConfigManager
    static std::vector<std::string> availableConfigs;
    static std::string activeConfig;
    static std::string path;
    static std::string legacyPath;
    static std::string legacyDir;

    // Window - forwards to WindowManager
    static HWND window;
    static HMODULE currentModule;

    // Settings - forwards to ClientSettingsManager
    static Settings settings;
    static Settings legacySettings;
    static nlohmann::json globalSettings;
    static bool hasLegacySettings;
    static bool softLoadLegacy;
    static bool privateInit;
    static bool savingSettings;
    static bool savingPrivate;

    // Initialization state - forwards to InitializationManager
    static bool init;
    static bool disable;

    class LocalPlayerState {
    private:
        bool _isInWorld{};
        std::string lastKnownUserName{};
        std::string lastKnownServerIP{};

        mutable std::shared_mutex playerStateMutex;
    public:
        const bool isInWorld();
        const std::string getUserName();
        const std::string getServerIP();

        void update(ClientInstance* client);
    };

    static LocalPlayerState& getPlayerState();

    // Helper functions
    static std::vector<std::string> getPlayersVector(const nlohmann::json& data);

    // Protocol handler
    static void UnregisterActivationHandler();

    // Config operations - forward to ConfigManager
    static void createConfig(std::string name);
    static void switchConfig(std::string name, bool deleting = false);
    static void deleteConfig(std::string name);
    static void loadAvailableConfigs();

    // Settings operations - forward to ClientSettingsManager
    static void LoadLegacySettings();
    static void SavePrivate();
    static void LoadPrivate();
    static void SaveSettings();
    static void LoadSettings();
    static void CheckSettingsFile();

    // Window operations - forward to WindowManager
    static void setWindowTitle(std::string title);
    static void centerCursor();

    // Initialization - forward to InitializationManager
    static void PerformPostLegacySetup();
    static void initialize();
};
