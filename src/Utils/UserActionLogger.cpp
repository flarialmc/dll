#include "UserActionLogger.hpp"
#include "Logger/Logger.hpp"
#include "Utils.hpp"
#include "CrashTelemetry.hpp"
#include "Telemetry.hpp"
#include <json/json.hpp>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include "SDK/SDK.hpp"
#include "Client/Module/Manager.hpp"

std::vector<UserAction> UserActionLogger::s_actions;
std::mutex UserActionLogger::s_mutex;
bool UserActionLogger::s_initialized = false;
std::string UserActionLogger::s_logDirectory = "";

void UserActionLogger::initialize() {
    if (s_initialized) return;

    std::lock_guard<std::mutex> lock(s_mutex);
    s_actions.reserve(MAX_ACTIONS);

    // Use the same logs directory as the main logger
    s_logDirectory = Utils::getLogsPath() + "\\";
    s_initialized = true;

    // Create log directory if it doesn't exist
    try {
        std::filesystem::create_directories(s_logDirectory);
    } catch (const std::exception& e) {
        Logger::warn("Failed to create user action log directory: {}", e.what());
    }

    Logger::debug("UserActionLogger initialized - Logs will be saved to: {}", s_logDirectory);
}

void UserActionLogger::logButtonClick(const std::string& buttonId, const std::string& position, bool success) {
    logAction("button_click", buttonId, "position: " + position, "mouse", success);
}

void UserActionLogger::logModuleToggle(const std::string& moduleName, bool enabled, const std::string& trigger) {
    std::string details = "enabled: " + std::string(enabled ? "true" : "false") + ", trigger: " + trigger;
    logAction("module_toggle", moduleName, details, trigger, true);
}

void UserActionLogger::logKeyPress(int keyCode, bool pressed, const std::string& context) {
    std::string details = "pressed: " + std::string(pressed ? "true" : "false");
    if (!context.empty()) {
        details += ", context: " + context;
    }
    logAction("key_press", "key_" + std::to_string(keyCode), details, "keyboard", true);
}


void UserActionLogger::logScreenChange(const std::string& fromScreen, const std::string& toScreen) {
    std::string details = "from: " + fromScreen + ", to: " + toScreen;
    logAction("screen_change", "navigation", details, "system", true);
}

void UserActionLogger::logCommandExecution(const std::string& command, bool success, const std::string& result) {
    std::string details = "result: " + result;
    logAction("command", command, details, "keyboard", success);
}

void UserActionLogger::logSettingsAccess(const std::string& moduleName, const std::string& action) {
    logAction("settings_access", moduleName, "action: " + action, "mouse", true);
}

void UserActionLogger::logGuiInteraction(const std::string& element, const std::string& action, const std::string& value) {
    std::string details = "action: " + action;
    if (!value.empty()) {
        details += ", value: " + value;
    }
    logAction("gui_interaction", element, details, "mouse", true);
}

void UserActionLogger::logAction(const std::string& type, const std::string& target, const std::string& details,
                                const std::string& inputMethod, bool success, int responseTimeMs) {
    if (!s_initialized) {
        initialize();
    }

    UserAction action;
    action.timestamp = std::chrono::system_clock::now();
    action.type = type;
    action.target = target;
    action.details = details;
    action.screen = getCurrentScreen();
    action.inputMethod = inputMethod;
    action.success = success;
    action.responseTimeMs = responseTimeMs;

    std::lock_guard<std::mutex> lock(s_mutex);

    // Add new action
    s_actions.push_back(action);

    // Remove old actions if we exceed the limit
    if (s_actions.size() > MAX_ACTIONS) {
        s_actions.erase(s_actions.begin(), s_actions.begin() + (s_actions.size() - MAX_ACTIONS));
    }
}

nlohmann::json UserActionLogger::getRecentActions(int maxActions) {
    std::lock_guard<std::mutex> lock(s_mutex);

    nlohmann::json actions = nlohmann::json::array();

    int startIndex = std::max(0, static_cast<int>(s_actions.size()) - maxActions);

    for (size_t i = startIndex; i < s_actions.size(); ++i) {
        const auto& action = s_actions[i];

        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            action.timestamp.time_since_epoch()
        ).count();

        nlohmann::json actionJson = {
            {"timestamp", timestamp},
            {"type", action.type},
            {"target", action.target},
            {"details", action.details},
            {"screen", action.screen},
            {"inputMethod", action.inputMethod},
            {"success", action.success},
            {"responseTimeMs", action.responseTimeMs}
        };

        actions.push_back(actionJson);
    }

    return actions;
}

