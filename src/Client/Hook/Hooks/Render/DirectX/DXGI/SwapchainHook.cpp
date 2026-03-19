#include "SwapchainHook.hpp"
#include <chrono>
#include "../../../../../GUI/D2D.hpp"
#include "d2d1.h"
#include "../../../../../Client.hpp"
#include <Psapi.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_dx12.h>
#include "unknwnbase.h"
#include "UnderUIHooks.hpp"
#include "CreateSwapChainForHwndHook.hpp"
#include "CreateSwapchainForCoreWindowHook.hpp"
#include "ResizeHook.hpp"
#include "../../../../../Module/Modules/DepthOfField/DepthOfFieldHelper.hpp"
#include "../../../../../Module/Manager.hpp"
using ::IUnknown;

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", 0) {}

winrt::com_ptr<ID3D12CommandQueue> SwapchainHook::queue = nullptr;
HANDLE SwapchainHook::fenceEvent = nullptr;

std::atomic<bool> SwapchainHook::initImgui{false};
bool allfontloaded = false;
bool first = false;
bool imguiWindowInit = false;
BOOL underscoreFlag = FALSE, dollarFlag = FALSE;

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
static std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

HWND window2 = FindWindowA(nullptr, "Minecraft");

int SwapchainHook::currentBitmap;

bool unloadDll(const wchar_t *moduleName) {
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (hModule != nullptr) {
        if (FreeLibrary(hModule)) {
            Logger::debug("DLL unloaded");
            return true;
        }
        LOG_ERROR("Failed to FreeLibrary");
        return false;
    }
    LOG_ERROR("Failed to unload DLL");
    return false;
}

bool SwapchainHook::containsModule(const std::wstring &moduleName) {
    HMODULE hModule = GetModuleHandleW(moduleName.c_str());
    return hModule != nullptr;
}

HWND FindWindowByTitle(const std::string &titlePart) {
    HWND hwnd = nullptr;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        char title[256];
        GetWindowTextA(hWnd, title, sizeof(title));
        if (strstr(title, reinterpret_cast<const char *>(lParam)) != nullptr) {
            *reinterpret_cast<HWND *>(lParam) = hWnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(titlePart.c_str()));
    return hwnd;
}
enum class GraphicsAPI { Unknown, D3D11, D3D12 };

GraphicsAPI DetectSwapchainAPI(IDXGISwapChain* swapchain) {
    if (!swapchain) return GraphicsAPI::Unknown;

    winrt::com_ptr<IUnknown> device;
    if (SUCCEEDED(swapchain->GetDevice(IID_PPV_ARGS(device.put())))) {
        if (auto d3d12Device = device.try_as<ID3D12Device>()) {
            return GraphicsAPI::D3D12;
        }
        if (auto d3d11Device = device.try_as<ID3D11Device>()) {
            return GraphicsAPI::D3D11;
        }
    }

    return GraphicsAPI::Unknown;
}
void SwapchainHook::enableHook() {

    recreate = Client::settings.getSettingByName<bool>("recreateAtStart")->value;
    if (Client::settings.getSettingByName<bool>("killdx")->value) recreate = true;

    if (!window2) {
        window2 = FindWindowByTitle("Minecraft");
    }

    if (!window2) {
        window2 = FindWindowByTitle("Flarial");
    }

    // Phase 1: Hook Present using kiero's vtable guess.
    // This works on most systems because DXGI's Present function is shared between DX11/DX12.
    // Phase 2 (in create-swapchain hooks) will re-hook from the live swapchain's vtable
    // to handle systems where kiero's guess differs from the actual game API.
    if (kiero::getRenderType() == kiero::RenderType::D3D12) {
        Logger::debug("[Swapchain] Phase 1: kiero detected DX12, hooking Present at kiero index 140");
        kiero::bind(140, (void **) &funcOriginal, (void *) swapchainCallback);
    } else if (kiero::getRenderType() == kiero::RenderType::D3D11) {
        Logger::debug("[Swapchain] Phase 1: kiero detected DX11, hooking Present at kiero index 8");
        kiero::bind(8, (void **) &funcOriginal, (void *) swapchainCallback);
    } else {
        Logger::warn("[Swapchain] Phase 1: kiero render type is {} — Present hook may not work until Phase 2",
                     static_cast<int>(kiero::getRenderType()));
    }

    winrt::com_ptr<IDXGIFactory2> pFactory;
    CreateDXGIFactory(IID_PPV_ARGS(pFactory.put()));
    if (!pFactory) LOG_ERROR("Factory not created");

    CreateSwapchainForCoreWindowHook::hook(pFactory.get());
    CreateSwapChainForHwndHook::Hook(pFactory.get());

    winrt::com_ptr<IDXGIAdapter> adapter;
    pFactory->EnumAdapters(0, adapter.put());
    DXGI_ADAPTER_DESC desc;
    adapter->GetDesc(&desc);
    std::wstring gpuNameW(desc.Description);
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, gpuNameW.c_str(), (int)gpuNameW.size(), NULL, 0, NULL, NULL);
    std::string gpuName(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, gpuNameW.c_str(), (int)gpuNameW.size(), gpuName.data(), sizeNeeded, NULL, NULL);
    MC::GPU = gpuName;
    Logger::info("GPU name: {}", gpuName.c_str());

    bool isRTSS = containsModule(L"RTSSHooks64.dll");
    bool isMedal = containsModule(L"medal-hook64.dll");
    if (isRTSS) {
        Logger::debug("[Swapchain] MSI Afterburner failed to unload!");
        Utils::MessageDialogW(L"Flarial: client failed to initialize, disable MSI Afterburner or RTSS!", L"Error!");
        ModuleManager::terminate();
        Client::disable = true;
    }
