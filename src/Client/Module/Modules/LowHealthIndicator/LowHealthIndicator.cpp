#include "LowHealthIndicator.hpp"

LowHealthIndicator::LowHealthIndicator(): Module("Low Health", "Warns you when you are at low health.", IDR_HEART_PNG, "", false)
{
    
}

void LowHealthIndicator::defaultConfig()
{
    settings.renameSetting("color", "", "rgb", "health");
    Module::defaultConfig("core");
    setDef("health", 12.f);
    setDef("maxopacity", 0.7f);
    setDef("health", (std::string)"FF0000", 1.f, false);
    
}

void LowHealthIndicator::settingsRender(float settingsOffset)
{
    initSettingsPage();


    addHeader("Low Health Indicator");
    addSlider("Maximum overlay opacity", "The maximum overlay opacity", "maxopacity", 1.0f, 0.0f);
    addColorPicker("Color", "", "health");
    addSlider("Health", "The health at which the overlay will be shown", "health", 20.0f, 0.0f);
    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void LowHealthIndicator::onEnable()
{
    Module::onEnable();
    Listen(this, TickEvent, &LowHealthIndicator::onTick);
    Listen(this, RenderEvent, &LowHealthIndicator::onRender);
}

void LowHealthIndicator::onDisable()
{
    Module::onDisable();
    Deafen(this, TickEvent, &LowHealthIndicator::onTick);
    Deafen(this, RenderEvent, &LowHealthIndicator::onRender);
}

void LowHealthIndicator::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (SDK::clientInstance) if (SDK::clientInstance->getLocalPlayer())
        health = SDK::clientInstance->getLocalPlayer()->getHealth();
}

void LowHealthIndicator::onRender(RenderEvent& event)
{
    if (this->isEnabled() && this->health <= this->settings.getSettingByName<float>("health")->value &&
        SDK::currentScreen == "hud_screen" && !ModuleManager::getModule("ClickGUI")->isEnabled()) {

        float maxOpacity = this->settings.getSettingByName<float>("maxopacity")->value;

        float maxHealth = 20.0f;
        float opacity = maxOpacity * (1.0f - (this->health / maxHealth));

        D2D1_COLOR_F color2 = getColor("health");
        color2.a = std::clamp(opacity, 0.0f, maxOpacity);

        //FlarialGUI::RoundedRect(0, 0, color2, MC::windowSize.x, MC::windowSize.y, 0, 0);
        D2D1_ROUNDED_RECT roundedrect;
        roundedrect.rect = { 0, 0, Constraints::PercentageConstraint(1, "left"), Constraints::PercentageConstraint(1, "top") };
        auto ShadowSize = Constraints::PercentageConstraint(0.8, "top") * (1.0f - (this->health / maxHealth));
        roundedrect.radiusX = 0;
        roundedrect.radiusY = 0;
        FlarialGUI::InnerShadowRect(roundedrect, ShadowSize, color2);
    }
}
