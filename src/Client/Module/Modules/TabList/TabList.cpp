#include "TabList.hpp"

#include <wrl/client.h>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>
#include <thread>
#include <future>
#include <queue>
#include <condition_variable>
#include <winrt/base.h>

#include "Command/Commands/SkinStealCommand.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Modules/Nick/NickModule.hpp"

bool logDebug = false;

TabList::TabList(): Module("Tab List", "Java-like tab list.\nLists the current online players on the server.", IDR_LIST_PNG, "TAB") {
}

// DX12 texture creation from raw bytes
enum class PlayerHeadLoadState {
    NotLoaded,
    Loading,
    Loaded,
    Failed
};

struct PlayerHeadTexture {
    winrt::com_ptr<ID3D12Resource> texture;
    D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;
    UINT descriptorId = UINT_MAX;
    std::chrono::steady_clock::time_point lastUsed;
    std::atomic<PlayerHeadLoadState> loadState{PlayerHeadLoadState::NotLoaded};
    bool valid = false;
};

struct PlayerHeadTextureDX11 {
    winrt::com_ptr<ID3D11ShaderResourceView> srv;
    std::atomic<PlayerHeadLoadState> loadState{PlayerHeadLoadState::NotLoaded};
    bool valid = false;
};

static std::unordered_map<std::string, PlayerHeadTextureDX11> g_playerHeadTexturesDX11;
static std::unordered_map<std::string, PlayerHeadTexture> g_playerHeadTextures;
static std::mutex g_playerHeadMutex;

// Async loading infrastructure
struct PlayerHeadLoadTask {
    std::string playerName;
    std::vector<unsigned char> imageData;
    int width;
    int height;
    bool isDX12;
};

struct PlayerHeadReadyTexture {
    std::string playerName;
    std::vector<unsigned char> processedData;
    int width;
    int height;
    bool isDX12;
};

static std::queue<PlayerHeadLoadTask> g_loadQueue;
static std::mutex g_loadQueueMutex;
static std::condition_variable g_loadQueueCV;
static std::queue<PlayerHeadReadyTexture> g_readyQueue; // Textures ready for main thread processing
static std::mutex g_readyQueueMutex;
static std::atomic<bool> g_shouldStopLoading{false};
static std::vector<std::thread> g_loaderThreads;
static constexpr int NUM_LOADER_THREADS = 3; // Use 3 threads for loading


// Dedicated DX12 Uploader Infrastructure
static winrt::com_ptr<ID3D12CommandQueue> g_uploadQueueDX12;
static winrt::com_ptr<ID3D12CommandAllocator> g_uploadAllocatorDX12;
static winrt::com_ptr<ID3D12GraphicsCommandList> g_uploadCmdListDX12;
static winrt::com_ptr<ID3D12Fence> g_uploadFenceDX12;
static UINT64 g_uploadFenceValue = 0;
static HANDLE g_uploadFenceEvent = nullptr;

void InitializeDX12Uploader() {
    if (g_uploadQueueDX12 || !SwapchainHook::d3d12Device5) {
        return;
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 1;
    if (FAILED(SwapchainHook::d3d12Device5->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(g_uploadQueueDX12.put())))) {
        if (logDebug) Logger::debug("Failed to create DX12 upload queue");
        return;
    }

    if (FAILED(SwapchainHook::d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(g_uploadAllocatorDX12.put())))) {
        if (logDebug) Logger::debug("Failed to create DX12 upload allocator");
        g_uploadQueueDX12 = nullptr;
        return;
    }

    if (FAILED(SwapchainHook::d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_uploadAllocatorDX12.get(), nullptr, IID_PPV_ARGS(g_uploadCmdListDX12.put())))) {
        if (logDebug) Logger::debug("Failed to create DX12 upload command list");
        g_uploadAllocatorDX12 = nullptr;
        g_uploadQueueDX12 = nullptr;
        return;
    }
    g_uploadCmdListDX12->Close();

    if (FAILED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(g_uploadFenceDX12.put())))) {
        if (logDebug) Logger::debug("Failed to create DX12 upload fence");
        g_uploadCmdListDX12 = nullptr;
        g_uploadAllocatorDX12 = nullptr;
        g_uploadQueueDX12 = nullptr;
        return;
    }
    g_uploadFenceValue = 0;

    g_uploadFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_uploadFenceEvent) {
        if (logDebug) Logger::debug("Failed to create DX12 upload fence event");
        g_uploadFenceDX12 = nullptr;
        g_uploadCmdListDX12 = nullptr;
        g_uploadAllocatorDX12 = nullptr;
        g_uploadQueueDX12 = nullptr;
        return;
    }

    if (logDebug) Logger::debug("Initialized DX12 Uploader");
}

void CleanupDX12Uploader() {
    if (g_uploadFenceEvent) {
        CloseHandle(g_uploadFenceEvent);
        g_uploadFenceEvent = nullptr;
    }
    g_uploadFenceDX12 = nullptr;
    g_uploadCmdListDX12 = nullptr;
    g_uploadAllocatorDX12 = nullptr;
    g_uploadQueueDX12 = nullptr;
    if (logDebug) Logger::debug("Cleaned up DX12 Uploader");
}

// END Dedicated DX12 Uploader Infrastructure


// Forward declarations for sync functions
PlayerHeadTexture *CreateTextureFromBytesDX12Sync(const std::string &playerName, const unsigned char *data, int width, int height);

ID3D11ShaderResourceView *CreateTextureFromBytesDX11Sync(const std::string &playerName, const unsigned char *data, int width, int height);

// Custom rendering function for crisp pixel art - DX12
void RenderPlayerHeadWithPointFiltering(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, D2D1_RECT_F rect, const std::string &debugName) {
    ImDrawList *drawList = ImGui::GetForegroundDrawList();
    if (drawList) {
        // Convert D2D1_RECT_F to ImVec2
        ImVec2 topLeft(rect.left, rect.top);
        ImVec2 bottomRight(rect.right, rect.bottom);

        // Render with higher resolution texture (64x64 instead of 32x32)
        // This reduces the blur effect from linear filtering
        drawList->AddImage(
            (ImTextureID) srvHandle.ptr,
            topLeft,
            bottomRight,
            ImVec2(0, 0), ImVec2(1, 1),
            IM_COL32_WHITE
        );

        if (logDebug) Logger::debug("Rendered DX12 playerhead {} with improved crispness", debugName);
    }
}

// Custom rendering function for crisp pixel art - DX11
void RenderPlayerHeadWithPointFilteringDX11(ID3D11ShaderResourceView *srv, D2D1_RECT_F rect, const std::string &debugName) {
    ImDrawList *drawList = ImGui::GetForegroundDrawList();
    if (drawList) {
        // Convert D2D1_RECT_F to ImVec2
        ImVec2 topLeft(rect.left, rect.top);
        ImVec2 bottomRight(rect.right, rect.bottom);

        // Render with higher resolution texture (64x64 instead of 32x32)
        // This reduces the blur effect from linear filtering
        drawList->AddImage(
            reinterpret_cast<ImTextureID>(srv),
            topLeft,
            bottomRight,
            ImVec2(0, 0), ImVec2(1, 1),
            IM_COL32_WHITE
        );

        if (logDebug) Logger::debug("Rendered DX11 playerhead {} with improved crispness", debugName);
    }
}

