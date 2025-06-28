#include "WeatherChanger.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"

WeatherChanger::WeatherChanger(): Module("Weather Changer", "Changes the weather ingame.", IDR_CLOUDY_PNG, "") {
}

void WeatherChanger::onEnable() {
    Listen(this, TickEvent, &WeatherChanger::onTick)
    Module::onEnable();
}

void WeatherChanger::onDisable() {
    Deafen(this, TickEvent, &WeatherChanger::onTick)
    Module::onDisable();
}

void WeatherChanger::defaultConfig() {
    Module::defaultConfig("core");
    setDef("rain", 1.00f);
    setDef("lightning", 0.00f);
    setDef("snow", false);
}

void WeatherChanger::settingsRender(float settingsOffset) {
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Misc");
    addSlider("Rain Intensity", "", "rain", 1.f);
    addSlider("Lightning Intensity", "", "lightning", 1.f);
    addToggle("Snow", "", "snow");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

bool equalBlockPos(const BlockPos one, const BlockPos two) {
    return one.x == two.x &&
           one.y == two.y &&
           one.z == two.z;
}

void WeatherChanger::onTick(TickEvent &event) {
    if (!this->isEnabled() || !SDK::clientInstance->getBlockSource() || !SDK::clientInstance->getBlockSource()->getDimension()->weather) return;

    if (getOps<float>("rain") > 0.02f) SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = this->settings.getSettingByName<float>("rain")->value;
    else SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = 0.0f;

    if (getOps<float>("lightning") < 0.02f) SDK::clientInstance->getBlockSource()->getDimension()->weather->lightningLevel = this->settings.getSettingByName<float>("lightning")->value;
    else SDK::clientInstance->getBlockSource()->getDimension()->weather->lightningLevel = 0.0f;

    if (getOps<bool>("snow")) {
        const Vec3<float> *pos = event.getActor()->getPosition();
        BlockPos bp(static_cast<int>(pos->x), static_cast<int>(pos->y), static_cast<int>(pos->z));

        if (Biome *curBiome = SDK::clientInstance->getBlockSource()->getBiome(bp); curBiome && curBiome->temperature != 0) {
            modifiedQueue.push_front(std::make_pair(bp, curBiome->temperature));
            curBiome->temperature = 0.0f;
        }
    }

    if (!getOps<bool>("snow")) while (getOps<bool>("snow") && modifiedQueue.size() > 1) {
        const Vec3<float> *pos = event.getActor()->getPosition();
        if (const BlockPos bp(static_cast<int>(pos->x), static_cast<int>(pos->y), static_cast<int>(pos->z)); equalBlockPos(bp, modifiedQueue.back().first)) continue;
        if (Biome *theBiome = SDK::clientInstance->getBlockSource()->getBiome(modifiedQueue.back().first)) theBiome->temperature = modifiedQueue.back().second;
        modifiedQueue.pop_back();
    }

    if (!getOps<bool>("snow") && modifiedQueue.size() > 0) {
        while (modifiedQueue.size() > 0) {
            if (Biome *theBiome = SDK::clientInstance->getBlockSource()->getBiome(modifiedQueue.back().first)) theBiome->temperature = modifiedQueue.back().second;
            modifiedQueue.pop_back();
        }
    }
}
