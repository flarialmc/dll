#include "MotionBlur.hpp"
#include "Client.hpp"
#include <algorithm>
#include <cmath>

static bool realMotionBlurInitialized = false;


void MotionBlur::onEnable() {
    if (SwapchainHook::isDX12) {
        if (!once) {
            FlarialGUI::Notify("Please turn on Better Frames in Settings!");
            onDisable();
            once = true;
        }
}

    ListenOrdered(this, RenderUnderUIEvent, &MotionBlur::onRender, EventOrder::IMMEDIATE)
    ListenOrdered(this, RenderEvent, &MotionBlur::onRenderNormal, EventOrder::IMMEDIATE)

    Module::onEnable();
}

void MotionBlur::onDisable() {
    Deafen(this, RenderUnderUIEvent, &MotionBlur::onRender)
    Deafen(this, RenderEvent, &MotionBlur::onRenderNormal)
    previousFrames.clear();
    frameTimestamps.clear();
    SwapchainHook::CleanupBackbufferStorage();
    RealMotionBlurHelper::Reset();
    VelocityBlurHelper::Reset();
    Module::onDisable();
}

void MotionBlur::defaultConfig() {
    Module::defaultConfig("core");
    setDef("intensity", 0.88f);
    setDef("intensity2", 6.0f);
    setDef("intensity_ghost", 6.0f);
    setDef("intensity_bleed", 6.0f);
    setDef("intensity_real", 6.0f);
    setDef("intensity_onix", 6.0f);
    setDef("intensity_velocity", 0.65f);
    setDef("blurType", (std::string)"Average Pixel Blur");
    setDef("dynamic", false);
    setDef("samples", 64.f);
    setDef("renderUnderUI", false);
    setDef("blurTimeConstant", 0.0667f);
    setDef("maxHistoryFrames", 8.0f);
}