// Stop async loading threads
void StopAsyncLoading() {
    g_shouldStopLoading = true;
    g_loadQueueCV.notify_all();

    for (auto &thread: g_loaderThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    g_loaderThreads.clear();

    // Clear remaining queues
    {
        std::lock_guard<std::mutex> lock(g_loadQueueMutex);
        while (!g_loadQueue.empty()) {
            g_loadQueue.pop();
        }
    } {
        std::lock_guard<std::mutex> lock(g_readyQueueMutex);
        while (!g_readyQueue.empty()) {
            g_readyQueue.pop();
        }
    }
}

// Cleanup function for player head textures
void CleanupPlayerHeadTextures() {
    if (logDebug) Logger::debug("Cleaning up player head textures - DX12: {}, DX11: {}", g_playerHeadTextures.size(), g_playerHeadTexturesDX11.size());

    // Stop async loading first
    StopAsyncLoading();

    CleanupDX12Uploader();

    // Clear DX12 textures and free descriptors
    {
        std::lock_guard<std::mutex> lock(g_playerHeadMutex);
        for (auto &pair: g_playerHeadTextures) {
            if (pair.second.valid && pair.second.descriptorId != UINT_MAX) {
                SwapchainHook::FreePlayerHeadDescriptor(pair.second.descriptorId);
            }
        }
        g_playerHeadTextures.clear();
    }

    // Clear DX11 textures
    g_playerHeadTexturesDX11.clear();

    // Reset the playerhead descriptor system
    SwapchainHook::ResetPlayerHeadDescriptors();
}

// Worker thread function for processing image data (no D3D operations)
void PlayerHeadLoaderWorker() {
    while (!g_shouldStopLoading) {
        PlayerHeadLoadTask task;

        // Wait for a task
        {
            std::unique_lock<std::mutex> lock(g_loadQueueMutex);
            g_loadQueueCV.wait(lock, [] { return !g_loadQueue.empty() || g_shouldStopLoading; });

            if (g_shouldStopLoading) break;

            if (g_loadQueue.empty()) continue;

            if (!g_loadQueue.front().imageData.size()) {
                g_loadQueue.pop();
                continue;
            }

            task = std::move(g_loadQueue.front());
            g_loadQueue.pop();
        }

        // Process the image data (CPU work only, no D3D operations)
        // The data is already processed, so we just move it to the ready queue
        {
            std::lock_guard<std::mutex> readyLock(g_readyQueueMutex);
            g_readyQueue.push({
                task.playerName,
                std::move(task.imageData),
                task.width,
                task.height,
                task.isDX12
            });
        }

        if (logDebug) Logger::debug("Background thread processed playerhead data for {}", task.playerName);
    }
}

// Initialize async loading system
void InitializeAsyncLoading() {
    if (!g_loaderThreads.empty()) return; // Already initialized

    g_shouldStopLoading = false;
    g_loaderThreads.reserve(NUM_LOADER_THREADS);

    for (int i = 0; i < NUM_LOADER_THREADS; ++i) {
        g_loaderThreads.emplace_back(PlayerHeadLoaderWorker);
    }

    if (logDebug) Logger::debug("Initialized {} playerhead loader threads", NUM_LOADER_THREADS);
}

// Process ready textures on main thread (called each frame)
void ProcessReadyPlayerHeadTextures() {
    if (SwapchainHook::d3d12Device5 && !g_uploadQueueDX12) {
        InitializeDX12Uploader();
    }

    // Skip processing if we're already running slow (adaptive throttling)
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    // More aggressive loading - only skip if frame time is really bad (40fps or worse)
    if (frameDuration.count() > 25000) {
        return;
    }

    // Process multiple textures per frame based on performance
    int maxPerFrame;
    if (frameDuration.count() < 10000) {
        maxPerFrame = 3; // Very fast frame, process 3
    } else if (frameDuration.count() < 16670) {
        maxPerFrame = 2; // Good frame, process 2
    } else {
        maxPerFrame = 1; // Slower frame, process 1
    }

    int processed = 0;
    while (processed < maxPerFrame) {
        PlayerHeadReadyTexture ready;

        // Get next ready texture
        {
            std::lock_guard<std::mutex> lock(g_readyQueueMutex);
            if (g_readyQueue.empty()) break;

            ready = std::move(g_readyQueue.front());
            g_readyQueue.pop();
        }

        // Create the actual D3D texture on main thread
        if (ready.isDX12) {
            CreateTextureFromBytesDX12Sync(ready.playerName, ready.processedData.data(), ready.width, ready.height);
        } else {
            CreateTextureFromBytesDX11Sync(ready.playerName, ready.processedData.data(), ready.width, ready.height);
        }

        processed++;
        if (logDebug) Logger::debug("Main thread created D3D texture for player {} ({}/{})", ready.playerName, processed, maxPerFrame);
    }
}

// Periodic cleanup to free unused descriptors (updated to use new system)
void CleanupUnusedPlayerHeadDescriptors() {
    // The new system automatically handles cleanup in SwapchainHook::CleanupOldPlayerHeads
    // We call it with a reasonable limit to keep memory usage under control
    SwapchainHook::CleanupOldPlayerHeads(500); // Keep at most 500 cached playerheads
}

void TabList::onEnable() {
    Listen(this, RenderEvent, &TabList::onRender)
    Listen(this, KeyEvent, &TabList::onKey)
    Listen(this, MouseEvent, &TabList::onMouse)
    InitializeAsyncLoading();
    // START OF CHANGE: Initialize DX12 Uploader
    InitializeDX12Uploader();
    // END OF CHANGE
    Module::onEnable();
}

void TabList::onDisable() {
    Deafen(this, RenderEvent, &TabList::onRender)
    Deafen(this, KeyEvent, &TabList::onKey)
    Deafen(this, MouseEvent, &TabList::onMouse)
    //CleanupPlayerHeadTextures();
    CleanupPlayerHeadTextures();
    Module::onDisable();
}

void TabList::defaultConfig() {
    this->settings.changeType<float, int>("maxColumn");
    setDef("enabled", true);
    setDef("uiscale", 0.65f);
    setDef("playerCount", true);
    setDef("serverPing", true);
    setDef("worldName", true);
    setDef("maxColumn", 10);
    setDef("togglable", false);
    setDef("showHeads", true);
    setDef("textalignment", (std::string) "Left");
    setDef("textShadow", false);
    setDef("textShadowOffset", 0.003f);
    getKeybind();
    Module::defaultConfig("core");
    Module::defaultConfig("pos");
    Module::defaultConfig("main");
    Module::defaultConfig("colors");
    setDef("alphaOrder", true);
    setDef("flarialFirst", true);
}

void TabList::onSetup() {
    keybindActions.clear();

    keybindActions.push_back([this](std::vector<std::any> args)-> std::any {
        if (SDK::getCurrentScreen() != "hud_screen" &&
            SDK::getCurrentScreen() != "zoom_screen" &&
            SDK::getCurrentScreen() != "f3_screen" && this->name != "ClickGUI"
        )
            return {};
        this->active = !this->active;
        refreshCache = true;
        return {};
    });
}

void TabList::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"), Constraints::RelativeConstraint(1.0, "width"), Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Tab List");
    defaultAddSettings("main");
    extraPadding();

    addHeader("Text");
    addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center"}, "textalignment", true);
    addToggle("Text Shadow", "Displays a shadow under the text", "textShadow");
    addConditionalSlider(getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);
    extraPadding();

    addHeader("Misc");
    addToggle("Togglable", "", "togglable");
    addToggle("Show Player Heads", "", "showHeads");
    addToggle("Player Count", "", "playerCount");
    addSliderInt("Max Players per Column", "", "maxColumn", 30, 1);
    addToggle("World Name", "", "worldName");
    addToggle("Server Ping", "", "serverPing");
    addToggle("Alphabetical Order", "", "alphaOrder");
    addToggle("Flarial First", "Prioritize Flarial users (Dev > Staff > Gamer > Supporter > Booster > Default) at the top", "flarialFirst");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
    extraPadding();

    addHeader("Colors");
    addConditionalColorPicker(getOps<bool>("showBg"), "Background Color", "", "bg");
    addConditionalColorPicker(getOps<bool>("rectShadow"), "Background Shadow Color", "", "rectShadow");
    addColorPicker("Text Color", "", "text");
    addConditionalColorPicker(getOps<bool>("textShadow"), "Text Shadow Color", "", "textShadow");
    addConditionalColorPicker(getOps<bool>("border"), "Border Color", "", "border");
    addConditionalColorPicker(getOps<bool>("glow"), "Glow Color", "", "glow");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

