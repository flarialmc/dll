#include "TabList.hpp"
#include "SDK/Client/Actor/LocalPlayer.hpp"
#include "Utils/APIUtils.hpp"

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
#include "Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

extern bool logDebug;

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

std::unordered_map<std::string, PlayerHeadTextureDX11> g_playerHeadTexturesDX11;
std::unordered_map<std::string, PlayerHeadTexture> g_playerHeadTextures;
std::mutex g_playerHeadMutex;

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

bool TabList::AllocatePlayerHeadDescriptor(const std::string& playerName, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle, UINT* out_descriptor_id) {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);

    if (!SwapchainHook::d3d12DescriptorHeapImGuiRender || !SwapchainHook::d3d12Device5) {
        Logger::custom(fg(fmt::color::red), "PlayerHeadDescriptor", "ERROR: Descriptor heap or device not available");
        return false;
    }

    // Check if we already have a descriptor for this player
    for (auto& [descriptorId, info] : playerHeadDescriptors) {
        if (info.playerName == playerName && info.inUse) {
            info.lastUsed = std::chrono::steady_clock::now();

            // Calculate descriptor handles - convert playerhead ID to heap index
            UINT heapIndex = descriptorId - PLAYERHEAD_DESCRIPTOR_START + SwapchainHook::IMGUI_FONT_DESCRIPTORS + SwapchainHook::MAX_IMAGE_DESCRIPTORS;

            UINT handle_increment = SwapchainHook::d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

            D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
            cpu.ptr += (handle_increment * heapIndex);

            D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
            gpu.ptr += (handle_increment * heapIndex);

            *out_cpu_handle = cpu;
            *out_gpu_handle = gpu;
            *out_descriptor_id = descriptorId;

            return true;
        }
    }

    UINT descriptorId;

    // Try to reuse a freed descriptor first
    if (!freePlayerHeadDescriptors.empty()) {
        descriptorId = freePlayerHeadDescriptors.front();
        freePlayerHeadDescriptors.pop();
        Logger::custom(fg(fmt::color::cyan), "PlayerHeadDescriptor", "Reusing freed descriptor {} for player '{}'", descriptorId, playerName);
    } else {
        // Allocate new descriptor
        static bool initialized = false;
        if (!initialized) {
            nextPlayerHeadDescriptorId = PLAYERHEAD_DESCRIPTOR_START;
            initialized = true;
        }

        if (nextPlayerHeadDescriptorId >= PLAYERHEAD_DESCRIPTOR_END) {
            Logger::custom(fg(fmt::color::red), "PlayerHeadDescriptor", "ERROR: No more playerhead descriptors available (reached limit of {})", MAX_PLAYERHEAD_DESCRIPTORS);
            return false;
        }

        descriptorId = nextPlayerHeadDescriptorId++;
    }

    // Calculate descriptor handles - convert playerhead ID to heap index
    UINT heapIndex = descriptorId - PLAYERHEAD_DESCRIPTOR_START + SwapchainHook::IMGUI_FONT_DESCRIPTORS + SwapchainHook::MAX_IMAGE_DESCRIPTORS;

    if (heapIndex >= SwapchainHook::TOTAL_CONSOLIDATED_DESCRIPTORS) {
        Logger::custom(fg(fmt::color::red), "PlayerHeadDescriptor", "ERROR: Calculated heap index {} exceeds total descriptors {} (descriptor ID {})", heapIndex, SwapchainHook::TOTAL_CONSOLIDATED_DESCRIPTORS, descriptorId);
        return false;
    }

    UINT handle_increment = SwapchainHook::d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart();
    cpu.ptr += (handle_increment * heapIndex);

    D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart();
    gpu.ptr += (handle_increment * heapIndex);

    // Store descriptor info
    playerHeadDescriptors[descriptorId] = {
        playerName,
        std::chrono::steady_clock::now(),
        true
    };

    *out_cpu_handle = cpu;
    *out_gpu_handle = gpu;
    *out_descriptor_id = descriptorId;

    return true;
}

void TabList::FreePlayerHeadDescriptor(UINT descriptorId) {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);

    auto it = playerHeadDescriptors.find(descriptorId);
    if (it != playerHeadDescriptors.end()) {
        it->second.inUse = false;
        freePlayerHeadDescriptors.push(descriptorId);
        playerHeadDescriptors.erase(it);
    }
}

