#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class ConfigManager {
public:
    static ConfigManager& instance();

    // Config operations
    void createConfig(const std::string& name);
    void switchConfig(const std::string& name, bool deleting = false);
    void deleteConfig(const std::string& name);
    void loadAvailableConfigs();

    // Accessors
    [[nodiscard]] const std::string& getActiveConfig() const { return activeConfig_; }
    [[nodiscard]] const std::vector<std::string>& getAvailableConfigs() const { return availableConfigs_; }
    [[nodiscard]] std::vector<std::string>& getAvailableConfigsMut() { return availableConfigs_; }
    [[nodiscard]] const std::string& getConfigPath() const { return path_; }
    [[nodiscard]] const std::string& getLegacyPath() const { return legacyPath_; }
    [[nodiscard]] const std::string& getLegacyDir() const { return legacyDir_; }

    // Mutators
    void setActiveConfig(const std::string& config) { activeConfig_ = config; }
    void setConfigPath(const std::string& path) { path_ = path; }
    void setLegacyPath(const std::string& path) { legacyPath_ = path; }
    void setLegacyDir(const std::string& dir) { legacyDir_ = dir; }

    // Delete copy/move
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(ConfigManager&&) = delete;

private:
    ConfigManager();
    ~ConfigManager() = default;

    std::string activeConfig_;
    std::vector<std::string> availableConfigs_;
    std::string path_;
    std::string legacyPath_;
    std::string legacyDir_;
};