int TabList::getRolePriority(const std::string &name) {
    std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
    if (clearedName.empty()) return 5; // Lowest priority for invalid names

    auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
    if (it == APIUtils::onlineUsers.end()) return 5; // Non-Flarial users

    // Check roles in order of priority using ApiUtils
    if (APIUtils::hasRole("Dev", clearedName)) return 0;
    if (APIUtils::hasRole("Staff", clearedName)) return 1;
    if (APIUtils::hasRole("Gamer", clearedName)) return 2;
    if (APIUtils::hasRole("Supporter", clearedName)) return 3;
    if (APIUtils::hasRole("Booster", clearedName)) return 4;
    return 5; // Default Flarial user (in onlineUsers but no specific role)
}

std::vector<PlayerListEntry> TabList::sortVecmap(
    const std::unordered_map<mcUUID, PlayerListEntry> &sourceMap, bool flarialFirst, bool sort) {
    if (flarialFirst) {
        std::vector<PlayerListEntry> flarialEntries;
        std::vector<PlayerListEntry> nonFlarialEntries;

        for (const auto &pair: sourceMap) {
            const PlayerListEntry &entry = pair.second;
            if (entry.name.empty()) continue;

            std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(entry.name));
            if (clearedName.empty()) clearedName = entry.name;

            auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
            if (it != APIUtils::onlineUsers.end()) {
                flarialEntries.push_back(entry);
            } else {
                nonFlarialEntries.push_back(entry);
            }
        }

        if (sort) {
            auto sortByName = [](const PlayerListEntry &a, const PlayerListEntry &b) {
                std::string clean_a = String::removeColorCodes(a.name);
                std::string clean_b = String::removeColorCodes(b.name);
                return std::lexicographical_compare(clean_a.begin(), clean_a.end(), clean_b.begin(), clean_b.end(),
                                                    [](char c1, char c2) { return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2)); });
            };

            auto sortByRoleAndName = [&sortByName](const PlayerListEntry &a, const PlayerListEntry &b) {
                int priorityA = getRolePriority(a.name);
                int priorityB = getRolePriority(b.name);
                if (priorityA != priorityB) return priorityA < priorityB;
                return sortByName(a, b);
            };

            std::sort(flarialEntries.begin(), flarialEntries.end(), sortByRoleAndName);
            std::sort(nonFlarialEntries.begin(), nonFlarialEntries.end(), sortByName);
        }

        std::vector<PlayerListEntry> result;
        result.reserve(sourceMap.size());
        result.insert(result.end(), flarialEntries.begin(), flarialEntries.end());
        result.insert(result.end(), nonFlarialEntries.begin(), nonFlarialEntries.end());
        return result;
    } else {
        std::vector<PlayerListEntry> result;
        result.reserve(sourceMap.size());
        for (const auto &pair: sourceMap) {
            if (!pair.second.name.empty()) {
                result.push_back(pair.second);
            }
        }

        // Conditionally sort the single list.
        if (sort) {
            auto sortByName = [](const PlayerListEntry &a, const PlayerListEntry &b) {
                std::string clean_a = String::removeColorCodes(a.name);
                std::string clean_b = String::removeColorCodes(b.name);
                return std::lexicographical_compare(clean_a.begin(), clean_a.end(), clean_b.begin(), clean_b.end(),
                                                    [](char c1, char c2) { return std::tolower(static_cast<unsigned char>(c1)) < std::tolower(static_cast<unsigned char>(c2)); });
            };
            std::sort(result.begin(), result.end(), sortByName);
        }

        return result;
    }
}

// DX11 texture creation from raw bytes
// Synchronous DX11 texture creation (called from worker threads)
ID3D11ShaderResourceView *CreateTextureFromBytesDX11Sync(
    const std::string &playerName,
    const unsigned char *data,
    int width,
    int height) {
    if (!SwapchainHook::d3d11Device || !data || width <= 0 || height <= 0) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_playerHeadMutex);

    // Check if texture already exists
    auto it = g_playerHeadTexturesDX11.find(playerName);
    if (it != g_playerHeadTexturesDX11.end() && it->second.valid) {
        return it->second.srv.get();
    }

    PlayerHeadTextureDX11 &playerTex = g_playerHeadTexturesDX11[playerName];
    playerTex.loadState = PlayerHeadLoadState::Loading;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = width * 4;
    initData.SysMemSlicePitch = 0;

    winrt::com_ptr<ID3D11Texture2D> texture;
    HRESULT hr = SwapchainHook::d3d11Device->CreateTexture2D(&desc, &initData, texture.put());
    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to create DX11 texture: 0x{:x}", hr);
        playerTex.loadState = PlayerHeadLoadState::Failed;
        return nullptr;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    hr = SwapchainHook::d3d11Device->CreateShaderResourceView(texture.get(), &srvDesc, playerTex.srv.put());
    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to create DX11 SRV: 0x{:x}", hr);
        playerTex.loadState = PlayerHeadLoadState::Failed;
        return nullptr;
    }

    playerTex.valid = true;
    playerTex.loadState = PlayerHeadLoadState::Loaded;
    if (logDebug) Logger::debug("Successfully created DX11 player head texture for {}", playerName);
    return playerTex.srv.get();
}

