#include "SwapchainHook.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../Events/Render/RenderEvent.hpp"
#include "../../../Events/EventHandler.hpp"
#include "d2d1.h"
#include "../../../Client.hpp"
#include "../../../Module/Modules/CPS/CPSListener.hpp"
#include <d3d11on12.h>
#include <algorithm>
#include <windows.h>
#include <iostream>
#include <Psapi.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include "../../../../../lib/ImGui/imgui.h"
#include "../../../../../lib/ImGui/imgui_impl_win32.h"
#include "../../../../../lib/ImGui/imgui_impl_dx11.h"
#include "../../../../../lib/ImGui/imgui_impl_dx12.h"

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

ID3D12CommandQueue *SwapchainHook::queue = nullptr;

bool initImgui = false;
bool allfontloaded = false;
bool first = false;

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
static std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

auto window = (HWND)FindWindowA(nullptr, (LPCSTR)"Minecraft");

int SwapchainHook::currentBitmap;
bool unloadDll(const wchar_t* moduleName) {
    HMODULE hModule = GetModuleHandleW(moduleName);
    if (hModule != nullptr) {
        if (FreeLibrary(hModule)) {
            Logger::debug("[UNLOAD DLL] DLL unloaded!");
            return true;
        } else {
            Logger::debug("[UNLOAD DLL] Failed to FreeLibrary!");
            return false;
        }
    } else {
        Logger::debug("[UNLOAD DLL] Failed to unload!");
        return false;
    }
}

bool containsModule(const std::wstring& moduleName) {
    // Get the handle to the current process
    HANDLE hProcess = GetCurrentProcess();
    HMODULE hMods[1024];
    DWORD cbNeeded;

    // Enumerate modules in the process
    if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                std::wstring baseModuleName = std::filesystem::path(szModName).filename().wstring();
                // Compare the base module name with the given module name
                if (moduleName == baseModuleName) {
                    return true;
                }
            }
        }
    }

    return false;
}

void SwapchainHook::enableHook() {


    if (kiero::getRenderType() == kiero::RenderType::D3D12) {
        kiero::bind(140, (void**)&funcOriginal, (void*)swapchainCallback);
    }
    else if (kiero::getRenderType() == kiero::RenderType::D3D11) {
        kiero::bind(8, (void**)&funcOriginal, (void*)swapchainCallback);
    }

    // CREDIT @AETOPIA

    IDXGIFactory2 *pFactory = NULL;
    CreateDXGIFactory(IID_PPV_ARGS(&pFactory));
    Memory::hookFunc((*(LPVOID **)pFactory)[16], (void*) CreateSwapChainForCoreWindow, (void**) &IDXGIFactory2_CreateSwapChainForCoreWindow, "CreateSwapchainForCoreWindow");
    Memory::SafeRelease(pFactory);

    bool isRTSS = containsModule(L"RTSSHooks64.dll");

    if(isRTSS) {
        // if(!unloadDll(L"RTSSHooks64.dll")) { // causes a crash sometimes
        // Logger::debug("[Swapchain] MSI Afterburner failed to unload!");
        MessageBox(NULL, "Flarial: client failed to initialize, disable MSI Afterburner!", "", MB_OK);
        ModuleManager::terminate();
        Client::disable = true;
        // }
    }
}

bool SwapchainHook::init = false;


// CREDIT @AETOPIA

BOOL _ = FALSE, $ = FALSE, fEnabled = FALSE, fD3D11 = FALSE, MADECHAIN = FALSE;