nlohmann::json UserActionLogger::getCurrentState() {
    return {
        {"currentScreen", getCurrentScreen()},
        {"activeModules", getActiveModules()},
        {"totalActionsLogged", s_actions.size()},
        {"timestamp", std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()}
    };
}

void UserActionLogger::cleanup() {
    std::lock_guard<std::mutex> lock(s_mutex);

    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(1); // Keep last hour of actions

    s_actions.erase(
        std::remove_if(s_actions.begin(), s_actions.end(),
            [cutoff](const UserAction& action) {
                return action.timestamp < cutoff;
            }),
        s_actions.end()
    );
}

std::string UserActionLogger::getCurrentScreen() {
    try {
        if (!SDK::currentScreen.empty()) {
            return SDK::currentScreen;
        }
        return "unknown";
    } catch (...) {
        return "error";
    }
}

std::vector<std::string> UserActionLogger::getActiveModules() {
    std::vector<std::string> activeModules;

    try {
        for (const auto& pair : ModuleManager::moduleMap) {
            if (pair.second && pair.second->isEnabled()) {
                activeModules.push_back(pair.second->name);
            }
        }
    } catch (...) {
        // Return empty vector if there's an error
    }

    return activeModules;
}

std::string UserActionLogger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void UserActionLogger::setLogDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_logDirectory = directory;

    // Ensure directory ends with a separator
    if (!s_logDirectory.empty() && s_logDirectory.back() != '/' && s_logDirectory.back() != '\\') {
        s_logDirectory += "/";
    }
}

void UserActionLogger::exportToFile(const std::string& filename) {
    try {
        // Create log directory if it doesn't exist
        std::filesystem::create_directories(s_logDirectory);

        // Generate filename if not provided
        std::string logFilename = filename;
        if (logFilename.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "user_actions_" << std::put_time(std::localtime(&time_t), "%Y-%m-%d_%H-%M-%S") << ".json";
            logFilename = ss.str();
        }

        std::string fullPath = s_logDirectory + logFilename;
        std::ofstream file(fullPath);

        if (!file.is_open()) {
            Logger::warn("Failed to create user action log file: {}", fullPath);
            return;
        }

        // Build a mock crash report with current data (same structure as API payload)
        nlohmann::json exportData;

        try {
            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()
            ).count();

            exportData = {
                {"crashId", "export-" + std::to_string(timestamp)},
                {"timestamp", timestamp},
                {"clientInfo", CrashTelemetry::getClientInfo()},
                {"crashInfo", {
                    {"type", "export"},
                    {"message", "Manual export - no crash"},
                    {"stackTrace", "No crash occurred - this is a manual export"}
                }},
                {"systemInfo", CrashTelemetry::getSystemInfo()},
                {"hardwareInfo", CrashTelemetry::getHardwareInfo()},
                {"sessionInfo", CrashTelemetry::getSessionInfo()},
                {"additionalData", {
                    {"logFiles", nlohmann::json::array()},
                    {"moduleStates", CrashTelemetry::getModuleStates()},
                    {"userActions", nlohmann::json::array()},
                    {"logs", CrashTelemetry::getLatestLogContent()}
                }}
            };

            // Add the actual user actions
            {
                std::lock_guard<std::mutex> lock(s_mutex);
                Logger::debug("Exporting {} user actions to file", s_actions.size());

                int maxActions = 100;
                int startIndex = std::max(0, static_cast<int>(s_actions.size()) - maxActions);

                for (size_t i = startIndex; i < s_actions.size(); ++i) {
                    const auto& action = s_actions[i];

                    auto actionTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
                        action.timestamp.time_since_epoch()
                    ).count();

                    nlohmann::json actionJson = {
                        {"timestamp", actionTimestamp},
                        {"type", action.type},
                        {"target", action.target},
                        {"details", action.details},
                        {"screen", action.screen},
                        {"inputMethod", action.inputMethod},
                        {"success", action.success},
                        {"responseTimeMs", action.responseTimeMs}
                    };

                    exportData["additionalData"]["userActions"].push_back(actionJson);
                }
            }
        } catch (const std::exception& e) {
            exportData = nlohmann::json::object();
            exportData["error"] = "Failed to build export data: " + std::string(e.what());
            exportData["initialized"] = s_initialized;
        }

        file << exportData.dump(2); // Pretty print with 2-space indentation
        file.close();

        Logger::info("User action log exported to: {} (contains {} actions)", fullPath, exportData.is_array() ? exportData.size() : 0);

    } catch (const std::exception& e) {
        Logger::warn("Failed to export user action log: {}", e.what());
    }
}