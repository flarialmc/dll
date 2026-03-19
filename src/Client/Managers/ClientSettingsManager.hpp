#pragma once

#include <string>

class ClientSettingsManager {
public:
    static ClientSettingsManager& instance();

    // State accessors (these reference static Client members)
    [[nodiscard]] bool hasLegacySettings() const { return hasLegacySettings_; }
    [[nodiscard]] bool isSoftLoadLegacy() const { return softLoadLegacy_; }
    [[nodiscard]] bool isPrivateInitialized() const { return privateInit_; }
    [[nodiscard]] bool isSavingSettings() const { return savingSettings_; }
    [[nodiscard]] bool isSavingPrivate() const { return savingPrivate_; }

    // State mutators
    void setHasLegacySettings(bool value) { hasLegacySettings_ = value; }
    void setSoftLoadLegacy(bool value) { softLoadLegacy_ = value; }
    void setPrivateInitialized(bool value) { privateInit_ = value; }

    // Settings operations - these operate on Client::settings directly
    void loadSettings();
    void saveSettings();
    void loadPrivate();
    void savePrivate();
    void loadLegacySettings();
    void checkSettingsFile();

    // Delete copy/move
    ClientSettingsManager(const ClientSettingsManager&) = delete;
    ClientSettingsManager& operator=(const ClientSettingsManager&) = delete;
    ClientSettingsManager(ClientSettingsManager&&) = delete;
    ClientSettingsManager& operator=(ClientSettingsManager&&) = delete;

private:
    ClientSettingsManager() = default;
    ~ClientSettingsManager() = default;

    bool hasLegacySettings_ = false;
    bool softLoadLegacy_ = false;
    bool privateInit_ = false;
    bool savingSettings_ = false;
    bool savingPrivate_ = false;

    std::string privatePath_;
};