HRESULT(*IDXGISwapChain_ResizeBuffers)
(IDXGISwapChain *This, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = NULL;

HRESULT (*IDXGISwapChain_Present)(IDXGISwapChain *, UINT, UINT) = NULL;

HRESULT(*SwapchainHook::IDXGIFactory2_CreateSwapChainForCoreWindow)
(IDXGIFactory2 *This, IUnknown *pDevice, IUnknown *pWindow, const DXGI_SWAP_CHAIN_DESC1 *pDesc,
 IDXGIOutput *pRestrictToOutput, IDXGISwapChain1 **ppSwapChain) = NULL;

HRESULT SwapchainHook::CreateSwapChainForCoreWindow(IDXGIFactory2 *This, IUnknown *pDevice, IUnknown *pWindow,
                                     DXGI_SWAP_CHAIN_DESC1 *pDesc, IDXGIOutput *pRestrictToOutput,
                                     IDXGISwapChain1 **ppSwapChain)

{

    if (!fEnabled)
        fEnabled = TRUE;

    pDesc->Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    MADECHAIN = TRUE;
    return IDXGIFactory2_CreateSwapChainForCoreWindow(This, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
}

// CREDIT @AETOPIA

HRESULT SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {

    if(!fEnabled) {
        return DXGI_ERROR_DEVICE_RESET;
    }

    if(!MADECHAIN && fEnabled) {
        return funcOriginal(pSwapChain, syncInterval, flags);
    }

    swapchain = pSwapChain;
    flagsreal = flags;

    if (Client::settings.getSettingByName<bool>("killdx")->value) {
        ID3D12Device5 *d3d12device3;

        if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d12device3)))) {
            Logger::debug("[SwapChain] Removed d3d12 device");
            pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            d3d12device3->RemoveDevice();

            return funcOriginal(pSwapChain, syncInterval, flags | DXGI_PRESENT_ALLOW_TEARING);
        }
    }

    std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - start;
    MC::frames += 1;


    if (elapsed.count() >= 0.5f) {
        // Calculate frame rate based on elapsed time
        MC::fps = static_cast<int>((float)MC::frames / elapsed.count());
        // Reset frame counter and update start time
        MC::frames = 0;
        start = std::chrono::high_resolution_clock::now();
    }

    constexpr float targetFrameRate = 60.0f;

// Measure the elapsed frame time
    std::chrono::duration<float> frameTime = std::chrono::high_resolution_clock::now() - previousFrameTime;
    previousFrameTime = std::chrono::high_resolution_clock::now();

// Calculate the current frame rate
    float currentFrameRate = 1.0f / frameTime.count();

// Calculate the frame factor as a percentage
    FlarialGUI::frameFactor = targetFrameRate / currentFrameRate;

