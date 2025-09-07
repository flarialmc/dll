#include "SwapchainHook.hpp"
#include <winrt/base.h>
#include <chrono>
#include "../../../../../GUI/D2D.hpp"
#include "d2d1.h"
#include "../../../../../Client.hpp"
#include <algorithm>
#include <windows.h>
#include <unknwn.h>
#include <iostream>
#include <Psapi.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_dx12.h>
#include "unknwnbase.h"
#include "UnderUIHooks.hpp"
#include "CreateSwapchainForCoreWindowHook.hpp"
#include "ResizeHook.hpp"
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

bool containsModule(const std::wstring &moduleName) {
    HANDLE hProcess = GetCurrentProcess();
    HMODULE hMods[1024];
    DWORD cbNeeded;

    if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];

            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                std::wstring baseModuleName = std::filesystem::path(szModName).filename().wstring();
                if (moduleName == baseModuleName) {
                    return true;
                }
            }
        }
    }

    return false;
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

    if (isRTSS) {
        Logger::debug("[Swapchain] MSI Afterburner failed to unload!");
        Utils::MessageDialogW(L"Flarial: client failed to initialize, disable MSI Afterburner or RTSS!", L"Error!");
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

    if (recreate) {
        init = false;
        initImgui = false;
        Logger::debug("[DEBUG] Recreating Swapchain");

        // CRITICAL: Force GPU to complete all pending operations before cleanup
        if (isDX12 && d3d12Device5 && queue) {
            winrt::com_ptr<ID3D12Fence> fence;
            if (SUCCEEDED(d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.put())))) {
                const UINT64 value = 1000;
                HANDLE evt = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                if (evt) {
                    queue->Signal(fence.get(), value);
                    if (fence->GetCompletedValue() < value) {
                        fence->SetEventOnCompletion(value, evt);
                        WaitForSingleObject(evt, 5000); // Extended timeout for recreation
                    }
                    CloseHandle(evt);
                }
            }
        } else if (!isDX12 && context.get()) {
            // For DX11, ensure all deferred operations are complete
            context->ClearState();
            context->Flush();
            Sleep(50); // Small delay to ensure flush completes
        }

        ResizeHook::cleanShit(true);
        recreate = false;
        swapchain = nullptr;

        // Additional safety delay before returning device reset
        Sleep(100);

        return DXGI_ERROR_DEVICE_RESET;
    }

    if(!swapchain) swapchain.copy_from(pSwapChain);
    if (D2D::context) MC::windowSize = Vec2(D2D::context->GetSize().width, D2D::context->GetSize().height);


    static bool hooker = false;

    if (!hooker && ((queue) || (!queue && context))) {
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

        FlarialGUI::PrepareBlur(blurIntensity);

        D2D1_IMAGE_BRUSH_PROPERTIES props = D2D1::ImageBrushProperties(
                D2D1::RectF(0, 0, MC::windowSize.x, MC::windowSize.y));
        D2D::context->CreateImageBrush(FlarialGUI::blur_bitmap_cache, props, &FlarialGUI::blurbrush);
        }
}


void SwapchainHook::Fonts() {
    if (ImGui::GetCurrentContext()) {
        static bool fontRebuildQueued = false;

        if (FlarialGUI::DoLoadFontLater) {
            FontKey fontK = FlarialGUI::LoadFontLater;
            if (Client::settings.getSettingByName<bool>("overrideFontWeight")->value) {
                fontK.weight = FlarialGUI::GetFontWeightFromString(Client::settings.getSettingByName<std::string>("fontWeight")->value);
            }
            
            if (!FlarialGUI::FontMap[fontK]) {
                FlarialGUI::LoadFontFromFontFamily(fontK);
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

void SwapchainHook::SaveBackbuffer(bool underui) {

    SavedD3D11BackBuffer = nullptr;
    ExtraSavedD3D11BackBuffer = nullptr;
    if (!isDX12) {

        SwapchainHook::swapchain->GetBuffer(0, IID_PPV_ARGS(SavedD3D11BackBuffer.put()));

        if (FlarialGUI::needsBackBuffer) {

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


    }
    else {
        {
            winrt::com_ptr<ID3D11Texture2D> tempBackBuffer;
            HRESULT hr = D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(tempBackBuffer.put()));
            if (SUCCEEDED(hr)) {
                SavedD3D11BackBuffer = tempBackBuffer;
            } else {
                std::cout << "Failed to query interface: " << std::hex << hr << std::endl;
            }
        }
       }
}

// End of SwapchainHook.cpp
