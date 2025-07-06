#include "DepthOfField.hpp"
#include "Events/EventManager.hpp"
#include "DoFEffect.hpp"
#include "Hook/Hooks/Render/SwapchainHook.hpp"

void DepthOfField::onEnable() {
    Listen(this, RenderEvent, &DepthOfField::onRender)
    Module::onEnable();
}

void DepthOfField::onDisable() {
    Deafen(this, RenderEvent, &DepthOfField::onRender)
    Module::onDisable();
}

void DepthOfField::defaultConfig() {
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

    addHeader("Disable Mouse Wheel");
    addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

bool init = false;
DepthOfFieldShader dof;

void DepthOfField::onRender(RenderEvent &event) {
    DepthOfFieldShader::d3d11Context = SwapchainHook::context;
    DepthOfFieldShader::d3d11Device = SwapchainHook::d3d11Device;

    if (!init)
    {
        dof.Initialize();
        init = true;
    }

    dof.Render(event.RTV, 3, 1.0);

}
