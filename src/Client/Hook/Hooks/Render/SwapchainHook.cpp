#include "SwapchainHook.hpp"
#include "../../../GUI/Data.hpp"
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

SwapchainHook::SwapchainHook() : Hook("swapchain_hook", "") {}

ID3D12CommandQueue *SwapchainHook::queue = nullptr;

bool initImgui = false;

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
static std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

auto window = (HWND)FindWindowA(nullptr, (LPCSTR)"Minecraft");;

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

// main stuff

HRESULT SwapchainHook::swapchainCallback(IDXGISwapChain3 *pSwapChain, UINT syncInterval, UINT flags) {


    //init
    if(init) {
        if (!Client::settings.getSettingByName<bool>("killdx")->value) {

            if(queue == nullptr) InitDX11();
            else InitDX12;

        } else {

            // Kill DX
            ID3D12Device5 *d3d12device3;
            if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d12device3)))) {
                Logger::debug("[SwapChain] Removed d3d12 device");
                pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
                d3d12device3->RemoveDevice();

                return funcOriginal(pSwapChain, syncInterval, flags);
            }

            if(queue == nullptr) InitDX11();
            else InitDX12;

        }
    }


    if (Client::settings.getSettingByName<bool>("vsync")->value) {
        return funcOriginal(pSwapChain, 0, DXGI_PRESENT_DO_NOT_WAIT);
    }

    return funcOriginal(pSwapChain, syncInterval, flags);
}


// other stuff

void SwapchainHook::InitDX12(IDXGISwapChain3* swapchain) {

                DXGI_SWAP_CHAIN_DESC sdesc;
	            swapchain->GetDesc(&sdesc);
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

                    if (SUCCEEDED(d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)))) {

                        for (size_t i = 0; i < buffersCounts; i++) {
                            frameContexts[i].commandAllocator = allocator;
                        };

                        if (SUCCEEDED(d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&d3d12CommandList)))) {

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
                                    swapchain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
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
                            }
                        }
                    }
                }

	            if (allocator) {
		            allocator->Release();
		            allocator = nullptr;
	            }

	            if (d3d12CommandList) {
		            d3d12CommandList->Release();
		            d3d12CommandList = nullptr;
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

}

void SwapchainHook::InitDX11() {

    ImGui::CreateContext();

    ID3D11DeviceContext* ppContext = nullptr;
    d3d11Device->GetImmediateContext(&ppContext);
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(d3d11Device, ppContext);
    ppContext->Release();
}

void SwapchainHook::RenderDX12() {
                                ImGui_ImplDX12_NewFrame();
                                ImGui_ImplWin32_NewFrame();
                                ImGui::NewFrame();

                                ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(ImVec2(), ImVec2(100, 100), ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 255), ImColor(100, 100, 100, 255), ImColor(0, 255, 0, 255));

                                RenderEvent event{};
                                EventHandler::onRender(event);

                                frameContexts[swapchain->GetCurrentBackBufferIndex()].commandAllocator->Reset();;

                                D3D12_RESOURCE_BARRIER barrier;
                                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                                barrier.Transition.pResource = frameContexts[swapchain->GetCurrentBackBufferIndex()].main_render_target_resource;
                                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
                                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

                                d3d12CommandList->Reset(frameContexts[swapchain->GetCurrentBackBufferIndex()].commandAllocator, nullptr);
                                d3d12CommandList->ResourceBarrier(1, &barrier);
                                d3d12CommandList->OMSetRenderTargets(1, &frameContexts[swapchain->GetCurrentBackBufferIndex()].main_render_target_descriptor, FALSE, nullptr);
                                d3d12CommandList->SetDescriptorHeaps(1, &d3d12DescriptorHeapImGuiRender);

                                ImGui::EndFrame();
                                ImGui::Render();

                                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12CommandList);

                                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

                                d3d12CommandList->ResourceBarrier(1, &barrier);
                                d3d12CommandList->Close();

                                queue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&d3d12CommandList));
}

void RenderDX11(IDXGISwapChain3* swapchain) {

    auto window = FindWindowA(nullptr, "Minecraft");


    ID3D11RenderTargetView* mainRenderTargetView = nullptr;
    ID3D11DeviceContext* ppContext = nullptr;
    ID3D11Texture2D* pBackBuffer = nullptr;


    SwapchainHook::d3d11Device->GetImmediateContext(&ppContext);

    if (ppContext)

        if (SUCCEEDED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))

            if (SUCCEEDED(SwapchainHook::d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView))) {

                ImGui_ImplDX11_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();

                ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(ImVec2(), ImVec2(100, 100), ImColor(255, 255, 255, 255), ImColor(0, 0, 0, 255), ImColor(100, 100, 100, 255), ImColor(0, 255, 0, 255));

                RenderEvent event;
                EventHandler::onRender(event);

                ImGui::EndFrame();
                ImGui::Render();

                ppContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            }

    if (pBackBuffer) pBackBuffer->Release();

    if (mainRenderTargetView) mainRenderTargetView->Release();

    if (ppContext) ppContext->Release();
}