PlayerHeadTexture *CreateTextureFromBytesDX12Sync(
    const std::string &playerName,
    const unsigned char *data,
    int width,
    int height) {
    // Check minimal requirements for DX12 and our uploader
    if (!SwapchainHook::d3d12Device5 || !SwapchainHook::d3d12DescriptorHeapImGuiRender || !data || width <= 0 || height <= 0 || !g_uploadQueueDX12) {
        if (logDebug)
            Logger::debug("DX12 minimal requirements not met - device: {}, heap: {}, data: {}, size: {}x{}, uploader: {}",
                          SwapchainHook::d3d12Device5 ? "valid" : "null",
                          SwapchainHook::d3d12DescriptorHeapImGuiRender ? "valid" : "null",
                          data ? "valid" : "null", width, height,
                          g_uploadQueueDX12 ? "valid" : "null");
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_playerHeadMutex);

    // Check if texture already exists and update its last used time
    auto it = g_playerHeadTextures.find(playerName);
    if (it != g_playerHeadTextures.end() && it->second.valid) {
        it->second.lastUsed = std::chrono::steady_clock::now();
        if (logDebug) Logger::debug("Reusing existing texture for player {}", playerName);
        return &it->second;
    }

    PlayerHeadTexture &playerTex = g_playerHeadTextures[playerName];

    // Mark as loading
    playerTex.loadState = PlayerHeadLoadState::Loading;

    // Allocate descriptor using new system
    if (!SwapchainHook::AllocatePlayerHeadDescriptor(playerName,
                                                     &playerTex.srvCpuHandle,
                                                     &playerTex.srvGpuHandle,
                                                     &playerTex.descriptorId)) {
        if (logDebug) Logger::debug("Failed to allocate DX12 playerhead descriptor for {}", playerName);
        playerTex.loadState = PlayerHeadLoadState::Failed;
        return nullptr;
    }

    // Create texture resource
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    HRESULT hr = SwapchainHook::d3d12Device5->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
        IID_PPV_ARGS(playerTex.texture.put()));

    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to create DX12 texture: 0x{:x}", hr);
        playerTex.loadState = PlayerHeadLoadState::Failed;
        return nullptr;
    }

    // Create upload buffer
    UINT uploadPitch = (width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    UINT uploadSize = height * uploadPitch;

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC uploadBufferDesc = {};
    uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadBufferDesc.Width = uploadSize;
    uploadBufferDesc.Height = 1;
    uploadBufferDesc.DepthOrArraySize = 1;
    uploadBufferDesc.MipLevels = 1;
    uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    uploadBufferDesc.SampleDesc.Count = 1;
    uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    winrt::com_ptr<ID3D12Resource> uploadBuffer;
    hr = SwapchainHook::d3d12Device5->CreateCommittedResource(
        &uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(uploadBuffer.put()));

    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to create DX12 upload buffer: 0x{:x}", hr);
        return nullptr;
    }

    // Map and copy data
    void *mapped = nullptr;
    D3D12_RANGE range = {0, uploadSize};
    hr = uploadBuffer->Map(0, &range, &mapped);
    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to map DX12 upload buffer: 0x{:x}", hr);
        return nullptr;
    }

    for (int y = 0; y < height; ++y) {
        memcpy(
            static_cast<unsigned char *>(mapped) + y * uploadPitch,
            data + y * width * 4,
            width * 4
        );
    }
    uploadBuffer->Unmap(0, &range);

    // Use the dedicated uploader to perform the copy
    hr = g_uploadAllocatorDX12->Reset();
    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to reset upload allocator: 0x{:x}", hr);
        return nullptr;
    }

    hr = g_uploadCmdListDX12->Reset(g_uploadAllocatorDX12.get(), nullptr);
    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to reset command list: 0x{:x}", hr);
        return nullptr;
    }

    // Copy texture
    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = uploadBuffer.get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint.Offset = 0;
    srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srcLocation.PlacedFootprint.Footprint.Width = width;
    srcLocation.PlacedFootprint.Footprint.Height = height;
    srcLocation.PlacedFootprint.Footprint.Depth = 1;
    srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = playerTex.texture.get();
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = 0;

    g_uploadCmdListDX12->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

    // Transition to shader resource
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = playerTex.texture.get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    g_uploadCmdListDX12->ResourceBarrier(1, &barrier);
    g_uploadCmdListDX12->Close();

    // Execute and wait for completion
    ID3D12CommandList *cmdLists[] = {g_uploadCmdListDX12.get()};
    g_uploadQueueDX12->ExecuteCommandLists(1, cmdLists);

    // Signal and wait using the global fence
    const UINT64 fenceValue = ++g_uploadFenceValue;
    hr = g_uploadQueueDX12->Signal(g_uploadFenceDX12.get(), fenceValue);
    if (FAILED(hr)) {
        if (logDebug) Logger::debug("Failed to signal fence: 0x{:x}", hr);
        return nullptr;
    }

    if (g_uploadFenceDX12->GetCompletedValue() < fenceValue) {
        hr = g_uploadFenceDX12->SetEventOnCompletion(fenceValue, g_uploadFenceEvent);
        if (FAILED(hr)) {
            if (logDebug) Logger::debug("Failed to set fence event: 0x{:x}", hr);
            return nullptr;
        }
        DWORD waitResult = WaitForSingleObject(g_uploadFenceEvent, 5000); // 5 second timeout
        if (waitResult != WAIT_OBJECT_0) {
            if (logDebug) Logger::debug("Fence wait timed out or failed: {}", waitResult);
            playerTex.loadState = PlayerHeadLoadState::Failed;
            return nullptr;
        }
    }

    if (logDebug) Logger::debug("DX12 texture upload completed successfully for {}", playerName);

    // Create SRV
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    SwapchainHook::d3d12Device5->CreateShaderResourceView(playerTex.texture.get(), &srvDesc, playerTex.srvCpuHandle);

    playerTex.valid = true;
    playerTex.lastUsed = std::chrono::steady_clock::now();
    playerTex.loadState = PlayerHeadLoadState::Loaded;
    if (logDebug) Logger::debug("Successfully created DX12 player head texture for {}", playerName);
    return &playerTex;
}

// Async DX12 texture creation (queues loading, returns immediately)
PlayerHeadTexture *CreateTextureFromBytesDX12(
    const std::string &playerName,
    const unsigned char *data,
    int width,
    int height) {
    std::lock_guard<std::mutex> lock(g_playerHeadMutex);

    // Check if texture already exists or is loading
    auto it = g_playerHeadTextures.find(playerName);
    if (it != g_playerHeadTextures.end()) {
        if (it->second.valid && it->second.loadState == PlayerHeadLoadState::Loaded) {
            it->second.lastUsed = std::chrono::steady_clock::now();
            return &it->second;
        }
        if (it->second.loadState == PlayerHeadLoadState::Loading) {
            return &it->second; // Already loading
        }
    }

    // Create entry and mark as loading
    PlayerHeadTexture &playerTex = g_playerHeadTextures[playerName];
    playerTex.loadState = PlayerHeadLoadState::Loading;
    playerTex.lastUsed = std::chrono::steady_clock::now();

    // Queue the loading task
    {
        std::lock_guard<std::mutex> queueLock(g_loadQueueMutex);
        g_loadQueue.push({
            playerName,
            std::vector<unsigned char>(data, data + (width * height * 4)),
            width,
            height,
            true // isDX12
        });
    }
    g_loadQueueCV.notify_one();

    return &playerTex;
}