isMedal = false;
    if (isMedal && (Client::settings.getSettingByName<bool>("vsync")->value || Client::settings.getSettingByName<bool>("killdx")->value))
    {
        Logger::debug("[Swapchain] Medal failed to unload!");
        Utils::MessageDialogW(L"Flarial: client failed to initialize, close Medal! We current don't support it with V-SYNC DISABLER or BETTER FRAMES.", L"Error!");
        ModuleManager::terminate();
        Client::disable = true;
    }

}

std::atomic<bool> SwapchainHook::init{false};
bool SwapchainHook::currentVsyncState;


// Extern declaration for dx12DeviceRemoved flag from SwapchainHook_DX12.cpp (Better Frames)
extern bool dx12DeviceRemoved;
bool dx11FallbackComplete = false; // Track if we've successfully switched to DX11 (non-static for extern access)

bool SwapchainHook::canForceDX11Fallback() {
    // Can force fallback if: we're on DX12, haven't already triggered RemoveDevice, and haven't completed fallback
    // Use d3d12Device5.get() for explicit null check - winrt::com_ptr comparison can be finicky
    return isDX12 && !dx12DeviceRemoved && !dx11FallbackComplete && d3d12Device5.get() != nullptr;
}

void SwapchainHook::forceDX11Fallback() {
    if (!canForceDX11Fallback()) {
        Logger::warn("[Swapchain] Cannot force DX11 fallback - conditions not met");
        return;
    }

    Logger::success("[Swapchain] Force DX11 Fallback: Calling RemoveDevice()");
    d3d12Device5->RemoveDevice();
    dx12DeviceRemoved = true;
    // The swapchainCallback will handle the rest - detecting DX11 and reinitializing
}

void SwapchainHook::rehookPresentFromSwapchain(IDXGISwapChain* pSwapChain) {
    if (!pSwapChain) return;

    // IDXGISwapChain::Present is always at vtable index 8, regardless of DX11/DX12
    void** vtable = *reinterpret_cast<void***>(pSwapChain);
    constexpr int PRESENT_VTABLE_INDEX = 8;
    void* livePresentAddr = vtable[PRESENT_VTABLE_INDEX];

    // Try to create a hook at the live swapchain's Present address.
    // If kiero already hooked this exact address, MH_CreateHook returns MH_ERROR_ALREADY_CREATED — no mismatch.
    // If the address differs (kiero's dummy vtable vs game's real vtable), a new hook is created.
    MH_STATUS status = MH_CreateHook(livePresentAddr, reinterpret_cast<void*>(swapchainCallback),
                                      reinterpret_cast<void**>(&funcOriginal));
    if (status == MH_OK) {
        MH_EnableHook(livePresentAddr);
        Logger::success("[Swapchain] Phase 2: Re-hooked Present from live swapchain vtable "
                        "(kiero had a different address — this fixes the mismatch)");
    } else if (status == MH_ERROR_ALREADY_CREATED) {
        Logger::debug("[Swapchain] Phase 2: Present already hooked at live swapchain address (no mismatch)");
    } else {
        Logger::error("[Swapchain] Phase 2: Failed to re-hook Present, MH_STATUS={}", static_cast<int>(status));
    }
}

