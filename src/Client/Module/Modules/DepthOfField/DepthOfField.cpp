#include "DepthOfField.hpp"
#include "Events/EventManager.hpp"
#include "DepthOfFieldHelper.hpp"
#include "Hook/Hooks/Render/SwapchainHook.hpp"
#include "Modules/MotionBlur/MotionBlur.hpp"
#include "Utils/Logger/Logger.hpp"

void DepthOfField::onEnable() {
    Listen(this, RenderUnderUIEvent, &DepthOfField::onRender)
    Module::onEnable();
}

void DepthOfField::onDisable() {
    Deafen(this, RenderUnderUIEvent, &DepthOfField::onRender)

    if (DepthOfFieldHelper::pDepthMapSRV) {
        DepthOfFieldHelper::pDepthMapSRV->Release();
        DepthOfFieldHelper::pDepthMapSRV = nullptr;
    }

    Module::onDisable();
}

void DepthOfField::defaultConfig() {
    if (settings.getSettingByName<float>("strength") == nullptr)
        settings.addSetting("strength", 0.8f);

    if (settings.getSettingByName<float>("range") == nullptr)
        settings.addSetting("range", 0.3f);

    if (settings.getSettingByName<float>("amount") == nullptr)
        settings.addSetting("amount", 2.0f);

    if (settings.getSettingByName<float>("quality") == nullptr)
        settings.addSetting("quality", 1.0f);

    if (settings.getSettingByName<float>("focus") == nullptr)
        settings.addSetting("focus", 0.3f);

    if (settings.getSettingByName<bool>("autoFocus") == nullptr)
        settings.addSetting("autoFocus", false);

    Module::defaultConfig("core");
}

void DepthOfField::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Depth of Field");
    addToggle("Auto Focus", "Automatically focus on what you're looking at", "autoFocus");
    addSlider("Strength", "How strong the effect is", "strength", 3.0f);
    addSlider("Range", "How wide the focused area is", "range", 5.0f);
    addSlider("Amount", "Maximum blur for out-of-focus areas", "amount", 5.0f);
    addSlider("Quality", "Higher values = smoother blur", "quality", 5.0f, 1.0f);
    addSlider("Focus", "Adjust what distance is in focus", "focus", 1.0f);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void DepthOfField::onRender(RenderUnderUIEvent &event) {
    if (!SwapchainHook::context || !SwapchainHook::d3d11Device || !event.RTV) {
        return;
    }

    static bool initialized = false;
    if (!initialized) {
        try {
            DepthOfFieldHelper::InitializePipeline();
            initialized = true;
        } catch (const std::exception& e) {
            return;
        }
    }

    auto strengthSetting = settings.getSettingByName<float>("strength");
    auto qualitySetting = settings.getSettingByName<float>("quality");
    auto rangeSetting = settings.getSettingByName<float>("range");
    auto amountSetting = settings.getSettingByName<float>("amount");
    auto focusSetting = settings.getSettingByName<float>("focus");
    auto autoFocusSetting = settings.getSettingByName<bool>("autoFocus");

    if (!strengthSetting || !qualitySetting || !rangeSetting || !amountSetting || !focusSetting || !autoFocusSetting) {
        return;
    }

    float intensity = strengthSetting->value;
    int iterations = static_cast<int>(qualitySetting->value);
    float focusRange = rangeSetting->value;
    float maxBlur = amountSetting->value;
    float focusDistance = focusSetting->value;
    bool autoFocus = autoFocusSetting->value;

    intensity = std::max(0.0f, std::min(intensity, 3.0f));
    iterations = std::max(1, std::min(iterations, 5));
    focusRange = std::max(0.1f, std::min(focusRange, 5.0f));
    maxBlur = std::max(1.0f, std::min(maxBlur, 5.0f));
    focusDistance = std::max(0.0f, std::min(focusDistance, 1.0f));

    if (intensity <= 0.01f) {
        return;
    }

    DepthOfFieldHelper::RenderDepthOfField(event.RTV, iterations, intensity, focusRange, focusDistance, autoFocus);
}