// Limit the frame factor to a maximum of 1.0
    FlarialGUI::frameFactor = std::min(FlarialGUI::frameFactor, 1.0f);

    if (!init) {
        /* INIT START */

        if(queue == nullptr) {

            DX11Init();

        } else {

            DX12Init();

        }

        /* INIT END */
        /* RENDERING START */

    } else {
        /* IMPORTANT FONT STUFF */
        if (ImGui::GetCurrentContext()) {

            bool fontLoaded = false;

            if(FlarialGUI::DoLoadModuleFontLater) {
                if(Client::settings.getSettingByName<bool>("overrideFontWeight")->value) FlarialGUI::LoadModuleFontLaterWeight = FlarialGUI::GetFontWeightFromString(Client::settings.getSettingByName<std::string>("fontWeight")->value);

                std::string font1 = FlarialGUI::LoadModuleFontLater;
                std::transform(font1.begin(), font1.end(), font1.begin(), ::towlower);
                std::string weightedName = FlarialGUI::GetWeightedName(font1, FlarialGUI::LoadModuleFontLaterWeight);
                std::transform(weightedName.begin(), weightedName.end(), weightedName.begin(), ::towlower);
                if (!FlarialGUI::FontMap[weightedName + "-1"]) {
                    if (FlarialGUI::LoadFontFromFontFamily(font1, weightedName, FlarialGUI::LoadModuleFontLaterWeight)) {
                        fontLoaded = true;
                    }
                }

                FlarialGUI::DoLoadModuleFontLater = false;
            }

            if(FlarialGUI::DoLoadGUIFontLater) {

                if(Client::settings.getSettingByName<bool>("overrideFontWeight")->value) FlarialGUI::LoadGUIFontLaterWeight = FlarialGUI::GetFontWeightFromString(Client::settings.getSettingByName<std::string>("fontWeight")->value);


                std::string font2 = FlarialGUI::LoadGUIFontLater;
                std::transform(font2.begin(), font2.end(), font2.begin(), ::towlower);
                std::string weightedName = FlarialGUI::GetWeightedName(font2, FlarialGUI::LoadGUIFontLaterWeight);
                std::transform(weightedName.begin(), weightedName.end(), weightedName.begin(), ::towlower);
                if (!FlarialGUI::FontMap[weightedName + "-1"]) {
                    if (FlarialGUI::LoadFontFromFontFamily(font2, weightedName, FlarialGUI::LoadGUIFontLaterWeight)) {
                        fontLoaded = true;
                    }
                }

                FlarialGUI::DoLoadGUIFontLater = false;
            }

                //std::cout << FlarialGUI::WideToNarrow(FlarialGUI::GetFontFilePath(L"Dosis", DWRITE_FONT_WEIGHT_EXTRA_BLACK)).c_str() << std::endl;
            /*
            if(!allfontloaded) {
                FlarialGUI::LoadFonts(FlarialGUI::FontMap);

                allfontloaded = true;
            }
            */

            if (!FlarialGUI::FontMap["162-1"]) {
                ImFontConfig config;
                FlarialGUI::FontMap["162-1"] = ImGui::GetIO().Fonts->AddFontFromFileTTF((Utils::getRoamingPath() + "\\Flarial\\assets\\" + "162" + ".ttf").c_str(), 23, &config);
                FlarialGUI::FontMap["162-2.0"] = ImGui::GetIO().Fonts->AddFontFromFileTTF((Utils::getRoamingPath() + "\\Flarial\\assets\\" + "162" + ".ttf").c_str(), 40, &config);
                fontLoaded = true;
            }

            if (!FlarialGUI::FontMap["163-1"]) {
                ImFontConfig config;
                FlarialGUI::FontMap["163-1"] = ImGui::GetIO().Fonts->AddFontFromFileTTF((Utils::getRoamingPath() + "\\Flarial\\assets\\" + "163" + ".ttf").c_str(), 23, &config);
                FlarialGUI::FontMap["163-2.0"] = ImGui::GetIO().Fonts->AddFontFromFileTTF((Utils::getRoamingPath() + "\\Flarial\\assets\\" + "163" + ".ttf").c_str(), 40, &config);

                fontLoaded = true;
            }

            if (!FlarialGUI::FontMap["164-1"]) {
                FlarialGUI::FontMap["164-1"] = ImGui::GetIO().Fonts->AddFontFromFileTTF((Utils::getRoamingPath() + "\\Flarial\\assets\\" + "164" + ".ttf").c_str(), 23);
                FlarialGUI::FontMap["164-2.0"] = ImGui::GetIO().Fonts->AddFontFromFileTTF((Utils::getRoamingPath() + "\\Flarial\\assets\\" + "164" + ".ttf").c_str(), 40);

                fontLoaded = true;
            }

            if (fontLoaded) {
                ImGui::GetIO().Fonts->Build();
                if (!queue) {
                    ImGui_ImplDX11_InvalidateDeviceObjects();
                    ImGui_ImplDX11_CreateDeviceObjects();
                }
                else {
                    ImGui_ImplDX12_InvalidateDeviceObjects();
                    ImGui_ImplDX12_CreateDeviceObjects();
                }

            }
        }

        /* IMPORTANT FONT STUFF */

        /* RENDER SYNC */

        if(init && initImgui) {
            frameTransformsMtx.lock();
            while (FrameTransforms.size() > transformDelay) {
                MC::Transform = FrameTransforms.front();
                FrameTransforms.pop();
            }
            frameTransformsMtx.unlock();
        }

         /* RENDER SYNC */

        /* EACH FRAME STUFF */

        if (D2D::context != nullptr && !Client::disable) {

            if (queue != nullptr) {

                currentBitmap = (int)pSwapChain->GetCurrentBackBufferIndex();

                ID3D11Resource *resource = D3D11Resources[currentBitmap];
                d3d11On12Device->AcquireWrappedResources(&resource, 1);

                SaveBackbuffer();
                D2D::context->SetTarget(D2D1Bitmaps[currentBitmap]);

                DX12Blur();

                D2D::context->BeginDraw();

                MC::windowSize = Vec2<float>(D2D::context->GetSize().width, D2D::context->GetSize().height);

                DXGI_SWAP_CHAIN_DESC sdesc;
	            pSwapChain->GetDesc(&sdesc);
	            sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	            sdesc.OutputWindow = window;
	            sdesc.Windowed = ((GetWindowLongPtr(window, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

	            buffersCounts = sdesc.BufferCount;
	            frameContexts.resize(buffersCounts);

	            D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
	            descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	            descriptorImGuiRender.NumDescriptors = buffersCounts;
	            descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

                if (d3d12DescriptorHeapImGuiRender or SUCCEEDED(d3d12Device5->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&d3d12DescriptorHeapImGuiRender)))) {

                    if (!allocator) d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));

                        for (size_t i = 0; i < buffersCounts; i++) {
                            frameContexts[i].commandAllocator = allocator;
                        };

                        if(!d3d12CommandList) d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&d3d12CommandList));
                        if (d3d12CommandList) {

                            D3D12_DESCRIPTOR_HEAP_DESC descriptorBackBuffers;
                            descriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                            descriptorBackBuffers.NumDescriptors = buffersCounts;
                            descriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                            descriptorBackBuffers.NodeMask = 1;

                            if (SUCCEEDED(d3d12Device5->CreateDescriptorHeap(&descriptorBackBuffers, IID_PPV_ARGS(&d3d12DescriptorHeapBackBuffers)))) {

                                const auto rtvDescriptorSize = d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

                                for (size_t i = 0; i < buffersCounts; i++) {
                                    ID3D12Resource* pBackBuffer = nullptr;

                                    frameContexts[i].main_render_target_descriptor = rtvHandle;
                                    pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
                                    d3d12Device5->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
                                    frameContexts[i].main_render_target_resource = pBackBuffer;
                                    rtvHandle.ptr += rtvDescriptorSize;

                                    pBackBuffer->Release();
                                };

                                if (!initImgui) {
                                    initImgui = true;

                                    ImGui::CreateContext();

                                    ImGui_ImplWin32_Init(window);
                                    ImGui_ImplDX12_Init(d3d12Device5, buffersCounts,
                                        DXGI_FORMAT_R8G8B8A8_UNORM, d3d12DescriptorHeapImGuiRender,
                                        d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
                                        d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart());
                                }

                                ImGui_ImplDX12_NewFrame();
                                ImGui_ImplWin32_NewFrame();
                                ImGui::NewFrame();

                                ID3D11Texture2D* buffer2D = nullptr;
                                D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(&buffer2D));

                                ID3D11RenderTargetView* mainRenderTargetView;
                                d3d11Device->CreateRenderTargetView(buffer2D, NULL, &mainRenderTargetView);

                                RenderEvent event{};
                                event.RTV = mainRenderTargetView;
                                //BlurDX12::RenderBlur(SwapchainHook::d3d12CommandList);
                                EventHandler::onRender(event);

                                if(!first && SwapchainHook::init && ModuleManager::getModule("ClickGUI")) {
                                    FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                                 "keybind")->value + " to open the menu in-game.");
                                    FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
                                    first = true;
                                }


                                D2D::context->EndDraw();

                                D2D::context->SetTarget(nullptr);

                                d3d11On12Device->ReleaseWrappedResources(&resource, 1);

                                context->Flush();

                                frameContexts[pSwapChain->GetCurrentBackBufferIndex()].commandAllocator->Reset();;

                                D3D12_RESOURCE_BARRIER barrier;
                                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                                barrier.Transition.pResource = frameContexts[pSwapChain->GetCurrentBackBufferIndex()].main_render_target_resource;
                                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
                                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

                                d3d12CommandList->Reset(frameContexts[pSwapChain->GetCurrentBackBufferIndex()].commandAllocator, nullptr);
                                d3d12CommandList->ResourceBarrier(1, &barrier);
                                d3d12CommandList->OMSetRenderTargets(1, &frameContexts[pSwapChain->GetCurrentBackBufferIndex()].main_render_target_descriptor, FALSE, nullptr);
                                d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);

                                ImGui::EndFrame();
                                ImGui::Render();

                                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList);

                                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

                                d3d12CommandList->ResourceBarrier(1, &barrier);
                                d3d12CommandList->Close();

                                queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&d3d12CommandList));

                                Memory::SafeRelease(mainRenderTargetView);
                                Memory::SafeRelease(buffer2D);

                            }
                    }
                }

	            if (d3d12DescriptorHeapBackBuffers) {
		            d3d12DescriptorHeapBackBuffers->Release();
		            d3d12DescriptorHeapBackBuffers = nullptr;
	            }

                if (!frameContexts.empty() && frameContexts.front().commandAllocator != nullptr) {
                    frameContexts.front().commandAllocator = nullptr;
                }

	            if (!frameContexts.empty() && frameContexts.front().main_render_target_resource != nullptr) {
		            frameContexts.front().main_render_target_resource->Release();
		            frameContexts.front().main_render_target_resource = nullptr;
	            }

	            frameContexts.resize(0);

            } else {

                DX11Blur();

                SaveBackbuffer();

                D2D::context->BeginDraw();

                MC::windowSize = Vec2<float>(D2D::context->GetSize().width, D2D::context->GetSize().height);

                ID3D11RenderTargetView* mainRenderTargetView = nullptr;
	            ID3D11DeviceContext* ppContext = nullptr;
	            ID3D11Texture2D* pBackBuffer = nullptr;

	            d3d11Device->GetImmediateContext(&ppContext);

	            if (ppContext)

	            if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))

                    if (SUCCEEDED(d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView))) {

                        ImGui_ImplDX11_NewFrame();
                        ImGui_ImplWin32_NewFrame();
                        ImGui::NewFrame();

                        RenderEvent event;
                        event.RTV = mainRenderTargetView;
                        EventHandler::onRender(event);

                        if(!first && SwapchainHook::init && ModuleManager::getModule("ClickGUI")) {
                            FlarialGUI::Notify("Click " + ModuleManager::getModule("ClickGUI")->settings.getSettingByName<std::string>(
                         "keybind")->value + " to open the menu in-game.");

                            FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
                            first = true;
                        }

                        D2D::context->EndDraw();

                        ImGui::EndFrame();
                        ImGui::Render();

                        ppContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
                        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

                    }

                if (pBackBuffer) pBackBuffer->Release();

	            if (mainRenderTargetView) mainRenderTargetView->Release();

                Memory::SafeRelease(ppContext);

            }

            Memory::SafeRelease(FlarialGUI::blurbrush);
            Memory::SafeRelease(FlarialGUI::blur);
        }
    }

    /* EACH FRAME STUFF */

    try {
        if(init && initImgui && !FlarialGUI::hasLoadedAll) FlarialGUI::LoadAllImages();
    } catch (const std::exception &ex) { return 0; }

    if (Client::settings.getSettingByName<bool>("vsync")->value) {
        return funcOriginal(pSwapChain, 0, DXGI_PRESENT_ALLOW_TEARING);
    }