void MotionBlur::settingsRender(float settingsOffset) {
    initSettingsPage();

    addToggle("Render Under UI", "When enabled, renders motion blur under the UI. When disabled, renders over the UI.", "renderUnderUI");

    addDropdown("Blur Type", "", std::vector<std::string>{
                    "Average Pixel Blur",
                    "Real Motion Blur",
                    "Ghost Frames",
                    "Time Aware Blur",
                    "V4",
                    "Velocity Blur"
                }, "blurType", true);

    addConditionalToggle(getOps<std::string>("blurType") == "Average Pixel Blur", "Dynamic Mode", "Automatically adjusts intensity according to FPS", "dynamic");
    addConditionalSlider(getOps<std::string>("blurType") == "Average Pixel Blur" && !getOps<bool>("dynamic"), "Intensity", "Amount of previous frames to render.", "intensity2", 30, 0, true);

    addConditionalSlider(getOps<std::string>("blurType") == "Ghost Frames", "Intensity", "Amount of previous frames to render.", "intensity_ghost", 30, 0, true);
    addConditionalSlider(getOps<std::string>("blurType") == "Ghost Frames", "Bleed Factor", "Amount of previous frames to render.", "intensity_bleed", 1, 0, true);

    addConditionalSlider(getOps<std::string>("blurType") == "Real Motion Blur", "Intensity", "Amount of previous frames to render.", "intensity_real", 30, 0, true);

    addConditionalSlider(getOps<std::string>("blurType") == "V4", "Intensity", "Amount of previous frames to render.", "intensity_onix", 30, 0, true);

    addConditionalSlider(getOps<std::string>("blurType") == "Time Aware Blur", "Blur Time Constant", "Higher = longer trails. 0.067 = Vegas 240fps->60fps look.", "blurTimeConstant", 0.2f, 0.01f, false);
    addConditionalSlider(getOps<std::string>("blurType") == "Time Aware Blur", "Max History Frames", "Performance limit. More frames = smoother but slower.", "maxHistoryFrames", 16, 4, true);

    addConditionalSlider(getOps<std::string>("blurType") == "Velocity Blur", "Intensity", "Strength of the velocity-based blur. Lower = cleaner.", "intensity_velocity", 2.0f, 0.05f, false);

    addConditionalSlider(getOps<std::string>("blurType") != "Ghost Frames" && getOps<std::string>("blurType") != "Time Aware Blur", "Samples", "", "samples", 256, 8, true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void MotionBlur::onRender(RenderUnderUIEvent &event) {
    if (!this->isEnabled()) return;
    if (SwapchainHook::isDX12) return;


    if (getOps<std::string>("blurType") == "Ghost Frames" || !getOps<bool>("renderUnderUI")) {
        return;
    }

    auto blurType = getOps<std::string>("blurType");
    int maxFrames;

    if (blurType == "Time Aware Blur") {
        maxFrames = (int) round(getOps<float>("maxHistoryFrames"));
    } else if (blurType == "V4") {
        maxFrames = (int) round(getOps<float>("intensity_onix"));
    } else if (blurType == "Velocity Blur") {
        maxFrames = 1;
    } else {
        maxFrames = (int) round(blurType == "Ghost Frames" ? getOps<float>("intensity_ghost") : blurType == "Real Motion Blur" ? getOps<float>("intensity_real") : getOps<float>("intensity2"));
    }

    if (getOps<bool>("dynamic") && blurType == "Average Pixel Blur") {
        if (MC::fps < 75) maxFrames = 1;
        else if (MC::fps < 100) maxFrames = 2;
        else if (MC::fps < 180) maxFrames = 3;
        else if (MC::fps > 300) maxFrames = 4;
        else if (MC::fps > 450) maxFrames = 5;
    }

    if (SDK::getCurrentScreen() == "hud_screen" && initted && this->isEnabled()) {
        if (previousFrames.size() > static_cast<size_t>(maxFrames)) {
            size_t toRemove = previousFrames.size() - maxFrames;
            previousFrames.erase(previousFrames.begin(), previousFrames.begin() + toRemove);
            frameTimestamps.erase(frameTimestamps.begin(), frameTimestamps.begin() + toRemove);
        }

        SwapchainHook::InitializeBackbufferStorage(maxFrames);

        float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

        auto buffer = BackbufferToSRVExtraMode(true);
        if (buffer) {
            previousFrames.push_back(std::move(buffer));
            frameTimestamps.push_back(currentTime);
        }

        if (blurType == "Real Motion Blur") {
            if (!realMotionBlurInitialized) {
                realMotionBlurInitialized = RealMotionBlurHelper::Initialize();
            }

            if (realMotionBlurInitialized && event.RTV) {
                auto sceneSRV = BackbufferToSRVExtraMode(true);

                if (sceneSRV) {
                    RealMotionBlurHelper::Render(event.RTV, sceneSRV);
                }
            }
        } else if (blurType == "Velocity Blur") {
            if (!velocityBlurInitialized) {
                velocityBlurInitialized = VelocityBlurHelper::Initialize();
            }

            if (velocityBlurInitialized && event.RTV) {
                auto sceneSRV = BackbufferToSRVExtraMode(true);

                if (sceneSRV) {
                    VelocityBlurHelper::Render(event.RTV, sceneSRV);
                }
            }
        } else if (blurType == "V4") {
            auto sceneSRV = BackbufferToSRVExtraMode(true);

            if (sceneSRV) {
                previousFrames.push_back(std::move(sceneSRV));
                frameTimestamps.push_back(currentTime);
            }

            float alpha = 0.35f;
            float bleedFactor = 0.85f;

            for (const auto &frame: previousFrames) {
                ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                alpha *= bleedFactor;
            }
        } else if (blurType == "Time Aware Blur") {
            float T = getOps<float>("blurTimeConstant");

            std::vector<float> weights;
            float totalWeight = 0.0f;

            for (size_t i = 0; i < previousFrames.size(); i++) {
                float age = currentTime - frameTimestamps[i];
                float weight = std::exp(-age / T);
                weights.push_back(weight);
                totalWeight += weight;
            }

            if (totalWeight > 0.0f) {
                for (float& w : weights) {
                    w /= totalWeight;
                }
            }

            for (size_t i = 0; i < previousFrames.size(); i++) {
                if (weights[i] > 0.001f) {
                    ImageWithOpacity(previousFrames[i], {MC::windowSize.x, MC::windowSize.y}, weights[i]);
                }
            }
        } else {
            float alpha = 0.3f;
            float bleedFactor = 0.8f;

            if (blurType == "Average Pixel Blur") {
                alpha = 0.25f;
                bleedFactor = 0.95f;
            }

            for (const auto &frame: previousFrames) {
                ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                alpha *= bleedFactor;
            }
        }
    } else {
        previousFrames.clear();
        frameTimestamps.clear();
    }
}

void MotionBlur::onRenderNormal(RenderEvent &event) {
    auto blurType = getOps<std::string>("blurType");
    auto renderUnderUI = getOps<bool>("renderUnderUI");

    if (!this->isEnabled() || ModuleManager::getModule("ClickGUI")->active) return;
    if (SwapchainHook::isDX12) return;

    if (blurType != "Ghost Frames" && renderUnderUI) {
        return;
    }

    int maxFrames;

    if (blurType == "Time Aware Blur") {
        maxFrames = (int) round(getOps<float>("maxHistoryFrames"));
    } else if (blurType == "V4") {
        maxFrames = (int) round(getOps<float>("intensity_onix"));
    } else if (blurType == "Velocity Blur") {
        maxFrames = 1;
    } else {
        maxFrames = (int) round(blurType == "Ghost Frames" ? getOps<float>("intensity_ghost") : blurType == "Real Motion Blur" ? getOps<float>("intensity_real") : getOps<float>("intensity2"));
    }

    if (getOps<bool>("dynamic") && blurType == "Average Pixel Blur") {
        if (MC::fps < 75) maxFrames = 1;
        else if (MC::fps < 100) maxFrames = 2;
        else if (MC::fps < 180) maxFrames = 3;
        else if (MC::fps > 300) maxFrames = 4;
        else if (MC::fps > 450) maxFrames = 5;
    }

    auto currentScreen = SDK::getCurrentScreen();
    if (currentScreen == "hud_screen" && initted && this->isEnabled()) {
        if (previousFrames.size() > static_cast<size_t>(maxFrames)) {
            size_t toRemove = previousFrames.size() - maxFrames;
            previousFrames.erase(previousFrames.begin(), previousFrames.begin() + toRemove);
            frameTimestamps.erase(frameTimestamps.begin(), frameTimestamps.begin() + toRemove);
        }


        SwapchainHook::InitializeBackbufferStorage(maxFrames);

        float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

        auto buffer = BackbufferToSRVExtraMode(false);
        if (buffer) {
            previousFrames.push_back(std::move(buffer));
            frameTimestamps.push_back(currentTime);
        }

        if (blurType == "Real Motion Blur") {
            if (!realMotionBlurInitialized) {
                realMotionBlurInitialized = RealMotionBlurHelper::Initialize();
            }

            if (realMotionBlurInitialized && event.RTV) {
                auto sceneSRV = BackbufferToSRVExtraMode(false);

                if (sceneSRV) {
                    RealMotionBlurHelper::Render(event.RTV, sceneSRV);
                }
            }
        } else if (blurType == "Velocity Blur") {
            if (!velocityBlurInitialized) {
                velocityBlurInitialized = VelocityBlurHelper::Initialize();
            }

            if (velocityBlurInitialized && event.RTV) {
                auto sceneSRV = BackbufferToSRVExtraMode(false);

                if (sceneSRV) {
                    VelocityBlurHelper::Render(event.RTV, sceneSRV);
                }
            }
        } else if (blurType == "V4") {
            auto sceneSRV = BackbufferToSRVExtraMode(false);

            if (sceneSRV) {
                previousFrames.push_back(std::move(sceneSRV));
                frameTimestamps.push_back(currentTime);
            }

            float alpha = 0.35f;
            float bleedFactor = 0.85f;

            for (const auto &frame: previousFrames) {
                ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                alpha *= bleedFactor;
            }
        } else if (blurType == "Time Aware Blur") {
            float T = getOps<float>("blurTimeConstant");

            std::vector<float> weights;
            float totalWeight = 0.0f;

            for (size_t i = 0; i < previousFrames.size(); i++) {
                float age = currentTime - frameTimestamps[i];
                float weight = std::exp(-age / T);
                weights.push_back(weight);
                totalWeight += weight;
            }

            if (totalWeight > 0.0f) {
                for (float& w : weights) {
                    w /= totalWeight;
                }
            }

            for (size_t i = 0; i < previousFrames.size(); i++) {
                if (weights[i] > 0.001f) {
                    ImageWithOpacity(previousFrames[i], {MC::windowSize.x, MC::windowSize.y}, weights[i]);
                }
            }
        } else {
            float alpha = 0.3f;
            float bleedFactor = 0.8f;

            if (blurType == "Average Pixel Blur") {
                alpha = 0.25f;
                bleedFactor = 0.95f;
            }

            for (const auto &frame: previousFrames) {
                ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                alpha *= bleedFactor;
            }
        }
    } else {
        previousFrames.clear();
        frameTimestamps.clear();
    }
}

void MotionBlur::ImageWithOpacity(const winrt::com_ptr<ID3D11ShaderResourceView> &srv, ImVec2 size, float opacity) {
    if (opacity <= 0.0f) {
        return;
    }

    opacity = opacity > 1.0f ? 1.0f : opacity < 0.0f ? 0.0f : opacity;
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    ImVec2 pos = {0, 0};
    ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(opacity * 255));
    draw_list->AddImage(ImTextureID(srv.get()), pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), col);
}