HRESULT SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {
    if (Client::disable || !Client::init) return funcOriginal(pSwapChain, syncInterval, flags);


    Logger::debug("{}", (void*)SDK::getBgfxContext()->getRendererContext());
    // Detect the actual API from the swapchain
    GraphicsAPI detectedAPI = DetectSwapchainAPI(pSwapChain);

    // One-time log: report whether runtime detection matches kiero's initial guess
    static bool apiMismatchLogged = false;
    if (!apiMismatchLogged) {
        apiMismatchLogged = true;
        bool kieroSaidDX12 = kiero::getRenderType() == kiero::RenderType::D3D12;
        bool runtimeIsDX12 = detectedAPI == GraphicsAPI::D3D12;
        if (kieroSaidDX12 != runtimeIsDX12) {
            Logger::warn("[Swapchain] API MISMATCH: kiero detected {} but runtime swapchain is {} "
                         "(Phase 2 rehook should have corrected this)",
                         kieroSaidDX12 ? "DX12" : "DX11",
                         runtimeIsDX12 ? "DX12" : "DX11");
        } else {
            Logger::debug("[Swapchain] API match confirmed: kiero and runtime both agree on {}",
                          runtimeIsDX12 ? "DX12" : "DX11");
        }
    }

    // If RemoveDevice() was called (Better Frames), check if game has recreated with DX11
    if (dx12DeviceRemoved && !dx11FallbackComplete)
    {
        if (detectedAPI == GraphicsAPI::D3D12)
        {
            // Swapchain is still DX12, waiting for game to recreate with DX11
            // Return error to trigger game's device lost handling
            return DXGI_ERROR_DEVICE_REMOVED;
        }
        // Game has recreated swapchain with DX11!
        isDX12 = false;
        init = false; // Need to reinitialize with the new DX11 swapchain
        swapchain = pSwapChain; // Update to new swapchain
        dx11FallbackComplete = true;
        Logger::success("[Swapchain] Better Frames: Game recreated swapchain with DX11!");

        // Re-hook ResizeBuffers for DX11 - the initial hook used DX12 indices from kiero
        ResizeHook::rehookForDX11(pSwapChain);
    }
    else if (!dx11FallbackComplete)
    {
        isDX12 = detectedAPI == GraphicsAPI::D3D12;
    }
    // else: dx11FallbackComplete is true, isDX12 stays false

    // Note: VSync changes require restart - the tearing flag must be set during swapchain creation
    // Don't trigger recreation here as it won't help and causes issues
    //if (containsModule(L"medal-hook64.dll")) recreate = false;

    if (recreate) {
        init = false;
        initImgui = false;
        recreate = false; // Reset flag BEFORE triggering recreation to prevent infinite loop
        Logger::debug("[Swapchain] Triggering swapchain recreation via ResizeBuffers");
        pSwapChain->ResizeBuffers(0, MC::windowSize.x, MC::windowSize.y, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

        return DXGI_ERROR_DEVICE_RESET;
    }

    if(!swapchain) swapchain = pSwapChain;
    if (D2D::context) MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);


    static bool hooker = false;

    if (!hooker && ((isDX12) || (!isDX12 && context))) {
        UnderUIHooks hook;
        hook.enableHook();
        hooker = true;
    }

    FPSMeasure();

    UnderUIHooks::resetPanoramaFrameFlag();


    if (!init) {

        if (isDX12) {

            DX12Init();

        } else {

            DX11Init();

        }

    } else {

        Fonts();

        RenderSync();

        if (D2D::context != nullptr && !Client::disable) {

            if (isDX12) {

                DX12Render();

            } else {

                // Note: Panorama shader now renders directly in PreSetupAndRenderEvent,
                // BEFORE the game's UI renders. We no longer need to fire RenderUnderUIEvent
                // here since that would be at Present time (after UI has already rendered).

                DX11Render();

            }
        }
    }

    try {
        if (init && initImgui && !FlarialGUI::hasLoadedAll) { FlarialGUI::LoadAllImages(); FlarialGUI::hasLoadedAll = true; }
    } catch (const std::exception &ex) { LOG_ERROR("Fail at loading all images: ", ex.what()); }

    if (currentVsyncState) {
        return funcOriginal(pSwapChain, 0, DXGI_PRESENT_ALLOW_TEARING);
    }

    return funcOriginal(pSwapChain, syncInterval, flags);

}