return funcOriginal(pSwapChain, syncInterval, flags);

}


void SwapchainHook::DX12Blur() {
    /* Blur Stuff */
    if(FlarialGUI::inMenu) FlarialGUI::needsBackBuffer = true;
    else FlarialGUI::needsBackBuffer = false;
    /* Blur End */
}

void SwapchainHook::DX11Blur() {
    /* Blur Stuff */

    if(FlarialGUI::inMenu || ModuleManager::getModule("Motion Blur")->active) FlarialGUI::needsBackBuffer = true;
    else FlarialGUI::needsBackBuffer = false;

    /* Blur End */
}

void SwapchainHook::DX11Init() {
    
    Logger::debug("[SwapChain] Not a DX12 device, running dx11 procedures");

    const D2D1_CREATION_PROPERTIES properties
    {
        D2D1_THREADING_MODE_MULTI_THREADED,
        D2D1_DEBUG_LEVEL_NONE,
        D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
};

    swapchain->GetDevice(IID_PPV_ARGS(&d3d11Device));

    IDXGISurface1 *eBackBuffer;
    swapchain->GetBuffer(0, IID_PPV_ARGS(&eBackBuffer));

    D2D1CreateDeviceContext(eBackBuffer, properties, &D2D::context);

    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96.0, 96.0);
    D2D::context->CreateBitmapFromDxgiSurface(eBackBuffer, props, &D2D1Bitmap);
    //ImGui Init

    if(!initImgui) {
        ImGui::CreateContext();

        d3d11Device->GetImmediateContext(&context);
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(d3d11Device, context);
        initImgui = true;

    }

    SaveBackbuffer();

    Blur::InitializePipeline();
    Memory::SafeRelease(eBackBuffer);
    init = true;
}