void TabList::CleanupOldPlayerHeads(size_t maxCached) {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);

    if (playerHeadDescriptors.size() <= maxCached) {
        return; // No need to cleanup
    }

    // Collect descriptors sorted by last used time
    std::vector<std::pair<UINT, std::chrono::steady_clock::time_point>> descriptorAges;
    for (const auto& [descriptorId, info] : playerHeadDescriptors) {
        if (info.inUse) {
            descriptorAges.emplace_back(descriptorId, info.lastUsed);
        }
    }

    // Sort by oldest first
    std::sort(descriptorAges.begin(), descriptorAges.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

    // Free oldest descriptors to get under the limit
    size_t toRemove = playerHeadDescriptors.size() - maxCached;
    size_t removed = 0;

    for (const auto& [descriptorId, _] : descriptorAges) {
        if (removed >= toRemove) break;

        auto it = playerHeadDescriptors.find(descriptorId);
        if (it != playerHeadDescriptors.end()) {
            freePlayerHeadDescriptors.push(descriptorId);
            playerHeadDescriptors.erase(it);
            removed++;
        }
    }

    Logger::custom(fg(fmt::color::green), "PlayerHeadDescriptor", "Cleaned up {} old playerhead descriptors, {} remaining", removed, playerHeadDescriptors.size());
}

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
                TabList::FreePlayerHeadDescriptor(pair.second.descriptorId);
            }
        }
        g_playerHeadTextures.clear();
    }

    // Clear DX11 textures
    g_playerHeadTexturesDX11.clear();
}

// Reset all TabList descriptor state - called during swapchain recreation
void TabList::ResetDescriptorState() {
    std::lock_guard<std::mutex> lock(playerHeadDescriptorMutex);

    Logger::debug("TabList: Resetting descriptor state - clearing {} descriptors", playerHeadDescriptors.size());

    // Clear all descriptor allocations
    playerHeadDescriptors.clear();

    // Clear the free descriptor queue
    while (!freePlayerHeadDescriptors.empty()) {
        freePlayerHeadDescriptors.pop();
    }

    // Reset the next descriptor ID to start
    nextPlayerHeadDescriptorId = PLAYERHEAD_DESCRIPTOR_START;

    // Clear all texture caches to force recreation
    {
        std::lock_guard<std::mutex> textureLock(g_playerHeadMutex);
        g_playerHeadTextures.clear();
        g_playerHeadTexturesDX11.clear();
    }

    Logger::debug("TabList: Descriptor state reset complete");
}

void PlayerHeadLoaderWorker() {
    while (!g_shouldStopLoading) {
        PlayerHeadLoadTask task;

        // Wait for a task
        {
            std::unique_lock<std::mutex> lock(g_loadQueueMutex);
            g_loadQueueCV.wait(lock, [] { return !g_loadQueue.empty() || g_shouldStopLoading; });

            if (g_shouldStopLoading) break;

            if (g_loadQueue.empty()) continue;

            task = std::move(g_loadQueue.front());
            g_loadQueue.pop();
        }

        if (logDebug) Logger::debug("Worker thread processing texture for player {}", task.playerName);

        // Process the texture (scaling, etc.)
        PlayerHeadReadyTexture ready;
        ready.playerName = task.playerName;
        ready.processedData = std::move(task.imageData);
        ready.width = task.width;
        ready.height = task.height;
        ready.isDX12 = task.isDX12;

        // Queue for main thread processing
        {
            std::lock_guard<std::mutex> lock(g_readyQueueMutex);
            g_readyQueue.push(std::move(ready));
        }
    }

    if (logDebug) Logger::debug("PlayerHead loader worker thread exiting");
}

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
    TabList::CleanupOldPlayerHeads(500); // Keep at most 500 cached playerheads
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
    if (!TabList::AllocatePlayerHeadDescriptor(playerName,
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

int getPingImage(int ping) {
    if (ping > 300) return IDR_PING_QUIT;
    else if (ping > 250) return IDR_PING_BAD;
    else if (ping > 175) return IDR_PING_OKAY;
    else if (ping > 99) return IDR_PING_GOOD;
    else return IDR_PING_AMAZING;
}