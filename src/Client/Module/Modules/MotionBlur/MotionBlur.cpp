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

    // Always register listeners - the render methods check isDX12 and return early if needed
    // This allows Motion Blur to start working automatically once Better Frames triggers DX11 fallback
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
    RealMotionBlurHelper::Reset(); // Reset motion blur state to prevent stale matrix issues
    OnixMotionBlurHelper::Reset();
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
    setDef("blurType", (std::string)"Average Pixel Blur");
    // setDef("avgpixel", true);
    setDef("dynamic", false);
    setDef("samples", 64.f);
    setDef("renderUnderUI", false);
    // Time Aware Blur settings
    setDef("blurTimeConstant", 0.0667f);  // T = 4/60s for Vegas 240fps->60fps look
    setDef("maxHistoryFrames", 8.0f);     // Performance cap for frame history
}

void MotionBlur::settingsRender(float settingsOffset) {
    initSettingsPage();

    addToggle("Render Under UI", "When enabled, renders motion blur under the UI. When disabled, renders over the UI.", "renderUnderUI");
    // addToggle("Average Pixel Mode", "Disabling this will likely look better on high FPS.", "avgpixel");

    addDropdown("Blur Type", "", std::vector<std::string>{
                    "Average Pixel Blur",
                    "Real Motion Blur",
                    "Ghost Frames",
                    "Time Aware Blur",
                    "V4"
                }, "blurType", true);

    addConditionalToggle(getOps<std::string>("blurType") == "Average Pixel Blur", "Dynamic Mode", "Automatically adjusts intensity according to FPS", "dynamic");
    addConditionalSlider(getOps<std::string>("blurType") == "Average Pixel Blur" && !getOps<bool>("dynamic"), "Intensity", "Amount of previous frames to render.", "intensity2", 30, 0, true);

    addConditionalSlider(getOps<std::string>("blurType") == "Ghost Frames", "Intensity", "Amount of previous frames to render.", "intensity_ghost", 30, 0, true);
    addConditionalSlider(getOps<std::string>("blurType") == "Ghost Frames", "Bleed Factor", "Amount of previous frames to render.", "intensity_bleed", 1, 0, true);

    addConditionalSlider(getOps<std::string>("blurType") == "Real Motion Blur", "Intensity", "Amount of previous frames to render.", "intensity_real", 30, 0, true);

    addConditionalSlider(getOps<std::string>("blurType") == "V4", "Intensity", "Amount of previous frames to render.", "intensity_onix", 30, 0, true);

    // Time Aware Blur settings
    addConditionalSlider(getOps<std::string>("blurType") == "Time Aware Blur", "Blur Time Constant", "Higher = longer trails. 0.067 = Vegas 240fps->60fps look.", "blurTimeConstant", 0.2f, 0.01f, false);
    addConditionalSlider(getOps<std::string>("blurType") == "Time Aware Blur", "Max History Frames", "Performance limit. More frames = smoother but slower.", "maxHistoryFrames", 16, 4, true);

    // addConditionalSlider(getOps<std::string>("blurType") != "Average Pixel Blur", "Intensity", "Control how strong the motion blur is.", "intensity", 2, 0.05f, true);
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

    // Removed: Real Motion Blur no longer forces maxFrames=1 since we use frame stacking now

    if (SDK::getCurrentScreen() == "hud_screen" && initted && this->isEnabled()) {
        // Remove excess frames if maxFrames is reduced
        if (previousFrames.size() > static_cast<size_t>(maxFrames)) {
            size_t toRemove = previousFrames.size() - maxFrames;
            previousFrames.erase(previousFrames.begin(), previousFrames.begin() + toRemove);
            frameTimestamps.erase(frameTimestamps.begin(), frameTimestamps.begin() + toRemove);
        }

        // Initialize storage if needed
        SwapchainHook::InitializeBackbufferStorage(maxFrames);

        // Get current time in seconds using high-precision timer
        float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

        auto buffer = BackbufferToSRVExtraMode(true);
        if (buffer) {
            previousFrames.push_back(std::move(buffer));
            frameTimestamps.push_back(currentTime);
        }

        // Real Motion Blur uses GPU Gems velocity-based shader approach
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
        } else if (blurType == "V4") {
            // Onix-style motion blur: unblurred frames with distinctive trail blending
            // Uses higher initial alpha and slower falloff for smoother trails
            auto sceneSRV = BackbufferToSRVExtraMode(true);

            if (sceneSRV) {
                previousFrames.push_back(std::move(sceneSRV));
                frameTimestamps.push_back(currentTime);
            }

            // Onix-style blending: smoother, more visible trails
            float alpha = 0.35f;  // Higher initial alpha for more visible trails
            float bleedFactor = 0.85f;  // Slower falloff for longer trails

            for (const auto &frame: previousFrames) {
                ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                alpha *= bleedFactor;
            }
        } else if (blurType == "Time Aware Blur") {
            // Time-aware exponential decay blending
            float T = getOps<float>("blurTimeConstant");

            // Calculate weights for each historical frame using exponential decay
            std::vector<float> weights;
            float totalWeight = 0.0f;

            for (size_t i = 0; i < previousFrames.size(); i++) {
                float age = currentTime - frameTimestamps[i];
                float weight = std::exp(-age / T);
                weights.push_back(weight);
                totalWeight += weight;
            }

            // Normalize weights so they sum to 1
            if (totalWeight > 0.0f) {
                for (float& w : weights) {
                    w /= totalWeight;
                }
            }

            // Render historical frames with their calculated weights
            for (size_t i = 0; i < previousFrames.size(); i++) {
                if (weights[i] > 0.001f) {  // Skip negligible contributions
                    ImageWithOpacity(previousFrames[i], {MC::windowSize.x, MC::windowSize.y}, weights[i]);
                }
            }
        } else {
            // Ghost Frames and Average Pixel Blur use frame stacking approach
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
    static int callCount = 0;
    bool shouldLog = (++callCount % 300 == 1);

    auto blurType = getOps<std::string>("blurType");
    auto renderUnderUI = getOps<bool>("renderUnderUI");

    if (shouldLog) {
        // Logger::debug("[MotionBlur] onRenderNormal: blurType={}, renderUnderUI={}, isDX12={}",
            // blurType, renderUnderUI, SwapchainHook::isDX12);
    }

    if (!this->isEnabled() || ModuleManager::getModule("ClickGUI")->active) return;
    if (SwapchainHook::isDX12) return;

    // THIS IS THE PROBLEM: Non-Ghost Frames modes with renderUnderUI=false should NOT return here
    // But the logic says: return if (NOT Ghost Frames) AND (renderUnderUI is true)
    // So with renderUnderUI=false, this should NOT return... but let's check
    if (blurType != "Ghost Frames" && renderUnderUI) {
        if (shouldLog) // Logger::debug("[MotionBlur] Returning because renderUnderUI check");
        return;
    }

    int maxFrames;

    if (blurType == "Time Aware Blur") {
        maxFrames = (int) round(getOps<float>("maxHistoryFrames"));
    } else if (blurType == "V4") {
        maxFrames = (int) round(getOps<float>("intensity_onix"));
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

    // Removed: Real Motion Blur no longer forces maxFrames=1 since we use frame stacking now

    auto currentScreen = SDK::getCurrentScreen();
    if (shouldLog) {
        // Logger::debug("[MotionBlur] screen={}, initted={}, maxFrames={}", currentScreen, initted, maxFrames);
    }
    if (currentScreen == "hud_screen" && initted && this->isEnabled()) {
        // Remove excess frames if maxFrames is reduced
        if (previousFrames.size() > static_cast<size_t>(maxFrames)) {
            size_t toRemove = previousFrames.size() - maxFrames;
            previousFrames.erase(previousFrames.begin(), previousFrames.begin() + toRemove);
            frameTimestamps.erase(frameTimestamps.begin(), frameTimestamps.begin() + toRemove);
        }


        SwapchainHook::InitializeBackbufferStorage(maxFrames);

        // Get current time in seconds using high-precision timer
        float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

        auto buffer = BackbufferToSRVExtraMode(false);
        if (buffer) {
            previousFrames.push_back(std::move(buffer));
            frameTimestamps.push_back(currentTime);
        }

        static int logCounter = 0;
        if (++logCounter % 300 == 1) { // Log every ~5 seconds at 60fps
            // Logger::debug("[MotionBlur] initted={}, maxFrames={}, previousFrames.size={}, needsBackBuffer={}, buffer={}",
                // initted, maxFrames, previousFrames.size(), FlarialGUI::needsBackBuffer, buffer ? "valid" : "null");
        }

        static int blurLogCounter = 0;
        if (++blurLogCounter % 300 == 1) {
            // Logger::debug("[MotionBlur] Rendering blurType={}, frames={}", blurType, previousFrames.size());
        }

        // Real Motion Blur uses GPU Gems velocity-based shader approach
        if (blurType == "Real Motion Blur") {
            // Initialize shader helper if needed
            if (!realMotionBlurInitialized) {
                realMotionBlurInitialized = RealMotionBlurHelper::Initialize();
                if (!realMotionBlurInitialized) {
                    // Logger::debug("[MotionBlur] Failed to initialize RealMotionBlurHelper");
                }
            }

            // Render using velocity-based motion blur shader
            if (realMotionBlurInitialized && event.RTV) {
                auto sceneSRV = BackbufferToSRVExtraMode(false);

                if (sceneSRV) {
                    RealMotionBlurHelper::Render(event.RTV, sceneSRV);
                }
            }
        } else if (blurType == "V4") {
            // Onix-style motion blur: unblurred frames with distinctive trail blending
            // Uses higher initial alpha and slower falloff for smoother trails
            auto sceneSRV = BackbufferToSRVExtraMode(false);

            if (sceneSRV) {
                previousFrames.push_back(std::move(sceneSRV));
                frameTimestamps.push_back(currentTime);
            }

            // Onix-style blending: smoother, more visible trails
            float alpha = 0.35f;  // Higher initial alpha for more visible trails
            float bleedFactor = 0.85f;  // Slower falloff for longer trails

            for (const auto &frame: previousFrames) {
                ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                alpha *= bleedFactor;
            }
        } else if (blurType == "Time Aware Blur") {
            // Time-aware exponential decay blending
            float T = getOps<float>("blurTimeConstant");

            // Calculate weights for each historical frame using exponential decay
            std::vector<float> weights;
            float totalWeight = 0.0f;

            for (size_t i = 0; i < previousFrames.size(); i++) {
                float age = currentTime - frameTimestamps[i];
                float weight = std::exp(-age / T);
                weights.push_back(weight);
                totalWeight += weight;
            }

            // Normalize weights so they sum to 1
            if (totalWeight > 0.0f) {
                for (float& w : weights) {
                    w /= totalWeight;
                }
            }

            // Render historical frames with their calculated weights
            for (size_t i = 0; i < previousFrames.size(); i++) {
                if (weights[i] > 0.001f) {  // Skip negligible contributions
                    ImageWithOpacity(previousFrames[i], {MC::windowSize.x, MC::windowSize.y}, weights[i]);
                }
            }
        } else {
            // Ghost Frames and Average Pixel Blur use frame stacking approach
            float alpha = 0.3f;
            float bleedFactor = 0.8f;

            if (blurType == "Average Pixel Blur") {
                alpha = 0.25f;
                bleedFactor = 0.95f; // Slower falloff = more averaging
            }
            // Ghost Frames uses defaults: alpha=0.3, bleedFactor=0.8

            if (blurLogCounter % 300 == 1) {
                // Logger::debug("[MotionBlur] Drawing {} frames with alpha={}", previousFrames.size(), alpha);
            }

            for (const auto &frame: previousFrames) {
                ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                alpha *= bleedFactor;
            }
        }
    } else {
        static bool loggedNotHud = false;
        if (!loggedNotHud && !initted) {
            // Logger::debug("[MotionBlur] Not rendering - screen={}, initted={}", currentScreen, initted);
            loggedNotHud = true;
        }
        previousFrames.clear();
        frameTimestamps.clear();
    }
}

void MotionBlur::ImageWithOpacity(const winrt::com_ptr<ID3D11ShaderResourceView> &srv, ImVec2 size, float opacity) {
    if (opacity <= 0.0f) {
        //std::cout << "alpha: " + FlarialGUI::cached_to_string(opacity) << std::endl;
        return;
    }

    opacity = opacity > 1.0f ? 1.0f : opacity < 0.0f ? 0.0f : opacity;
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    ImVec2 pos = {0, 0};
    ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(opacity * 255));
    draw_list->AddImage(ImTextureID(srv.get()), pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), col);
    // NOTE: Removed ImGui::SetCursorScreenPos() - it requires an active ImGui window context
    // which doesn't exist during RenderUnderUIEvent (ImGui Begin/End is commented out there).
    // The call was unnecessary anyway since we're just drawing to the background draw list.
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

    // Take a local copy under lock to prevent race condition - another thread could null the static between check and use
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
    // Take a local copy under lock to prevent race condition
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