void SwapchainHook::RenderSync() {
    if (init && initImgui) {
        frameTransformsMtx.lock();
        while (FrameTransforms.size() > transformDelay) {
            MC::Transform = FrameTransforms.front();
            FrameTransforms.pop();
        }
        frameTransformsMtx.unlock();
    }
}

void SwapchainHook::prepareBlur() {

    auto blurIntensity = Client::settings.getSettingByName<float>("blurintensity")->value;

    if ((ModuleManager::doesAnyModuleHave("BlurEffect") &&
        blurIntensity > 1) && !FlarialGUI::inMenu) {


        Memory::SafeRelease(FlarialGUI::blur_bitmap_cache);

        FlarialGUI::PrepareBlur(blurIntensity);

        if (!FlarialGUI::blurbrush) {
            D2D1_IMAGE_BRUSH_PROPERTIES props = D2D1::ImageBrushProperties(
                    D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y));
            D2D::context->CreateImageBrush(FlarialGUI::blur_bitmap_cache, props, &FlarialGUI::blurbrush);
        }
        }
}


void SwapchainHook::Fonts() {
    if (ImGui::GetCurrentContext()) {
        static bool fontRebuildQueued = false;

        if (FlarialGUI::DoLoadFontLater) {
            FontKey fontK = FlarialGUI::LoadFontLater;
            FontKey originalFontK = fontK;

            if (Client::settings.getSettingByName<bool>("overrideFontWeight")->value) {
                fontK.weight = FlarialGUI::GetFontWeightFromString(Client::settings.getSettingByName<std::string>("fontWeight")->value);
            }

            if (!FlarialGUI::FontMap[fontK]) {
                bool fontLoaded = FlarialGUI::LoadFontFromFontFamily(fontK);

                if (!fontLoaded && Client::settings.getSettingByName<bool>("overrideFontWeight")->value) {
                    if (!FlarialGUI::FontMap[originalFontK]) {
                        FlarialGUI::LoadFontFromFontFamily(originalFontK);
                    }
                }
            }
            FlarialGUI::DoLoadFontLater = false;
        }

        auto& io = ImGui::GetIO();

        if (FlarialGUI::HasAFontLoaded) {
            fontRebuildQueued = true;
            FlarialGUI::HasAFontLoaded = false;
        }

        if (fontRebuildQueued) {
            io.Fonts->Build();
            
            static int frameDelay = 0;
            if (++frameDelay >= 3) {
                if (!isDX12) {
                    ImGui_ImplDX11_InvalidateDeviceObjects();
                    ImGui_ImplDX11_CreateDeviceObjects();
                } else {
                    ImGui_ImplDX12_InvalidateDeviceObjects();
                    ImGui_ImplDX12_CreateDeviceObjects();
                }
                fontRebuildQueued = false;
                frameDelay = 0;
            }
        }
    }
}
void SwapchainHook::FPSMeasure() {

    std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - start;
    MC::frames += 1;


    if (elapsed.count() >= 0.5f) {
        MC::fps = static_cast<int>((float) MC::frames / elapsed.count());
        MC::frames = 0;
        start = std::chrono::high_resolution_clock::now();
    }

    constexpr float targetFrameRate = 60.0f;

    std::chrono::duration<float> frameTime = std::chrono::high_resolution_clock::now() - previousFrameTime;
    previousFrameTime = std::chrono::high_resolution_clock::now();

    MC::frameTime = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(frameTime).count();

    float currentFrameRate = 1.0f / frameTime.count();

    FlarialGUI::frameFactor = targetFrameRate / currentFrameRate;

    FlarialGUI::frameFactor = std::min(FlarialGUI::frameFactor, 1.0f);

}


