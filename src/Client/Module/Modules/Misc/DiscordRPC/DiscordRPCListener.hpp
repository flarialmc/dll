#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Client.hpp"
#include "../HiveModeCatcher/HiveModeCatcherListener.hpp"
#include "Utils/PlatformUtils.hpp"
#include "Utils/Threading.hpp"

#include <discord-rpc.hpp>

static std::mutex g_pendingTitleMutex;
static std::optional<std::string> g_pendingTitle;
static std::wstring g_lastAppliedTitle;

// Separate function for SEH: __try/__except can't coexist with C++ destructors
// in the same function (std::lock_guard etc.), so we isolate the vtable call.
static LocalPlayer* safeGetLocalPlayer() {
    auto* ci = SDK::clientInstance;
    if (!ci) return nullptr;
    __try {
        return ci->getLocalPlayer();
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }
}

class DiscordRPCListener : public Listener {
private:
    static constexpr const char* CLIENT_ID = "1058426966602174474";

    struct ServerInfo {
        std::string name;
        std::string imageKey;
    };

    std::string previousIp;
    std::string previousGamemode;
    std::atomic<bool> rpcInitialized = false;
    std::mutex rpcMutex; // protects previousIp, previousGamemode, and Presence singleton access

public:
    DiscordRPCListener() {
        Listen(this, TickEvent, &DiscordRPCListener::onTick);
        Listen(this, RenderEvent, &DiscordRPCListener::onRender);
        initializeRPC();
    }

    ~DiscordRPCListener() {
        Deafen(this, TickEvent, &DiscordRPCListener::onTick);
        Deafen(this, RenderEvent, &DiscordRPCListener::onRender);
        shutdownRPC();
    }

    void onTick(TickEvent& event) {
        if (!rpcInitialized || Client::disable) return;
        handleServerIpChange();
    }

