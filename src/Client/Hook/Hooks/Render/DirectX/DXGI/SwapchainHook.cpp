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
#include "CreateSwapchainForCoreWindowHook.hpp"
#include "ResizeHook.hpp"
#include "../../../../../Module/Modules/DepthOfField/DepthOfFieldHelper.hpp"
using ::IUnknown;

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", 0) {}

winrt::com_ptr<ID3D12CommandQueue> SwapchainHook::queue = nullptr;
HANDLE SwapchainHook::fenceEvent = nullptr;

bool SwapchainHook::initImgui = false;
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

    if (kiero::getRenderType() == kiero::RenderType::D3D12) {
        kiero::bind(140, (void **) &funcOriginal, (void *) swapchainCallback);
    } else if (kiero::getRenderType() == kiero::RenderType::D3D11) {
        kiero::bind(8, (void **) &funcOriginal, (void *) swapchainCallback);
    }

    winrt::com_ptr<IDXGIFactory2> pFactory;
    CreateDXGIFactory(IID_PPV_ARGS(pFactory.put()));
    if (!pFactory) LOG_ERROR("Factory not created");

    CreateSwapchainForCoreWindowHook::hook(pFactory.get());

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

bool SwapchainHook::init = false;
bool SwapchainHook::currentVsyncState;


HRESULT SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {
    if (Client::disable || !Client::init) return funcOriginal(pSwapChain, syncInterval, flags);

    isDX12 = GraphicsAPI::D3D12 == DetectSwapchainAPI(pSwapChain);

    if (currentVsyncState != Client::settings.getSettingByName<bool>("vsync")->value) {
        recreate = true;
    }
    //if (containsModule(L"medal-hook64.dll")) recreate = false;

    if (recreate) {
        init = false;
        initImgui = false;
        Logger::debug("[DEBUG] Recreating Swapchain");
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
}

winrt::com_ptr<ID3D11ShaderResourceView> SwapchainHook::GetCurrentBackbufferSRV(bool underUI) {

    if (underUI) {
        if (backbufferStorageUnderUI.empty()) {
            return nullptr;
        }
        int prevIndex = (currentBackbufferIndexUnderUI - 1 + backbufferStorageUnderUI.size()) % backbufferStorageUnderUI.size();
        return backbufferStorageUnderUI[prevIndex].srv;
    } else {
        if (backbufferStorage.empty()) {
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

                if (UnderUIHooks::bgfxCtx->m_msaart) {
                    context->ResolveSubresource(currentStorage.texture.get(), 0, UnderUIHooks::bgfxCtx->m_msaart, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                } else {
                    context->CopyResource(currentStorage.texture.get(), SavedD3D11BackBuffer.get());
                }

                currentBackbufferIndexUnderUI = (currentBackbufferIndexUnderUI + 1) % backbufferStorageUnderUI.size();
            } else {

                auto& currentStorage = backbufferStorage[currentBackbufferIndex];
                context->CopyResource(currentStorage.texture.get(), SavedD3D11BackBuffer.get());
                currentBackbufferIndex = (currentBackbufferIndex + 1) % backbufferStorage.size();
            }

        } else if (FlarialGUI::needsBackBuffer)
        {
            // Fallback to old behavior if storage not initialized
            ExtraSavedD3D11BackBuffer = nullptr;
            if (!ExtraSavedD3D11BackBuffer) {
                D3D11_TEXTURE2D_DESC textureDesc = {};
                SavedD3D11BackBuffer->GetDesc(&textureDesc);
                textureDesc.Usage = D3D11_USAGE_DEFAULT;
                textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                textureDesc.CPUAccessFlags = 0;

                SwapchainHook::d3d11Device->CreateTexture2D(&textureDesc, nullptr, ExtraSavedD3D11BackBuffer.put());
            }

            if (underui) {
                if (UnderUIHooks::bgfxCtx->m_msaart) {
                    context->ResolveSubresource(ExtraSavedD3D11BackBuffer.get(), 0, UnderUIHooks::bgfxCtx->m_msaart, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
                } else {
                    context->CopyResource(ExtraSavedD3D11BackBuffer.get(), SavedD3D11BackBuffer.get());
                }
            } else {
                context->CopyResource(ExtraSavedD3D11BackBuffer.get(), SavedD3D11BackBuffer.get());
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
    if (!pDepthStencilView || isDX12) return;

    auto depthOfFieldModule = ModuleManager::getModule("Depth Of Field");
    if (!depthOfFieldModule || !depthOfFieldModule->isEnabled()) {
        return;
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
winrt::com_ptr<ID3D11Texture2D> SwapchainHook::ExtraSavedD3D11BackBuffer;
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
