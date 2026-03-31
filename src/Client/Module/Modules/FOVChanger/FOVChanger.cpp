#include "FOVChanger.hpp"
#include "PaniniProjectionHelper.hpp"
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"

void FOVChanger::onEnable() {
    Listen(this, FOVEvent, &FOVChanger::onGetFOV)
    ListenOrdered(this, RenderEvent, &FOVChanger::onRender, EventOrder::IMMEDIATE)
    ListenOrdered(this, RenderUnderUIEvent, &FOVChanger::onRenderUnderUI, EventOrder::IMMEDIATE)
    Module::onEnable();
}

void FOVChanger::onDisable() {
    Deafen(this, FOVEvent, &FOVChanger::onGetFOV)
    Deafen(this, RenderEvent, &FOVChanger::onRender)
    Deafen(this, RenderUnderUIEvent, &FOVChanger::onRenderUnderUI)
    Module::onDisable();
}

void FOVChanger::defaultConfig() {
    Module::defaultConfig("core");
    setDef("fovvalue", 60.00f);
    setDef("fovaffectshand", false);
    setDef("panini", false);
    setDef("paniniStrength", 0.5f);
    setDef("paniniCrop", 1.0f);
    setDef("paniniUnderUI", false);
}

void FOVChanger::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSlider("FOV Value", "", "fovvalue", 359.0f, 0, false);
    addToggle("Affect Hand Size", "Keep normal hand size or not.", "fovaffectshand");
    addHeader("Panini Projection");
    addToggle("Enable Panini Projection",
        "Reduces edge distortion at high FOV by using\ncylindrical perspective instead of rectilinear.",
        "panini");

    if (getOps<bool>("panini")) {
        addSlider("Strength", "How much cylindrical compression to apply.\n0 = rectilinear, 1 = full Panini.", "paniniStrength", 1.0f, 0, false);
        addSlider("Crop to Fit", "Zoom in to prevent black borders at edges.\n1 = no borders, 0 = no zoom.", "paniniCrop", 1.0f, 0, false);
        addToggle("Render Under UI",
            "Apply Panini before the game draws its UI, so hotbar\nand chat stay undistorted. DX11 only (requires Better Frames on DX12).",
            "paniniUnderUI");
    }

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void FOVChanger::onGetFOV(FOVEvent &event) {
    if (!this->isEnabled()) return;
    if (auto zoom = ModuleManager::getModule("Zoom"); zoom && zoom->active) return;
    if (!getOps<bool>("fovaffectshand")) {
        if (event.getFOV() == 70) return;
    }

    bool inserver;

    std::string serverIP = SDK::getServerIP();

    if (serverIP.find("world") != std::string::npos) inserver = true;
    else inserver = false;

    auto fovSetting = getOps<float>("fovvalue");

    if (inserver) {
        if (fovSetting > 150) {
            if (!notified150Fov) {
                FlarialGUI::Notify("FOV Changer has been limmited to 150 on servers.");
                notified150Fov = true;
            }
            event.setFOV(150.0f);
        }
    }
    event.setFOV(fovSetting);
}

void FOVChanger::onRender(RenderEvent& event) {
    // On DX12, RenderUnderUIEvent doesn't fire (mid-frame D3D12 resource state
    // management is not viable), so always render at Present time.
    if (getOps<bool>("paniniUnderUI") && !SwapchainHook::isDX12) return;
    renderPanini(event.RTV);
}

void FOVChanger::onRenderUnderUI(RenderUnderUIEvent& event) {
    if (!getOps<bool>("paniniUnderUI")) return;
    renderPanini(event.RTV);
}

void FOVChanger::renderPanini(ID3D11RenderTargetView* rtv) {
    if (!getOps<bool>("panini")) return;
    if (!rtv) return;

    float d = getOps<float>("paniniStrength");
    if (d <= 0.001f) return;

    if (!PaniniProjectionHelper::initialized) {
        if (!PaniniProjectionHelper::Initialize())
            return;
    }

    winrt::com_ptr<ID3D11Texture2D> backbuffer;
    {
        std::lock_guard<std::mutex> lock(SwapchainHook::backbufferMutex);
        backbuffer = SwapchainHook::SavedD3D11BackBuffer;
    }
    if (!backbuffer) return;

    auto* sceneSRV = PaniniProjectionHelper::CopyBackbufferToSRV(backbuffer.get());
    if (!sceneSRV) return;

    float fov = getOps<float>("fovvalue");
    float cropToFit = getOps<float>("paniniCrop");

    PaniniProjectionHelper::Render(rtv, sceneSRV, fov, d, cropToFit);
}
