#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <json/json_fwd.hpp>

struct UserAction {
    std::chrono::system_clock::time_point timestamp;
    std::string type;
    std::string target;
    std::string details;
    std::string screen;
    std::string inputMethod;
    bool success;
    int responseTimeMs;
};

class UserActionLogger {
public:
    // Initialize the logging system
    static void initialize();

    // Log different types of user actions
    static void logButtonClick(const std::string& buttonId, const std::string& position, bool success = true);
    static void logModuleToggle(const std::string& moduleName, bool enabled, const std::string& trigger = "unknown");
    static void logKeyPress(int keyCode, bool pressed, const std::string& context = "");
    static void logScreenChange(const std::string& fromScreen, const std::string& toScreen);
    static void logCommandExecution(const std::string& command, bool success, const std::string& result = "");
    static void logSettingsAccess(const std::string& moduleName, const std::string& action);
    static void logGuiInteraction(const std::string& element, const std::string& action, const std::string& value = "");

    // Get recent actions for crash telemetry
    static nlohmann::json getRecentActions(int maxActions = 20);

    // Get current system state
    static nlohmann::json getCurrentState();

    // Clear old actions (called periodically)
    static void cleanup();

    // Export actions to text file for manual review
    static void exportToFile(const std::string& filename = "");

    // Set the directory for log files
    static void setLogDirectory(const std::string& directory);

private:
    // Core logging function
    static void logAction(const std::string& type, const std::string& target, const std::string& details,
                         const std::string& inputMethod = "unknown", bool success = true, int responseTimeMs = 0);

    // Helper functions
    static std::string getCurrentScreen();
    static std::vector<std::string> getActiveModules();
    static std::string getCurrentTimestamp();

    // Storage
    static std::vector<UserAction> s_actions;
    static std::mutex s_mutex;
    static bool s_initialized;
    static std::string s_logDirectory;
    static constexpr size_t MAX_ACTIONS = 100; // Keep last 100 actions
};