winrt::com_ptr<ID3D11Texture2D> SwapchainHook::GetBackbuffer() {
    std::lock_guard<std::mutex> lock(backbufferMutex);
    return SavedD3D11BackBuffer;
}

void SwapchainHook::InitializeBackbufferStorage(int maxFrames) {
    if (maxFrames <= 0 || maxFrames == maxBackbufferFrames) return;

    CleanupBackbufferStorage();

    maxBackbufferFrames = maxFrames;
    currentBackbufferIndex = 0;
    currentBackbufferIndexUnderUI = 0;
    backbufferStorage.resize(maxFrames);
    backbufferStorageUnderUI.resize(maxFrames);

    if (!SavedD3D11BackBuffer) { swapchain->GetBuffer(0, IID_PPV_ARGS(SavedD3D11BackBuffer.put())); }

    D3D11_TEXTURE2D_DESC textureDesc = {};
    SavedD3D11BackBuffer->GetDesc(&textureDesc);
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    SavedD3D11BackBuffer = nullptr;

    // Create all textures and SRVs upfront for regular storage
    for (int i = 0; i < maxFrames; ++i) {
        HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, backbufferStorage[i].texture.put());
        if (FAILED(hr)) continue;


        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        d3d11Device->CreateShaderResourceView(backbufferStorage[i].texture.get(), &srvDesc, backbufferStorage[i].srv.put());
    }

    // Create all textures and SRVs upfront for underUI storage
    for (int i = 0; i < maxFrames; ++i) {
        HRESULT hr = d3d11Device->CreateTexture2D(&textureDesc, nullptr, backbufferStorageUnderUI[i].texture.put());
        if (FAILED(hr)) continue;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        d3d11Device->CreateShaderResourceView(backbufferStorageUnderUI[i].texture.get(), &srvDesc, backbufferStorageUnderUI[i].srv.put());
    }
}

void SwapchainHook::CleanupBackbufferStorage() {
    backbufferStorage.clear();
    backbufferStorageUnderUI.clear();
    maxBackbufferFrames = 0;
    currentBackbufferIndex = 0;
    currentBackbufferIndexUnderUI = 0;
    validBackbufferFrames = 0;
    validBackbufferFramesUnderUI = 0;
}

winrt::com_ptr<ID3D11ShaderResourceView> SwapchainHook::GetCurrentBackbufferSRV(bool underUI) {
    static int logCounter = 0;

    if (underUI) {
        if (backbufferStorageUnderUI.empty() || validBackbufferFramesUnderUI == 0) {
            if (++logCounter % 300 == 1) Logger::debug("[MotionBlur] GetCurrentBackbufferSRV(underUI): empty={}, valid={}", backbufferStorageUnderUI.empty(), validBackbufferFramesUnderUI);
            return nullptr;
        }
        int prevIndex = (currentBackbufferIndexUnderUI - 1 + backbufferStorageUnderUI.size()) % backbufferStorageUnderUI.size();
        return backbufferStorageUnderUI[prevIndex].srv;
    } else {
        if (backbufferStorage.empty() || validBackbufferFrames == 0) {
            if (++logCounter % 300 == 1) Logger::debug("[MotionBlur] GetCurrentBackbufferSRV: empty={}, valid={}", backbufferStorage.empty(), validBackbufferFrames);
            return nullptr;
        }
        int prevIndex = (currentBackbufferIndex - 1 + backbufferStorage.size()) % backbufferStorage.size();
        return backbufferStorage[prevIndex].srv;
    }
}

