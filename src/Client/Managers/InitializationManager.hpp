#pragma once

#include <string>
#include <cstdint>

class InitializationManager {
public:
    static InitializationManager& instance();

    // Lifecycle
    void initialize();
    void performPostLegacySetup();
    void shutdown();

    // State accessors
    [[nodiscard]] bool isInitialized() const { return init_; }
    [[nodiscard]] bool isDisabled() const { return disable_; }
    [[nodiscard]] const std::string& getVersion() const { return version_; }
    [[nodiscard]] const std::string& getCurrentCommit() const { return currentCommit_; }
    [[nodiscard]] float getElapsedTime() const { return elapsed_; }
    [[nodiscard]] uint64_t getStartTime() const { return start_; }

    // State mutators
    void setInitialized(bool value) { init_ = value; }
    void setDisabled(bool value) { disable_ = value; }
    void setVersion(const std::string& version) { version_ = version; }
    void setElapsedTime(float elapsed) { elapsed_ = elapsed; }
    void setStartTime(uint64_t start) { start_ = start; }

    // Delete copy/move
    InitializationManager(const InitializationManager&) = delete;
    InitializationManager& operator=(const InitializationManager&) = delete;
    InitializationManager(InitializationManager&&) = delete;
    InitializationManager& operator=(InitializationManager&&) = delete;

private:
    InitializationManager();
    ~InitializationManager() = default;

    bool init_ = false;
    bool disable_ = false;
    float elapsed_ = 0.0f;
    uint64_t start_ = 0;
    std::string version_;
    std::string currentCommit_;
};
