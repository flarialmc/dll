#include "WeatherChanger.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"

WeatherChanger::WeatherChanger(): Module("Weather Changer", "Changes the weather ingame.", IDR_CLOUDY_PNG, "") {
}

void WeatherChanger::onEnable() {
    Listen(this, TickEvent, &WeatherChanger::onTick)
    Listen(this, RenderEvent, &WeatherChanger::onRender)
    Module::onEnable();
}

void WeatherChanger::onDisable() {
    if (oldBiome) {
        oldBiome->temperature = oldTemp;
        oldBiome = nullptr;
    }

    Deafen(this, TickEvent, &WeatherChanger::onTick)
    Deafen(this, RenderEvent, &WeatherChanger::onRender)
    Module::onDisable();
}

void WeatherChanger::onRender(RenderEvent& event) {
    if (SDK::getServerIP() == "none") oldBiome = nullptr;
}

void WeatherChanger::defaultConfig() {
    Module::defaultConfig("core");
    setDef("rain", 1.00f);
    setDef("lightning", 0.00f);
    setDef("customTemp", false);
    setDef("temperature", 0.5f);
    setDef("snow", false);
}

void WeatherChanger::settingsRender(float settingsOffset) {
    initSettingsPage();

    addHeader("Misc");
    addSlider("Rain Intensity", "", "rain", 1.f);
    addSlider("Lightning Intensity", "", "lightning", 1.f);
    addToggle("Custom Temperature", "", "customTemp");
    addConditionalSlider(getOps<bool>("customTemp"), "Temperature", "", "temperature", 1.f, 0.f, true);
    addConditionalToggle(!getOps<bool>("customTemp"), "Snow", "", "snow");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void WeatherChanger::onTick(TickEvent &event) {
    if (!this->isEnabled() || !SDK::clientInstance->getBlockSource() || !SDK::clientInstance->getBlockSource()->getDimension() || !SDK::clientInstance->getBlockSource()->getDimension()->weather) return;

    if (getOps<float>("rain") > 0.02f) SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = this->settings.getSettingByName<float>("rain")->value;
    else SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = 0.0f;

    if (getOps<float>("lightning") < 0.02f) SDK::clientInstance->getBlockSource()->getDimension()->weather->lightningLevel = this->settings.getSettingByName<float>("lightning")->value;
    else SDK::clientInstance->getBlockSource()->getDimension()->weather->lightningLevel = 0.0f;

    if (getOps<bool>("snow") || getOps<bool>("customTemp")) {
        const Vec3<float> *pos = event.getActor()->getPosition();
        const BlockPos bp(static_cast<int>(pos->x), static_cast<int>(pos->y), static_cast<int>(pos->z));
        const float toSet = getOps<bool>("snow") ? 0.0f : getOps<float>("temperature");

        if (Biome *curBiome = SDK::clientInstance->getBlockSource()->getBiome(bp); curBiome && curBiome->temperature != toSet) {
            if (oldBiome) oldBiome->temperature = oldTemp;
            oldBiome = curBiome;
            oldTemp = curBiome->temperature;
            curBiome->temperature = toSet;
        }
    } else if (oldBiome) {
        oldBiome->temperature = oldTemp;
        oldBiome = nullptr;
    }
}