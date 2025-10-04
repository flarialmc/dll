#pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <vector>
#include <algorithm>
#include <initializer_list>
#include <unordered_map>

#include "../../../../Assets/Assets.hpp"
#include "../../../../SDK/Client/Level/Level.hpp"

struct TabListEntry {
    bool valid = false;
    std::string clearedName;

    ImVec2 headPos;
    ImVec2 headPos2;
    ImVec2 headSize2D;
    ImVec2 headSize22D;

    ImVec2 pNameMetrics;

    D2D_RECT_F imageRect;

    float textX;
    float textY;

    float textShadowX;
    float textShadowY;

    float textShadowX2;
    float textShadowY2;

    float textWidth;

    float nfTextX;
    float nfTextY;
};

class TabList : public Module {
private:
    std::map<int, int> PlatformIcon = {
    {static_cast<int>(BuildPlatform::Google), IDR_ANDROID_LOGO_PNG},
    {static_cast<int>(BuildPlatform::IOS), IDR_APPLE_LOGO_PNG},
    {static_cast<int>(BuildPlatform::Uwp), IDR_WIN10_LOGO_PNG},
    {static_cast<int>(BuildPlatform::Sony), IDR_SONY_LOGO_PNG},
    {static_cast<int>(BuildPlatform::Nx), IDR_NX_LOGO_PNG},
    {static_cast<int>(BuildPlatform::Xbox), IDR_XBOX_LOGO_PNG},
    {static_cast<int>(BuildPlatform::Linux), IDR_CHROMEOS_LOGO_PNG}
    };
    std::map<std::string, int> roleLogos = {{"Dev", IDR_CYAN_LOGO_PNG}, {"Staff", IDR_WHITE_LOGO_PNG}, {"Gamer", IDR_GAMER_LOGO_PNG}, {"Booster", IDR_BOOSTER_LOGO_PNG}, {"Supporter", IDR_SUPPORTER_LOGO_PNG}, {"Default", IDR_RED_LOGO_PNG}};
    static inline std::unordered_map<mcUUID, PlayerListEntry> pmap_cache;
    static inline auto lastPlayerMapUpdate = std::chrono::steady_clock::now();

    bool refreshCache = true;

    bool isHiveDetcted = false;

    std::vector<float> columnx;
    std::vector<PlayerListEntry> vecmap;
    std::vector<TabListEntry> vectab;
    float totalWidth;
    float baseTotalHeight;
    size_t validPlayers;

    std::string cache_worldName;
    std::string cache_serverPing;
    std::string cache_playerCount;
    std::string cache_curPlayer;

    ImVec2 worldNameMetrics;
    ImVec2 curPingMetrics;
    ImVec2 playerCountMetrics;
    ImVec2 curPlayerMetrics;

    std::string curPlayer;
    std::string countTxt;

    float headDisplaySize;
    float headDisplaySize2;
    float diff;

    ImVec2 part1Metrics;
    float textX1;
    float textX2;
    float textX3;


public:

    TabList(): Module("Tab List", "Java-like tab list.\nLists the current online players on the server.",
        IDR_LIST_PNG, "TAB", false, {"player list"}) {
    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void onSetup() override;

    void settingsRender(float settingsOffset) override;

    static int getRolePriority(const std::string &name);

    static std::vector<PlayerListEntry> sortVecmap(const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap, bool flarialFirst, bool sort);

    int getPingImage(int ping);

    void onRender(RenderEvent &event);

    void onMouse(const MouseEvent &event);

    void onKey(const KeyEvent &event);

    // PlayerHead descriptor management functions
    static bool AllocatePlayerHeadDescriptor(const std::string& playerName, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle, UINT* out_descriptor_id);
    static void FreePlayerHeadDescriptor(UINT descriptorId);
    static void CleanupOldPlayerHeads(size_t maxCached = 50);

    // Reset all TabList descriptor state - called during swapchain recreation
    static void ResetDescriptorState();

    // Cleanup functions for resource management during resize
    static void CleanupDX12Uploader();
    static void CleanupPlayerHeadTextures();
    static void InitializeAsyncLoading();
    static void ReinitializeAfterResize();

    // PlayerHead descriptor management data
    struct PlayerHeadDescriptorInfo {
        std::string playerName;
        std::chrono::steady_clock::time_point lastUsed;
        bool inUse = false;
    };
    static inline std::unordered_map<UINT, PlayerHeadDescriptorInfo> playerHeadDescriptors;
    static inline std::queue<UINT> freePlayerHeadDescriptors;
    static inline UINT nextPlayerHeadDescriptorId;
    static inline std::mutex playerHeadDescriptorMutex;
    static constexpr UINT PLAYERHEAD_DESCRIPTOR_START = 10000;    // Start well beyond static images
    static constexpr UINT MAX_PLAYERHEAD_DESCRIPTORS = 2000;     // Support 2000 concurrent playerheads
    static constexpr UINT PLAYERHEAD_DESCRIPTOR_END = PLAYERHEAD_DESCRIPTOR_START + MAX_PLAYERHEAD_DESCRIPTORS;

};
