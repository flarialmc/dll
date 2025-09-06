#include "../DXGI/SwapchainHook.hpp"
#include <winrt/base.h>
#include "../../../../../GUI/D2D.hpp"
#include "../../../../../Events/Render/RenderEvent.hpp"
#include "../../../../../Events/Render/RenderUnderUIEvent.hpp"
#include "d2d1.h"
#include "../../../../../Client.hpp"
#include <d3d11on12.h>
#include <algorithm>
#include <codecvt>
#include <windows.h>
#include <unknwn.h>
#include <iostream>
#include <Psapi.h>
#include <tlhelp32.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include "unknwnbase.h"
#include "../DXGI/UnderUIHooks.hpp"
#include "../DXGI/ResizeHook.hpp"
#include "Modules/MotionBlur/MotionBlur.hpp"
using ::IUnknown;

void SwapchainHook::DX11Init() {
    Logger::debug("Initializing for DX11");
    if (!swapchain) {
        Logger::error("Swapchain is null");
        return;
    }

    if (FAILED(swapchain->GetDevice(IID_PPV_ARGS(d3d11Device.put())))) {
        Logger::error("Failed to get D3D11 device from swapchain");
        return;
    }

    d3d11Device->GetImmediateContext(context.put());
    if (!context) {
        Logger::error("Failed to get D3D11 immediate context");
        return;
    }

    const D2D1_CREATION_PROPERTIES properties = {
        D2D1_THREADING_MODE_MULTI_THREADED,
        D2D1_DEBUG_LEVEL_NONE,
        D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS
    };

    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(backBuffer.put())))) {
        Logger::error("Failed to get backbuffer");
        return;
    }

    HRESULT hr = D2D1CreateDeviceContext(backBuffer.get(), properties, D2D::context.put());
    if (FAILED(hr) || !D2D::context) {
        Logger::error("Failed to create D2D1 device context: {}", Logger::getHRESULTError(hr));
        backBuffer = nullptr;
        return;
    }

    D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f
    );

    D2D::context->CreateBitmapFromDxgiSurface(backBuffer.get(), bitmapProps, D2D1Bitmap.put());

    if (!initImgui && !imguiCleanupInProgress) {

        while (imguiCleanupInProgress) {
            Sleep(1);
        }

        if (!ImGui::GetCurrentContext()) {
            ImGui::CreateContext();
        }

        auto& io = ImGui::GetIO();
        if (!io.BackendPlatformUserData) {
            ImGui_ImplWin32_Init(window2);
        }

        if (!io.BackendRendererUserData) {
            ImGui_ImplDX11_Init(d3d11Device.get(), context.get());
        }

        initImgui = true;
    }

    Blur::InitializePipeline();
    MotionBlur::initted = AvgPixelMotionBlurHelper::Initialize() && RealMotionBlurHelper::Initialize();

    SaveBackbuffer();
    init = true;
}

void SwapchainHook::DX11Render(bool underui) {
    if (!D2D::context || !context) return;

    DX11Blur();

    SaveBackbuffer(underui);

    static UINT lastBufferWidth = 0, lastBufferHeight = 0;

    winrt::com_ptr<ID3D11Texture2D> backBuffer;
    if (FAILED(swapchain->GetBuffer(0, IID_PPV_ARGS(backBuffer.put())))) {
        return;
    }

    D3D11_TEXTURE2D_DESC desc;
    backBuffer->GetDesc(&desc);

    if (!cachedDX11RTV.get() || desc.Width != lastBufferWidth || desc.Height != lastBufferHeight) {
        cachedDX11RTV = nullptr;
        if (FAILED(d3d11Device->CreateRenderTargetView(backBuffer.get(), nullptr, cachedDX11RTV.put()))) {
            return;
        }
        lastBufferWidth = desc.Width;
        lastBufferHeight = desc.Height;
    }

    ID3D11RenderTargetView* originalRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    ID3D11DepthStencilView* originalDSV = nullptr;
    context->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, originalRTVs, &originalDSV);

    D2D::context->BeginDraw();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("t", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDecoration);

    if (!underui) {
        auto event = nes::make_holder<RenderEvent>();
        event->RTV = cachedDX11RTV.get();
        eventMgr.trigger(event);
    } else {
        auto event = nes::make_holder<RenderUnderUIEvent>();
        event->RTV = cachedDX11RTV.get();
        eventMgr.trigger(event);
    }

    static bool notificationsShown = false;
    if (!notificationsShown && SwapchainHook::init) {
        if (auto clickGUI = ModuleManager::getModule("ClickGUI")) {
            FlarialGUI::Notify("Click " +
                clickGUI->settings.getSettingByName<std::string>("keybind")->value +
                " to open the menu in-game.");
            FlarialGUI::Notify("Join our discord! https://flarial.xyz/discord");
            notificationsShown = true;
        }
    }

    D2D::context->EndDraw();

    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();

    ID3D11RenderTargetView* rtvPtr = cachedDX11RTV.get();
    context->OMSetRenderTargets(1, &rtvPtr, originalDSV);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    bool allValid = true;
    for (auto & originalRTV : originalRTVs) {
        if (originalRTV == nullptr) {
            allValid = false;
        }
    }

    if (allValid)
    context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, originalRTVs, originalDSV);

    Memory::SafeRelease(originalDSV);
    for (auto & originalRTV : originalRTVs) {
        Memory::SafeRelease(originalRTV);
    }
}

void SwapchainHook::DX11Blur() {


    /* Blur Stuff */
    prepareBlur();
    if (ModuleManager::initialized) {
        auto module = ModuleManager::getModule("Motion Blur");
        if (module) {
            if (module->isEnabled() || FlarialGUI::inMenu) FlarialGUI::needsBackBuffer = true;
            else FlarialGUI::needsBackBuffer = false;
        }
    }
    /* Blur End */

}