    void onRender(RenderEvent& event) {
        if (!rpcInitialized || Client::disable) return;

        if (SDK::clientInstance && !safeGetLocalPlayer()) {
            // No local player — we left the world or are on main menu
            std::lock_guard lk(rpcMutex);
            if (previousIp != "none") {
                previousIp = "none";
                previousGamemode.clear();
                updatePresence("none");
                if (!PlatformUtils::isUWP()) {
                    updateWindowTitle("none");
                }
            }
        }
    }

private:
    void initializeRPC() {
        try {
            auto& rpc = discord::RPCManager::get();
            rpc.setClientID(CLIENT_ID);
            rpc.onReady([](const discord::User& user) {
                Logger::info("Discord RPC connected for user: {}", user.username);
            });
            rpc.onDisconnected([](int code, std::string_view message) {
                Logger::warn("Discord RPC disconnected ({}): {}", code, message);
            });
            rpc.onErrored([](int code, std::string_view message) {
                Logger::error("Discord RPC error ({}): {}", code, message);
            });
            rpc.initialize();
            rpcInitialized = true;

            // Set initial presence
            auto& presence = rpc.getPresence();
            presence.setDetails("Ready to play")
                .setLargeImageKey("flarialbig")
                .setLargeImageText("Flarial Client")
                .setStartTimestamp(std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count())
                .setButton1("Website", "https://flarial.xyz", true)
                .setButton2("Discord Server", "https://flarial.xyz/discord", true);
            presence.refresh();

            // Set initial window title and tracking state
            previousIp = "none";
            if (!PlatformUtils::isUWP()) {
                updateWindowTitle("none");
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR("Failed to initialize Discord RPC: {}", e.what());
            rpcInitialized = false;
        }
    }

    void shutdownRPC() {
        if (rpcInitialized.exchange(false)) {
            // rpcInitialized is now false — event handlers will bail out
            std::lock_guard lk(rpcMutex); // wait for any in-flight handler to finish
            try {
                discord::RPCManager::get().clearPresence().shutdown();
            }
            catch (...) {}
        }
    }

    ServerInfo getServerInfo(const std::string& ip) const {
        if (ip.find("hive") != std::string::npos)
            return {"Hive Network", "hivemc"};
        if (ip.find("nethergames") != std::string::npos)
            return {"Nethergames Network", "ngmc"};
        if (ip.find("cubecraft") != std::string::npos)
            return {"Cubecraft Network", "ccmc"};
        if (ip.find("zeqa") != std::string::npos)
            return {"Zeqa Network", "zeqamc"};
        if (ip == "realm")
            return {"Playing on a Realm", "flarialbig"};
        if (ip == "world")
            return {"Playing Singleplayer", "flarialbig"};
        if (ip == "none")
            return {"Ready to play", "flarialbig"};
        // Unknown server — still multiplayer
        return {"Playing Multiplayer", "flarialbig"};
    }

    void updatePresence(const std::string& ip) {
        if (!rpcInitialized) return;

        auto [name, imageKey] = getServerInfo(ip);

        auto& presence = discord::RPCManager::get().getPresence();
        presence.setDetails(name)
            .setLargeImageKey(imageKey)
            .setLargeImageText("Flarial Client")
            .setStartTimestamp(std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count())
            .setButton1("Website", "https://flarial.xyz", true)
            .setButton2("Discord Server", "https://flarial.xyz/discord", true);

        // Show current Hive gamemode as the second line (state field)
        const std::string gamemode = HiveModeCatcherListener::fullgamemodename;
        if (ip.find("hive") != std::string::npos && !gamemode.empty()) {
            presence.setState("Playing " + gamemode);
        } else {
            presence.setState("");
        }

        // Show Minecraft icon as small image when on a server/world
        if (ip != "none") {
            presence.setSmallImageKey("mcicon")
                .setSmallImageText("Minecraft Bedrock");
        }
        else {
            presence.setSmallImageKey("")
                .setSmallImageText("");
        }

        presence.refresh();
    }

    void handleServerIpChange() {
        std::lock_guard lk(rpcMutex);

        const std::string ip = SDK::getServerIP();
        const std::string gamemode = HiveModeCatcherListener::fullgamemodename;

        const bool ipChanged = ip != previousIp;
        const bool gamemodeChanged = gamemode != previousGamemode;

        if (ipChanged) {
            static std::string settingspath = Utils::getClientPath() + "\\serverip.txt";

            if (!std::filesystem::exists(settingspath)) {
                createSettingsFile(settingspath);
            }
            updateSettingsFile(settingspath, ip);

            // Update window title (non-UWP only)
            if (!PlatformUtils::isUWP()) {
                updateWindowTitle(ip);
            }
        }

        // Refresh Discord presence when IP or Hive gamemode changes
        if (ipChanged || gamemodeChanged) {
            updatePresence(ip);
            previousGamemode = gamemode;
        }
    }

    void updateWindowTitle(const std::string& ip) {
        auto [serverName, imageKey] = getServerInfo(ip);

        static auto [major, minor, build, error] = WinrtUtils::impl::getGameVersion();
        std::ostringstream oss;
        if (minor < 26) oss << major << "." << minor << "." << build;
        else oss << minor << "." << build;
        std::string ver = oss.str();

        std::string windowTitle;
        if (serverName == "Ready to play" || serverName == "Not Connected") {
            windowTitle = fmt::format("Flarial Client {} (#{})", ver, COMMIT_HASH);
        }
        else if (serverName == "Playing Singleplayer" || serverName == "Playing Multiplayer") {
            windowTitle = fmt::format("Flarial Client {} (#{}) - {}", ver, COMMIT_HASH, serverName);
        }
        else {
            windowTitle = fmt::format("Flarial Client {} (#{}) - Playing on {}", ver, COMMIT_HASH, serverName);
        }

        {
            std::lock_guard lk(g_pendingTitleMutex);
            g_pendingTitle = windowTitle;
        }

        ThreadingUtils::getInstance().queueInMainThread([windowTitle]() {
            const HWND root = GetAncestor(Client::window, GA_ROOTOWNER);
            const std::wstring wtitle = String::utf8_to_utf16(windowTitle);

            if (wtitle != g_lastAppliedTitle) {
                SetWindowTextW(root, wtitle.c_str());
                g_lastAppliedTitle = wtitle;
            }
        });
    }

    void createSettingsFile(const std::string& settingspath) {
        const std::filesystem::path filePath(settingspath);
        std::filesystem::create_directories(filePath.parent_path());

        const HANDLE fileHandle = CreateFileA(settingspath.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (fileHandle == INVALID_HANDLE_VALUE) {
            LOG_ERROR("Failed to create file: {}", settingspath);
            return;
        }

        CloseHandle(fileHandle);
    }

    void updateSettingsFile(const std::string& settingspath, const std::string& ip) {
        std::ofstream outputFile(settingspath);
        if (outputFile.is_open()) {
            outputFile << ip;
            previousIp = ip;
            outputFile.close();
        } else {
            LOG_ERROR("Failed to open file: {}", settingspath);
        }
    }
};