void SwapchainHook::DX12Init() {


                ID3D12Device5 *device;
            swapchain->GetDevice(IID_PPV_ARGS(&d3d12Device5));

            if (SUCCEEDED(swapchain->GetDevice(IID_PPV_ARGS(&device))) &&
                kiero::getRenderType() == kiero::RenderType::D3D12) {
                D3D11On12CreateDevice(device,
                                      D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                      (IUnknown **) &queue, 1, 0, &SwapchainHook::d3d11Device, &context,
                                      nullptr);

                d3d11Device->QueryInterface(IID_PPV_ARGS(&d3d11On12Device));

                D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
                ID2D1Factory7 *d2dFactory;
                D2D1_FACTORY_OPTIONS factoryOptions{};
                D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory7), &factoryOptions,
                                  (void **) &d2dFactory);

                IDXGIDevice *dxgiDevice;
                d3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void **) &dxgiDevice);

                ID2D1Device6 *device2;
                d2dFactory->CreateDevice(dxgiDevice, &device2);

                device2->CreateDeviceContext(deviceOptions, &D2D::context);

                Logger::debug("[SwapChain] Prepared.");

                DXGI_SWAP_CHAIN_DESC1 swapChainDescription;
                swapchain->GetDesc1(&swapChainDescription);

                bufferCount = swapChainDescription.BufferCount;

                DXGISurfaces.resize(bufferCount, nullptr);
                D3D11Resources.resize(bufferCount, nullptr);
                D2D1Bitmaps.resize(bufferCount, nullptr);

                auto dpi = (float) GetDpiForSystem();

                D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
                        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

                D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorBackBuffers = {};
                heapDescriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
                heapDescriptorBackBuffers.NumDescriptors = bufferCount;
                heapDescriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
                heapDescriptorBackBuffers.NodeMask = 1;

                device->CreateDescriptorHeap(&heapDescriptorBackBuffers,
                                             IID_PPV_ARGS(&D3D12DescriptorHeap));

                uintptr_t rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

                for (int i = 0; i < bufferCount; i++) {

                    ID3D12Resource *backBufferPtr;
                    swapchain->GetBuffer(i, IID_PPV_ARGS(&backBufferPtr));
                    device->CreateRenderTargetView(backBufferPtr, nullptr, rtvHandle);
                    rtvHandle.ptr += rtvDescriptorSize;


                    D3D11_RESOURCE_FLAGS d3d11_flags = {D3D11_BIND_RENDER_TARGET};

                    d3d11On12Device->CreateWrappedResource(backBufferPtr, &d3d11_flags,
                                                                          D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                                          D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(
                                                                                  &D3D11Resources[i]));
                    D3D11Resources[i]->QueryInterface(&DXGISurfaces[i]);

                    D2D1Bitmaps[i] = nullptr; // Initialize to nullptr

                    D2D::context->CreateBitmapFromDxgiSurface(DXGISurfaces[i], props,
                                                              &(D2D1Bitmaps[i]));
                    Memory::SafeRelease(backBufferPtr);

                }

                Memory::SafeRelease(device);
                Memory::SafeRelease(device2);
                Memory::SafeRelease(dxgiDevice);
                Memory::SafeRelease(d2dFactory);

                Blur::InitializePipeline();
                init = true;
            }
}