void SwapchainHook::SaveBackbuffer(bool underui) {
    SavedD3D11BackBuffer = nullptr;

    if (!isDX12) {
        SwapchainHook::swapchain->GetBuffer(0, IID_PPV_ARGS(SavedD3D11BackBuffer.put()));

        if (FlarialGUI::needsBackBuffer && !backbufferStorage.empty() && !backbufferStorageUnderUI.empty()) {
            if (underui) {
                // Use underUI storage for frames without UI
                auto& currentStorage = backbufferStorageUnderUI[currentBackbufferIndexUnderUI];

                if (UnderUIHooks::bgfxCtx && UnderUIHooks::bgfxCtx->m_msaart) {
                    context->ResolveSubresource(currentStorage.texture.get(), 0, UnderUIHooks::bgfxCtx->m_msaart, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                } else {
                    context->CopyResource(currentStorage.texture.get(), SavedD3D11BackBuffer.get());
                }

                currentBackbufferIndexUnderUI = (currentBackbufferIndexUnderUI + 1) % backbufferStorageUnderUI.size();
                // Track that we have valid frame data in this storage
                if (validBackbufferFramesUnderUI < static_cast<int>(backbufferStorageUnderUI.size())) {
                    validBackbufferFramesUnderUI++;
                }
            } else {

                auto& currentStorage = backbufferStorage[currentBackbufferIndex];
                context->CopyResource(currentStorage.texture.get(), SavedD3D11BackBuffer.get());
                currentBackbufferIndex = (currentBackbufferIndex + 1) % backbufferStorage.size();
                // Track that we have valid frame data in this storage
                if (validBackbufferFrames < static_cast<int>(backbufferStorage.size())) {
                    validBackbufferFrames++;
                }
            }

        } else if (FlarialGUI::needsBackBuffer)
        {
            // Fallback to old behavior if storage not initialized
            // Use mutex to protect ExtraSavedD3D11BackBuffer access (prevents race with MotionBlur)
            winrt::com_ptr<ID3D11Texture2D> localExtraBuffer;
            {
                std::lock_guard<std::mutex> lock(backbufferMutex);

                // Check if we need to recreate the texture (first time or size changed)
                bool needsRecreate = !ExtraSavedD3D11BackBuffer;
                if (ExtraSavedD3D11BackBuffer) {
                    D3D11_TEXTURE2D_DESC existingDesc = {};
                    ExtraSavedD3D11BackBuffer->GetDesc(&existingDesc);
                    D3D11_TEXTURE2D_DESC currentDesc = {};
                    SavedD3D11BackBuffer->GetDesc(&currentDesc);
                    if (existingDesc.Width != currentDesc.Width || existingDesc.Height != currentDesc.Height) {
                        needsRecreate = true;
                    }
                }
                if (needsRecreate) {
                    ExtraSavedD3D11BackBuffer = nullptr;
                    D3D11_TEXTURE2D_DESC textureDesc = {};
                    SavedD3D11BackBuffer->GetDesc(&textureDesc);
                    textureDesc.Usage = D3D11_USAGE_DEFAULT;
                    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                    textureDesc.CPUAccessFlags = 0;

                    SwapchainHook::d3d11Device->CreateTexture2D(&textureDesc, nullptr, ExtraSavedD3D11BackBuffer.put());
                }

                // Take local copy for GPU operations outside the lock
                localExtraBuffer = ExtraSavedD3D11BackBuffer;
            }

            // Perform GPU operations with local copy (outside mutex to avoid blocking)
            if (localExtraBuffer) {
                if (underui) {
                    if (UnderUIHooks::bgfxCtx && UnderUIHooks::bgfxCtx->m_msaart) {
                        context->ResolveSubresource(localExtraBuffer.get(), 0, UnderUIHooks::bgfxCtx->m_msaart, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                    } else {
                        context->CopyResource(localExtraBuffer.get(), SavedD3D11BackBuffer.get());
                    }
                } else {
                    context->CopyResource(localExtraBuffer.get(), SavedD3D11BackBuffer.get());
                }
            }
        }
    } else {
        HRESULT hr = D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(SavedD3D11BackBuffer.put()));
        if (FAILED(hr)) {
            std::cout << "Failed to query interface: " << std::hex << hr << std::endl;
        }
    }
}

void SwapchainHook::SaveDepthmap(ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDepthStencilView) {
    static int callCounter = 0;

    if (!pDepthStencilView || isDX12) {
        if (++callCounter % 600 == 1) {
            Logger::debug("[SaveDepthmap] Early return - pDepthStencilView={}, isDX12={}",
                pDepthStencilView != nullptr, isDX12);
        }
        return;
    }

    // Check if either Depth of Field OR Real Motion Blur needs the depth buffer
    auto depthOfFieldModule = ModuleManager::getModule("Depth Of Field");
    auto motionBlurModule = ModuleManager::getModule("Motion Blur");

    bool dofNeedsDepth = depthOfFieldModule && depthOfFieldModule->isEnabled();
    bool motionBlurNeedsDepth = motionBlurModule && motionBlurModule->isEnabled()
                                && motionBlurModule->getOps<std::string>("blurType") == "Real Motion Blur";

    if (!dofNeedsDepth && !motionBlurNeedsDepth) {
        if (++callCounter % 600 == 1 && motionBlurModule && motionBlurModule->isEnabled()) {
            Logger::debug("[SaveDepthmap] No depth needed - blurType={}",
                motionBlurModule->getOps<std::string>("blurType"));
        }
        return;
    }

    if (++callCounter % 300 == 1) {
        Logger::debug("[SaveDepthmap] Capturing depth buffer - dof={}, motionBlur={}", dofNeedsDepth, motionBlurNeedsDepth);
    }

    ID3D11Resource* pResource = nullptr;
    pDepthStencilView->GetResource(&pResource);
    ID3D11Texture2D* pDepthBuffer = nullptr;
    pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pDepthBuffer);
    pResource->Release();

    D3D11_TEXTURE2D_DESC desc;
    pDepthBuffer->GetDesc(&desc);

    bool isMSAA = desc.SampleDesc.Count > 1;
    DXGI_FORMAT targetFormat = DXGI_FORMAT_R24G8_TYPELESS;

    bool needsRecreation = !cachedDepthMapTexture || !cachedDepthMapSRV ||
                          lastDepthMapWidth != desc.Width ||
                          lastDepthMapHeight != desc.Height ||
                          lastDepthMapSampleCount != desc.SampleDesc.Count ||
                          lastDepthMapFormat != targetFormat;

    if (needsRecreation) {
        // Release old resources
        cachedDepthMapTexture = nullptr;
        cachedDepthMapSRV = nullptr;

        // Create new texture
        D3D11_TEXTURE2D_DESC depthTexDesc = desc;
        depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
        depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        depthTexDesc.CPUAccessFlags = 0;
        depthTexDesc.MiscFlags = 0;
        depthTexDesc.SampleDesc = desc.SampleDesc;
        depthTexDesc.Format = targetFormat;

        HRESULT hr = d3d11Device->CreateTexture2D(&depthTexDesc, nullptr, cachedDepthMapTexture.put());
        if (FAILED(hr)) {
            Logger::debug("SwapchainHook::SaveDepthmap - Failed to create depth map texture, hr: {:x}", hr);
            pDepthBuffer->Release();
            return;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

        if (isMSAA) {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
            srvDesc.Texture2DMS.UnusedField_NothingToDefine = 0;
        } else {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
        }

        hr = d3d11Device->CreateShaderResourceView(cachedDepthMapTexture.get(), &srvDesc, cachedDepthMapSRV.put());
        if (FAILED(hr)) {
            Logger::debug("SwapchainHook::SaveDepthmap - Failed to create depth map SRV, hr: {:x}", hr);
            cachedDepthMapTexture = nullptr;
            pDepthBuffer->Release();
            return;
        }

        lastDepthMapWidth = desc.Width;
        lastDepthMapHeight = desc.Height;
        lastDepthMapSampleCount = desc.SampleDesc.Count;
        lastDepthMapFormat = targetFormat;
    }

    pContext->CopyResource(cachedDepthMapTexture.get(), pDepthBuffer);

    DepthOfFieldHelper::pDepthMapSRV = cachedDepthMapSRV.get();

    DepthOfFieldHelper::isMSAADepth = isMSAA;
    DepthOfFieldHelper::msaaSampleCount = desc.SampleDesc.Count;

    pDepthBuffer->Release();
}

SwapchainHook::SwapchainOriginal SwapchainHook::funcOriginal = nullptr;
bool SwapchainHook::recreate = true;
bool SwapchainHook::isDX12;
std::atomic<bool> SwapchainHook::imguiCleanupInProgress{false};
int SwapchainHook::dx12FrameCount = 0;

winrt::com_ptr<ID3D11Texture2D> SwapchainHook::SavedD3D11BackBuffer;
// Backbuffer storage system for MotionBlur
std::vector<SwapchainHook::BackbufferStorage> SwapchainHook::backbufferStorage;
std::vector<SwapchainHook::BackbufferStorage> SwapchainHook::backbufferStorageUnderUI;
int SwapchainHook::currentBackbufferIndex = 0;
int SwapchainHook::currentBackbufferIndexUnderUI = 0;
int SwapchainHook::maxBackbufferFrames = 0;
int SwapchainHook::validBackbufferFrames = 0;
int SwapchainHook::validBackbufferFramesUnderUI = 0;
winrt::com_ptr<ID3D11Texture2D> SwapchainHook::ExtraSavedD3D11BackBuffer;
std::mutex SwapchainHook::imguiInputMutex;
std::mutex SwapchainHook::backbufferMutex;
UINT SwapchainHook::lastBackbufferWidth = 0;
UINT SwapchainHook::lastBackbufferHeight = 0;

// Depth map storage system
winrt::com_ptr<ID3D11Texture2D> SwapchainHook::cachedDepthMapTexture;
winrt::com_ptr<ID3D11ShaderResourceView> SwapchainHook::cachedDepthMapSRV;
UINT SwapchainHook::lastDepthMapWidth = 0;
UINT SwapchainHook::lastDepthMapHeight = 0;
UINT SwapchainHook::lastDepthMapSampleCount = 0;
DXGI_FORMAT SwapchainHook::lastDepthMapFormat = DXGI_FORMAT_UNKNOWN;

std::vector<winrt::com_ptr<IDXGISurface1>> SwapchainHook::DXGISurfaces;
std::vector<winrt::com_ptr<ID2D1Bitmap1>> SwapchainHook::D2D1Bitmaps;
std::vector<winrt::com_ptr<ID3D11Resource>> SwapchainHook::D3D11Resources;
IDXGISwapChain3* SwapchainHook::swapchain;
winrt::com_ptr<ID3D12DescriptorHeap> SwapchainHook::D3D12DescriptorHeap;
winrt::com_ptr<ID3D11On12Device> SwapchainHook::d3d11On12Device;
winrt::com_ptr<ID2D1Bitmap1> SwapchainHook::D2D1Bitmap;
winrt::com_ptr<IDXGISurface1> SwapchainHook::backBuffer;

winrt::com_ptr<ID3D11DeviceContext> SwapchainHook::context;
winrt::com_ptr<ID3D11Device> SwapchainHook::d3d11Device;
uintptr_t SwapchainHook::bufferCount;
winrt::com_ptr<ID3D11Texture2D> SwapchainHook::stageTex;

winrt::com_ptr<ID3D12Device5> SwapchainHook::d3d12Device5 = nullptr;

winrt::com_ptr<ID3D12DescriptorHeap> SwapchainHook::d3d12DescriptorHeapImGuiRender = nullptr;
winrt::com_ptr<ID3D12DescriptorHeap> SwapchainHook::d3d12DescriptorHeapBackBuffers = nullptr;
winrt::com_ptr<ID3D12GraphicsCommandList> SwapchainHook::d3d12CommandList = nullptr;
winrt::com_ptr<ID3D12CommandQueue> SwapchainHook::d3d12CommandQueue = nullptr;
winrt::com_ptr<ID3D12CommandAllocator> SwapchainHook::allocator = nullptr;

UINT SwapchainHook::nextAvailableDescriptorIndex = SwapchainHook::IMGUI_FONT_DESCRIPTORS;
std::mutex SwapchainHook::descriptorAllocationMutex;

uint64_t SwapchainHook::buffersCounts = 0;
std::vector<FrameContext> SwapchainHook::frameContexts = {};

std::mutex SwapchainHook::frameTransformsMtx;
std::queue<FrameTransform> SwapchainHook::FrameTransforms;
int SwapchainHook::transformDelay = 3;

UINT SwapchainHook::flagsreal;

winrt::com_ptr<ID3D11RenderTargetView> SwapchainHook::cachedDX11RTV = nullptr;

std::vector<winrt::com_ptr<ID3D11RenderTargetView>> SwapchainHook::cachedDX12RTVs;
winrt::com_ptr<ID3D12Fence> SwapchainHook::cachedDX12Fence = nullptr;
UINT64 SwapchainHook::cachedDX12FenceValue = 0;

bool SwapchainHook::hasResized;

// End of SwapchainHook.cpp