winrt::com_ptr<ID3D11ShaderResourceView> MotionBlur::BackbufferToSRVExtraMode(bool underui) {
    static int failLogCounter = 0;

    if (!FlarialGUI::needsBackBuffer) {
        return nullptr;
    }
    if (SwapchainHook::isDX12) return BackbufferToSRV(underui);

    auto srv = SwapchainHook::GetCurrentBackbufferSRV(underui);
    if (srv) {
        return srv;
    }

    winrt::com_ptr<ID3D11Texture2D> extraBuffer;
    {
        std::lock_guard<std::mutex> lock(SwapchainHook::backbufferMutex);
        extraBuffer = SwapchainHook::ExtraSavedD3D11BackBuffer;
    }
    if (!extraBuffer) {
        return nullptr;
    }

    if (!SwapchainHook::d3d11Device) {
        return nullptr;
    }

    HRESULT hr;
    D3D11_TEXTURE2D_DESC d;
    extraBuffer->GetDesc(&d);
    winrt::com_ptr<ID3D11ShaderResourceView> outSRV;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = d.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = d.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if (FAILED(hr = SwapchainHook::d3d11Device->CreateShaderResourceView(extraBuffer.get(), &srvDesc, outSRV.put()))) {
        std::cout << "Failed to create shader resource view: " << std::hex << hr << std::endl;
    }

    return outSRV;
}

winrt::com_ptr<ID3D11ShaderResourceView> MotionBlur::BackbufferToSRV(bool underui) {
    winrt::com_ptr<ID3D11Texture2D> savedBuffer;
    {
        std::lock_guard<std::mutex> lock(SwapchainHook::backbufferMutex);
        savedBuffer = SwapchainHook::SavedD3D11BackBuffer;
    }
    if (!savedBuffer) {
        return nullptr;
    }

    if (!SwapchainHook::d3d11Device) {
        return nullptr;
    }

    HRESULT hr;
    D3D11_TEXTURE2D_DESC d;
    savedBuffer->GetDesc(&d);
    winrt::com_ptr<ID3D11ShaderResourceView> outSRV;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = d.Format;

    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = d.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if (FAILED(hr = SwapchainHook::d3d11Device->CreateShaderResourceView(savedBuffer.get(), &srvDesc, outSRV.put()))) {
        std::cout << "Failed to create shader resource view: " << std::hex << hr << std::endl;
    }

    return outSRV;
}
