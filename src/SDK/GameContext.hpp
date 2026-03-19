#pragma once

#include <string>
#include <atomic>
#include <cstdint>

// Forward declarations
class ClientInstance;

class GameContext {
public:
    static GameContext& instance();

    // Client instance
    [[nodiscard]] ClientInstance* getClientInstance() const { return clientInstance_; }
    void setClientInstance(ClientInstance* instance);

    // Initialization state
    [[nodiscard]] bool hasInstanced() const { return hasInstanced_.load(); }
    void setHasInstanced(bool value) { hasInstanced_.store(value); }

    // Current screen
    [[nodiscard]] const std::string& getCurrentScreen() const { return currentScreen_; }
    void setCurrentScreen(const std::string& screen);

    // Server info
    [[nodiscard]] uint64_t getServerPing() const { return serverPing_; }
    void setServerPing(uint64_t ping) { serverPing_ = ping; }
    [[nodiscard]] int getServerPingInt() const;
    [[nodiscard]] std::string getServerIP() const;
    [[nodiscard]] std::string getServerPort() const;

    // Delete copy/move
    GameContext(const GameContext&) = delete;
    GameContext& operator=(const GameContext&) = delete;
    GameContext(GameContext&&) = delete;
    GameContext& operator=(GameContext&&) = delete;

private:
    GameContext() = default;
    ~GameContext() = default;

    ClientInstance* clientInstance_ = nullptr;
    std::atomic<bool> hasInstanced_{false};
    std::string currentScreen_;
    uint64_t serverPing_ = 0;
};