ID3D11Texture2D* SwapchainHook::GetBackbuffer()
{
    return SavedD3D11BackBuffer;
}

  void SwapchainHook::SaveBackbuffer() {

    if(!FlarialGUI::needsBackBuffer) return;

    if(!SwapchainHook::queue) {

        Memory::SafeRelease(SavedD3D11BackBuffer);

        ID3D11DeviceContext* deviceContext = SwapchainHook::context;
        IDXGISurface1* backBuffer = nullptr;
        HRESULT hr;

        hr = SwapchainHook::swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (FAILED(hr)) {
            return; // Early exit on failure
        }

        ID3D11Texture2D* buffer2D = nullptr;
        if (FAILED(backBuffer->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer2D)))) {
            Memory::SafeRelease(backBuffer);
            return; // Early exit on failure
        }

        D3D11_TEXTURE2D_DESC desc;
        buffer2D->GetDesc(&desc);

        if (!stageTex) {
            D3D11_TEXTURE2D_DESC stageDesc = desc;
            stageDesc.Usage = D3D11_USAGE_STAGING;
            stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            stageDesc.BindFlags = 0;

            hr = SwapchainHook::d3d11Device->CreateTexture2D(&stageDesc, nullptr, &stageTex);
            if (FAILED(hr)) {
                Memory::SafeRelease(backBuffer);
                Memory::SafeRelease(buffer2D);
                return;
            }
        }

        deviceContext->CopySubresourceRegion(stageTex, 0, 0, 0, 0, buffer2D, 0, nullptr);

        if (!SavedD3D11BackBuffer) {
            D3D11_TEXTURE2D_DESC defaultDesc = desc;
            defaultDesc.Usage = D3D11_USAGE_DEFAULT;
            defaultDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            defaultDesc.CPUAccessFlags = 0;

            hr = SwapchainHook::d3d11Device->CreateTexture2D(&defaultDesc, nullptr, &SavedD3D11BackBuffer);
            if (FAILED(hr)) {
                Memory::SafeRelease(backBuffer);
                Memory::SafeRelease(buffer2D);
                return;
            }
        }

        deviceContext->CopySubresourceRegion(SavedD3D11BackBuffer, 0, 0, 0, 0, stageTex, 0, nullptr);

        Memory::SafeRelease(backBuffer);
        Memory::SafeRelease(buffer2D);

    } else {


        ID3D11Texture2D* buffer2D = nullptr;
        HRESULT hr;
        hr = D3D11Resources[currentBitmap]->QueryInterface(IID_PPV_ARGS(&buffer2D));
        if (FAILED(hr))  std::cout << "Failed to query interface: " << std::hex << hr << std::endl;

        ID3D11DeviceContext* deviceContext = context;

        D3D11_TEXTURE2D_DESC desc;
        buffer2D->GetDesc(&desc);
        HRESULT r;

        if(!stageTex) {
            D3D11_TEXTURE2D_DESC stageDesc = {};
            stageDesc = desc;
            stageDesc.Usage = D3D11_USAGE_STAGING;
            stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
            stageDesc.BindFlags = 0;
            stageDesc.MiscFlags = 0;
            r = SwapchainHook::d3d11Device->CreateTexture2D(&stageDesc, nullptr, &stageTex);
            if (FAILED(r))  std::cout << "Failed to create stage texture: " << std::hex << r << std::endl;
        }
        deviceContext->CopyResource(stageTex, buffer2D);

        D3D11_TEXTURE2D_DESC defaultDesc = desc;
        defaultDesc.Usage = D3D11_USAGE_DEFAULT;
        defaultDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        defaultDesc.CPUAccessFlags = 0;

        if(!SavedD3D11BackBuffer) {
            hr = SwapchainHook::d3d11Device->CreateTexture2D(&defaultDesc, nullptr, &SavedD3D11BackBuffer);
            if (FAILED(hr)) {
                std::cout << "Failed to create def texture: " << std::hex << r << std::endl;
            }
        }

        deviceContext->CopyResource(SavedD3D11BackBuffer, stageTex);
        Memory::SafeRelease(buffer2D);
    }
}