// Async DX11 texture creation (queues loading, returns immediately)
ID3D11ShaderResourceView *CreateTextureFromBytesDX11(
    const std::string &playerName,
    const unsigned char *data,
    int width,
    int height) {
    std::lock_guard<std::mutex> lock(g_playerHeadMutex);

    // Check if texture already exists or is loading
    auto it = g_playerHeadTexturesDX11.find(playerName);
    if (it != g_playerHeadTexturesDX11.end()) {
        if (it->second.valid && it->second.loadState == PlayerHeadLoadState::Loaded) {
            return it->second.srv.get();
        }
        if (it->second.loadState == PlayerHeadLoadState::Loading) {
            return nullptr; // Still loading, return null for now
        }
    }

    // Create entry and mark as loading
    PlayerHeadTextureDX11 &playerTex = g_playerHeadTexturesDX11[playerName];
    playerTex.loadState = PlayerHeadLoadState::Loading;

    // Queue the loading task
    {
        std::lock_guard<std::mutex> queueLock(g_loadQueueMutex);
        g_loadQueue.push({
            playerName,
            std::vector<unsigned char>(data, data + (width * height * 4)),
            width,
            height,
            false // isDX12
        });
    }
    g_loadQueueCV.notify_one();

    return nullptr; // Will be available next frame
}

int TabList::getPingImage(int ping) {
    if (ping > 300) return IDR_PING_QUIT;
    else if (ping > 250) return IDR_PING_BAD;
    else if (ping > 175) return IDR_PING_OKAY;
    else if (ping > 99) return IDR_PING_GOOD;
    else return IDR_PING_AMAZING;
}

void TabList::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    if (SDK::hasInstanced && (active || ClickGUI::editmenu)) {
        // Process ready textures from background threads (adaptive rate based on performance)
        ProcessReadyPlayerHeadTextures();

        // Periodically clean up unused descriptors (every ~5 seconds at 60fps)
        static int cleanupCounter = 0;
        if (++cleanupCounter > 300) {
            cleanupCounter = 0;
            CleanupUnusedPlayerHeadDescriptors();
            if (logDebug)
                Logger::debug("PlayerHead usage: {} DX12 textures, {} DX11 textures",
                              g_playerHeadTextures.size(), g_playerHeadTexturesDX11.size());
        }
        if (SDK::clientInstance->getLocalPlayer() != nullptr) {
            float keycardSize = Constraints::RelativeConstraint(0.05f * getOps<float>("uiscale"), "height", true);

            Vec2<float> settingperc{0.0, 0.0};

            std::string textAlignment = getOps<std::string>("textalignment");
            bool showHeads = getOps<bool>("showHeads");
            bool alphaOrder = getOps<bool>("alphaOrder");
            bool flarialFirst = getOps<bool>("flarialFirst");

            // if (SwapchainHook::queue != nullptr) showHeads = false;
            auto module = ModuleManager::getModule("Nick");

            int maxColumn = floor(getOps<int>("maxColumn"));

            float fontSize = Constraints::SpacingConstraint(3, keycardSize);

            float totalHeight = 0;

            // do some caching bullshit
            // getPlayerMap() is expensive to run so we only check for updates every second

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastPlayerMapUpdate).count() >= 1) {
                pmap_cache = SDK::clientInstance->getLocalPlayer()->getLevel()->getPlayerMap();
                lastPlayerMapUpdate = now;
            }

            if (vecmap.size() != pmap_cache.size() || refreshCache) {
                totalHeight = 0;
                columnx.clear();
                refreshCache = true;
                vecmap = sortVecmap(pmap_cache, flarialFirst, alphaOrder);

                totalWidth = keycardSize * (showHeads ? 1 : 0.4);

                float curMax = 0;

                validPlayers = 0;

                for (size_t i = 0; i < vecmap.size(); i++) {
                    if (vecmap[i].name.empty()) continue;

                    std::string name = String::removeColorCodes(vecmap[i].name);
                    if (name.empty()) continue;

                    std::string clearedName = String::removeNonAlphanumeric(name);
                    if (clearedName.empty()) clearedName = name;

                    if (module && module->isEnabled() && !NickModule::backupOri.empty() && clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
                        name = module->getOps<std::string>("nick");
                        if (name.empty()) name = clearedName;
                    }

                    auto textMetric = FlarialGUI::getFlarialTextSize(String::StrToWStr(name).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);

                    auto it = std::ranges::find(APIUtils::onlineUsers, clearedName);
                    if (it != APIUtils::onlineUsers.end()) textMetric.x += Constraints::SpacingConstraint(0.6, keycardSize);

                    if (textMetric.x > curMax) curMax = textMetric.x;
                    if (i < maxColumn) totalHeight += keycardSize * 0.7f;

                    validPlayers++;

                    if ((i + 1) % maxColumn == 0 || i == vecmap.size() - 1) {
                        totalWidth += curMax + keycardSize * (showHeads ? 1 : 0.4);
                        columnx.push_back(curMax + keycardSize * (showHeads ? 1 : 0.4f));
                        curMax = 0;
                    }
                }

                baseTotalHeight = totalHeight;
            } else {
                totalHeight = baseTotalHeight;
            }

            if (getOps<bool>("worldName")) {
                std::string worldName = SDK::clientInstance->getLocalPlayer()->getLevel()->getLevelData()->getLevelName();
                if (worldName != cache_worldName || refreshCache) {
                    worldNameMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(worldName).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    cache_worldName = worldName;
                }
                if (totalWidth < worldNameMetrics.x + keycardSize) totalWidth = worldNameMetrics.x + keycardSize;

                totalHeight += keycardSize * 0.75f;
            }

            if (getOps<bool>("serverPing")) {
                std::string serverPing = "Ping:__" + std::format("{}ms", SDK::getServerPing());
                if (serverPing != cache_serverPing || refreshCache) {
                    curPingMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(serverPing).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    curPingMetrics.x += Constraints::SpacingConstraint(0.5, keycardSize);
                    cache_serverPing = serverPing;
                }
                if (totalWidth < curPingMetrics.x + keycardSize) totalWidth = curPingMetrics.x + keycardSize;

                totalHeight += keycardSize * 0.5f;
            }

            totalHeight += keycardSize * 0.5f;

            if (getOps<bool>("playerCount")) {
                std::string playerCount = std::to_string(validPlayers) + " player(s) online";
                if (playerCount != cache_playerCount || refreshCache) {
                    playerCountMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(playerCount).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    cache_playerCount = playerCount;
                }

                std::string _curPlayer = String::removeNonAlphanumeric(String::removeColorCodes(module && module->isEnabled() && !NickModule::backupOri.empty() ? module->getOps<std::string>("nick") : SDK::clientInstance->getLocalPlayer()->getPlayerName()));
                std::string curPlayer = "Player:__" + _curPlayer;
                if (_curPlayer != cache_curPlayer || refreshCache) {
                    curPlayerMetrics = FlarialGUI::getFlarialTextSize(FlarialGUI::to_wide(curPlayer).c_str(), keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    curPlayerMetrics.x += Constraints::SpacingConstraint(0.5, keycardSize);
                    cache_curPlayer = _curPlayer;
                }

                if (totalWidth < playerCountMetrics.x + keycardSize) totalWidth = playerCountMetrics.x + keycardSize;
                if (totalWidth < curPlayerMetrics.x + keycardSize) totalWidth = curPlayerMetrics.x + keycardSize;

                totalHeight += keycardSize * 2.f;
            }
            else totalHeight += keycardSize * 0.5f;

            Vec2<float> realcenter;
            if (settingperc.x != 0 || settingperc.y != 0) realcenter = Vec2<float>(settingperc.x * MC::windowSize.x, settingperc.y * MC::windowSize.y);
            else realcenter = Constraints::CenterConstraint(totalWidth, totalHeight, "y", 0.0f, -0.85f);

            float fakex = realcenter.x;
            Vec2<float> vec2 = realcenter;

            Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding") * getOps<float>("uiscale"), getOps<float>("rounding") * getOps<float>("uiscale"));

            D2D1_COLOR_F disabledColor = getColor("bg");
            D2D1_COLOR_F textColor = getColor("text");
            D2D1_COLOR_F borderColor = getColor("border");

            if (getOps<bool>("glow")) FlarialGUI::ShadowRect(Vec2<float>(fakex, realcenter.y), Vec2<float>(totalWidth, totalHeight), getColor("glow"), rounde.x, (getOps<float>("glowAmount") / 100.f) * Constraints::PercentageConstraint(0.1f, "top"));
            if (getOps<bool>("BlurEffect")) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(fakex, realcenter.y, fakex + totalWidth, realcenter.y + totalHeight), rounde.x, rounde.x));
            if (getOps<bool>("border")) FlarialGUI::RoundedHollowRect(fakex, realcenter.y, Constraints::RelativeConstraint((getOps<float>("borderWidth") * getOps<float>("uiscale")) / 100.0f, "height", true), borderColor, totalWidth, totalHeight, rounde.x, rounde.x);
            if (getOps<bool>("showBg")) {
                if (getOps<bool>("rectShadow")) FlarialGUI::RoundedRect(fakex + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"), realcenter.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset")) * getOps<float>("uiscale"), getColor("rectShadow"), totalWidth, totalHeight, rounde.x, rounde.x);
                FlarialGUI::RoundedRect(fakex, realcenter.y, disabledColor, totalWidth, totalHeight, rounde.x, rounde.x);
            }

            if (getOps<bool>("worldName")) {
                float textX = MC::windowSize.x / 2.f;
                float textY = realcenter.y;

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(SDK::clientInstance->getLocalPlayer()->getLevel()->getLevelData()->getLevelName()).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX, textY, FlarialGUI::to_wide(cache_worldName).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
            }
            else realcenter.y -= keycardSize * 0.75f;

            if (getOps<bool>("serverPing")) {
                static float p1 = 0.175;
                static float p2 = 0.196;
                static float p3 = 0.7;
                static float p4 = 0.77;

                realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                float textX1 = (MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.2f);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX1 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        realcenter.y + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        L"Ping:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX1, realcenter.y, L"Ping:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                ImVec2 part2Metrics = FlarialGUI::getFlarialTextSize(L"Ping:_", keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                FlarialGUI::image(getPingImage(SDK::getServerPing()), D2D1::RectF(
                                      (MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.f) + part2Metrics.x + Constraints::SpacingConstraint(p1, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      realcenter.y + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p2, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize),
                                      ((MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.f) + part2Metrics.x) + Constraints::SpacingConstraint(p3, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      (realcenter.y + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p4, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize))));

                float textX2 = (MC::windowSize.x / 2.f) - (curPingMetrics.x / 2.2f) + part2Metrics.x + Constraints::SpacingConstraint(0.5, keycardSize);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX2 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        realcenter.y + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(std::format(" {}ms", SDK::getServerPing())).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX2, realcenter.y, FlarialGUI::to_wide(std::format(" {}ms", SDK::getServerPing())).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
            }
            else realcenter.y -= keycardSize * 0.5f;

            realcenter.y += keycardSize * 1.25f;

            for (size_t i = 0; i < vecmap.size(); i++) {
                if (!vecmap[i].name.size() || vecmap[i].name.empty()) continue;

                std::string pname = String::removeColorCodes(vecmap[i].name);
                if (pname.empty()) continue;

                if (refreshCache) vectab[i].clearedName = String::removeNonAlphanumeric(pname);
                if (vectab[i].clearedName.empty()) vectab[i].clearedName = pname;

                if (refreshCache) {
                    if (module && module->isEnabled() && !NickModule::backupOri.empty() && vectab[i].clearedName == String::removeNonAlphanumeric(String::removeColorCodes(NickModule::backupOri))) {
                        if (std::string nick = module->getOps<std::string>("nick"); !nick.empty()) vectab[i].clearedName = nick;
                    }
                }

                float xx = 0;

                if (showHeads) {
                    // PLAYER HEAD START
                    const auto &skinImage = vecmap[i].playerSkin.mSkinImage;

                    if (logDebug)
                        Logger::debug("Player {} skin: format={}, size={}x{}, data_size={}",
                                      vecmap[i].name,
                                      static_cast<int>(skinImage.imageFormat),
                                      skinImage.mWidth, skinImage.mHeight,
                                      skinImage.mImageBytes.size());

                    if (static_cast<int>(skinImage.imageFormat) == 4 && // RGBA8Unorm format
                        skinImage.mWidth == skinImage.mHeight && // Square texture
                        skinImage.mWidth == 64 || skinImage.mWidth == 128 && // 64x64 or 128x128
                        (skinImage.mWidth & (skinImage.mWidth - 1)) == 0) {
                        // Power of 2

                        // Determine skin format based on width (0 = classic 64x64, 1 = slim 64x64+)
                        int skinFormat = (skinImage.mWidth == 64) ? 0 : 1;

                        std::string uniqueTextureKey = vecmap[i].playerSkin.mId;

                        bool alrExists = false;

                        if (SwapchainHook::queue != nullptr && g_playerHeadTextures.contains(uniqueTextureKey)) alrExists = true;
                        else if (g_playerHeadTexturesDX11.contains(uniqueTextureKey)) alrExists = true;

                        std::vector<unsigned char> head = SkinStealCommand::cropHead(skinImage, skinFormat);
                        std::vector<unsigned char> head2 = SkinStealCommand::cropHead(skinImage, skinFormat, true);

                        if (head.empty() && head2.empty()) {
                            if (logDebug) Logger::debug("Empty head data for player {}", vecmap[i].name);
                            continue;
                        }
                        // Dynamic head size detection and scaling for crisp pixels
                        // Support any skin resolution (64x64, 128x128, 256x256, etc.)
                        const int headSize = skinImage.mWidth / 8; // Head is always 1/8th of skin width
                        const int scaledSize = std::max(64, headSize * 2); // At least 64x64, or 2x the original head size
                        const int scale = scaledSize / headSize;
                        std::vector<unsigned char> scaledHead(scaledSize * scaledSize * 4);
                        std::vector<unsigned char> scaledHead2(scaledSize * scaledSize * 4);

                        if (!alrExists) {
                            for (int y = 0; y < scaledSize; y++) {
                                for (int x = 0; x < scaledSize; x++) {
                                    int srcX = x / scale;
                                    int srcY = y / scale;
                                    int srcIndex = (srcY * headSize + srcX) * 4;
                                    int dstIndex = (y * scaledSize + x) * 4;

                                    scaledHead[dstIndex + 0] = head[srcIndex + 0]; // R
                                    scaledHead[dstIndex + 1] = head[srcIndex + 1]; // G
                                    scaledHead[dstIndex + 2] = head[srcIndex + 2]; // B
                                    scaledHead[dstIndex + 3] = head[srcIndex + 3]; // A

                                    scaledHead2[dstIndex + 0] = head2[srcIndex + 0]; // R
                                    scaledHead2[dstIndex + 1] = head2[srcIndex + 1]; // G
                                    scaledHead2[dstIndex + 2] = head2[srcIndex + 2]; // B
                                    scaledHead2[dstIndex + 3] = head2[srcIndex + 3]; // A
                                }
                            }
                        }

                        std::string playerName = String::removeNonAlphanumeric(String::removeColorCodes(vecmap[i].name));
                        if (logDebug) {
                            // Debug: Check if the scaled head has any non-transparent pixels
                            bool hasVisiblePixels = false;
                            for (size_t j = 3; j < scaledHead.size(); j += 4) {
                                if (scaledHead[j] > 0) {
                                    // Alpha channel
                                    hasVisiblePixels = true;
                                    break;
                                }
                                if (scaledHead2[j] > 0) {
                                    // Alpha channel
                                    hasVisiblePixels = true;
                                    break;
                                }
                            }

                            Logger::debug("Player {} scaled head: size={}, visible_pixels={}", playerName, scaledHead.size(), hasVisiblePixels);
                        }

                        if (refreshCache) {
                            headDisplaySize = Constraints::SpacingConstraint(0.6, keycardSize);
                            headDisplaySize2 = Constraints::SpacingConstraint(0.68, keycardSize);
                            diff = headDisplaySize2 - headDisplaySize;

                            // Position for the head (left of the player name)
                            vectab[i].headPos = ImVec2(fakex + Constraints::SpacingConstraint(0.45, keycardSize), realcenter.y + Constraints::SpacingConstraint(0.3, keycardSize));
                            vectab[i].headPos2 = ImVec2(
                                fakex + Constraints::SpacingConstraint(0.45, keycardSize) - diff / 2.f,
                                realcenter.y + Constraints::SpacingConstraint(0.3, keycardSize) - diff / 2.f);
                            vectab[i].headSize2D = ImVec2(headDisplaySize, headDisplaySize);
                            vectab[i].headSize22D = ImVec2(headDisplaySize2, headDisplaySize2);
                        }

                        if (SwapchainHook::queue != nullptr) {
                            // DX12 path
                            PlayerHeadTexture *playerTex = CreateTextureFromBytesDX12(uniqueTextureKey, scaledHead.data(), scaledSize, scaledSize);
                            PlayerHeadTexture *playerTex2 = CreateTextureFromBytesDX12("_" + uniqueTextureKey, scaledHead2.data(), scaledSize, scaledSize);

                            if (playerTex && playerTex->valid && playerTex->loadState == PlayerHeadLoadState::Loaded) {
                                // Update last used time when rendering
                                {
                                    std::lock_guard<std::mutex> lock(g_playerHeadMutex);
                                    playerTex->lastUsed = std::chrono::steady_clock::now();
                                }

                                ImDrawList *drawList = ImGui::GetForegroundDrawList();
                                if (drawList) {
                                    if (logDebug) Logger::debug("DX12 GPU handle for {}: 0x{:x}", playerName, playerTex->srvGpuHandle.ptr);

                                    // Render with point filtering for crisp pixels
                                    RenderPlayerHeadWithPointFiltering(playerTex->srvGpuHandle, D2D1::RectF(vectab[i].headPos.x, vectab[i].headPos.y, vectab[i].headPos.x + vectab[i].headSize2D.x, vectab[i].headPos.y + vectab[i].headSize2D.y), playerName);
                                    RenderPlayerHeadWithPointFiltering(playerTex2->srvGpuHandle, D2D1::RectF(vectab[i].headPos2.x, vectab[i].headPos2.y, vectab[i].headPos2.x + vectab[i].headSize22D.x, vectab[i].headPos2.y + vectab[i].headSize22D.y), "_" + playerName);

                                    if (logDebug) Logger::debug("Rendered DX12 head for player {} at ({}, {})", playerName, vectab[i].headPos.x, vectab[i].headPos.y);
                                }
                            } else {
                                if (logDebug) Logger::debug("Failed to create/get DX12 texture for player {}", playerName);
                            }
                        } else if (SwapchainHook::d3d11Device != nullptr) {
                            // DX11 path
                            ID3D11ShaderResourceView *srv = CreateTextureFromBytesDX11(uniqueTextureKey, scaledHead.data(), scaledSize, scaledSize);
                            ID3D11ShaderResourceView *srv2 = CreateTextureFromBytesDX11("_" + uniqueTextureKey, scaledHead2.data(), scaledSize, scaledSize);

                            if (srv) {
                                ImDrawList *drawList = ImGui::GetForegroundDrawList();
                                if (drawList) {
                                    if (logDebug) Logger::debug("DX11 SRV for {}: 0x{:x}", playerName, reinterpret_cast<uintptr_t>(srv));

                                    // Render with point filtering for crisp pixels
                                    RenderPlayerHeadWithPointFilteringDX11(srv, D2D1::RectF(vectab[i].headPos.x, vectab[i].headPos.y, vectab[i].headPos.x + vectab[i].headSize2D.x, vectab[i].headPos.y + vectab[i].headSize2D.y), playerName);
                                    RenderPlayerHeadWithPointFilteringDX11(srv2, D2D1::RectF(vectab[i].headPos2.x, vectab[i].headPos2.y, vectab[i].headPos2.x + vectab[i].headSize22D.x, vectab[i].headPos2.y + vectab[i].headSize22D.y), "_" + playerName);

                                    if (logDebug) Logger::debug("Rendered DX11 head for player {} at ({}, {})", playerName, vectab[i].headPos.x, vectab[i].headPos.y);
                                }
                                // Don't release - it's managed by the cache now
                            } else {
                                if (logDebug) Logger::debug("Failed to create/get DX11 texture for player {}", playerName);
                            }
                        } else {
                            if (logDebug) Logger::debug("No DirectX device available for player {}", playerName);
                        }
                    }
                    // PLAYER HEAD END
                }

                auto pit = std::ranges::find(APIUtils::onlineUsers, vectab[i].clearedName);
                if (refreshCache) {
                    vectab[i].pNameMetrics = FlarialGUI::getFlarialTextSize(String::StrToWStr(vectab[i].clearedName).c_str(), columnx[i / maxColumn] - (0.825 * keycardSize), keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);
                    vectab[i].textWidth = columnx[i / maxColumn] - (0.825 * keycardSize);
                }

                if (pit != APIUtils::onlineUsers.end()) {
                    // FLARIAL TAG START
                    static float p1 = 0.175;
                    static float p2 = 0.196;
                    static float p3 = 0.7;
                    static float p4 = 0.77;

                    int imageResource = roleLogos["Default"];
                    for (const auto &[role, resource]: roleLogos) {
                        if (APIUtils::hasRole(role, vectab[i].clearedName)) {
                            imageResource = resource;
                            break;
                        }
                    }

                    if (refreshCache) {
                        float lol = columnx[i / maxColumn] - vectab[i].pNameMetrics.x;
                        float trollOffset = keycardSize * (showHeads ? 1 : 0.3f);

                        if (!showHeads && textAlignment == "Center") trollOffset -= keycardSize * 1.75f;

                        vectab[i].imageRect = D2D1::RectF(
                            fakex + Constraints::SpacingConstraint(p1, keycardSize) + trollOffset + (textAlignment == "Center" ? (lol / 2.f) - trollOffset * 0.75f : 0.f),
                            realcenter.y + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize),
                            fakex + Constraints::SpacingConstraint(p3, keycardSize) + trollOffset + (textAlignment == "Center" ? (lol / 2.f) - trollOffset * 0.75f : 0.f),
                            realcenter.y + Constraints::SpacingConstraint(p4, keycardSize) + Constraints::SpacingConstraint(0.17f, keycardSize));
                    }

                    FlarialGUI::image(imageResource, vectab[i].imageRect);

                    xx += Constraints::SpacingConstraint(0.6, keycardSize);

                    // FLARIAL TAG END

                    if (refreshCache) {
                        vectab[i].textX = fakex + (textAlignment == "Center" ? xx / 2.f : textAlignment == "Right" ? 0 : xx) + keycardSize * (showHeads ? 1.2f : 0.5f);
                        vectab[i].textY = realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize);
                    }
                    if (getOps<bool>("textShadow")) {
                        if (refreshCache) {
                            vectab[i].textShadowX = vectab[i].textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                            vectab[i].textShadowY = vectab[i].textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                        }

                        FlarialGUI::FlarialTextWithFont(
                            vectab[i].textShadowX,
                            vectab[i].textShadowY,
                            String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);
                    }

                    FlarialGUI::FlarialTextWithFont(vectab[i].textX, vectab[i].textY, String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
                } else {
                    if (getOps<bool>("textShadow")) {
                        if (refreshCache) {
                            vectab[i].textShadowY2 = (realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize)) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                            vectab[i].textShadowX2 = (fakex + xx + keycardSize * (showHeads ? 1.2f : 0.5f)) + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale");
                        }
                        FlarialGUI::FlarialTextWithFont(
                            vectab[i].textShadowX2,
                            vectab[i].textShadowY2,
                            String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);
                    }

                    if (refreshCache) {
                        vectab[i].nfTextX = fakex + xx + keycardSize * (showHeads ? 1.2f : 0.5f);
                        vectab[i].nfTextY = realcenter.y + Constraints::SpacingConstraint(0.12, keycardSize);
                    }

                    FlarialGUI::FlarialTextWithFont(vectab[i].nfTextX, vectab[i].nfTextY, String::StrToWStr(vectab[i].clearedName).c_str(), vectab[i].textWidth, keycardSize, alignments[getOps<std::string>("textalignment")], floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
                }
                realcenter.y += Constraints::SpacingConstraint(0.70, keycardSize);

                if ((i + 1) % maxColumn == 0) {
                    realcenter.y -= Constraints::SpacingConstraint(0.70, keycardSize) * maxColumn;
                    fakex += columnx[i / maxColumn];
                }
            }

            if (getOps<bool>("playerCount")) {
                float curY = vec2.y + totalHeight;

                if (getOps<bool>("serverPing") == getOps<bool>("worldName")) curY -= keycardSize * 1.75f;
                else if (!getOps<bool>("serverPing")) curY -= keycardSize * 1.75f;
                else if (!getOps<bool>("worldName")) curY -= keycardSize * 1.5f;

                static float p1 = 0.175;
                static float p2 = 0.196;
                static float p3 = 0.7;
                static float p4 = 0.77;

                int imageResource = roleLogos["Default"];
                for (const auto &[role, resource]: roleLogos) {
                    if (APIUtils::hasRole(role, cache_curPlayer)) {
                        imageResource = resource;
                        break;
                    }
                }

                if (refreshCache) textX1 = (MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.2f);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX1 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        curY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        L"Player:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX1, curY, L"Player:", 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                if (refreshCache) part1Metrics = FlarialGUI::getFlarialTextSize(L"Player:_", keycardSize * 5, keycardSize, DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, true);

                FlarialGUI::image(imageResource, D2D1::RectF(
                                      (MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.f) + part1Metrics.x + Constraints::SpacingConstraint(p1, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      curY + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p2, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize),
                                      ((MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.f) + part1Metrics.x) + Constraints::SpacingConstraint(p3, keycardSize) - Constraints::SpacingConstraint(0.1f, keycardSize),
                                      (curY + Constraints::SpacingConstraint(p2, keycardSize) + Constraints::SpacingConstraint(p4, keycardSize) - Constraints::SpacingConstraint(0.05f, keycardSize))));

                if (refreshCache) textX2 = (MC::windowSize.x / 2.f) - (curPlayerMetrics.x / 2.2f) + part1Metrics.x + Constraints::SpacingConstraint(0.5, keycardSize);

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX2 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        curY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(cache_curPlayer).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX2, curY, FlarialGUI::to_wide(cache_curPlayer).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_LEADING, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);

                curY += Constraints::SpacingConstraint(0.70, keycardSize);

                if (refreshCache) textX3 = MC::windowSize.x / 2.f;

                if (getOps<bool>("textShadow"))
                    FlarialGUI::FlarialTextWithFont(
                        textX3 + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        curY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * getOps<float>("uiscale"),
                        FlarialGUI::to_wide(cache_playerCount).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, getColor("textShadow"), true);

                FlarialGUI::FlarialTextWithFont(textX3, curY, FlarialGUI::to_wide(cache_playerCount).c_str(), 0, keycardSize * 0.5f + Constraints::SpacingConstraint(0.70, keycardSize), DWRITE_TEXT_ALIGNMENT_CENTER, floor(fontSize), DWRITE_FONT_WEIGHT_NORMAL, textColor, true);
            }

            refreshCache = false;
        }
    }
}

void TabList::onMouse(const MouseEvent &event) {
    if (!this->isEnabled()) return;
    if (Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Press) keybindActions[0]({});
    if (!getOps<bool>("togglable") && Utils::getMouseAsString(event.getButton()) == getOps<std::string>("keybind") && event.getAction() == MouseAction::Release) this->active = false;
}

void TabList::onKey(const KeyEvent &event) {
    if (!this->isEnabled()) return;
    if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key)) keybindActions[0]({});

    if (!getOps<bool>("togglable") && !this->isKeybind(event.keys)) this->active = false;
}