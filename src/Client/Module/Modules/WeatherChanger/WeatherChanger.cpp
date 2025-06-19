#include "WeatherChanger.hpp"
#include "Client.hpp"
#include "Events/EventManager.hpp"

WeatherChanger::WeatherChanger(): Module("Weather Changer", "Changes the weather ingame.", IDR_CLOUDY_PNG, "")
{
    
}

void WeatherChanger::onEnable()
{
    Listen(this, TickEvent, &WeatherChanger::onTick)
    Module::onEnable();
}

void WeatherChanger::onDisable()
{
    Deafen(this, TickEvent, &WeatherChanger::onTick)
    Module::onDisable();
}

void WeatherChanger::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("rain", 1.00f);
    setDef("lighting", 0.00f);
    setDef("snow", false);
    
}

void WeatherChanger::settingsRender(float settingsOffset)
{
    float x = Constraints::PercentageConstraint(0.019, "left");
    float y = Constraints::PercentageConstraint(0.10, "top");

    const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

    FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
    FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                              Constraints::RelativeConstraint(1.0, "width"),
                              Constraints::RelativeConstraint(0.88f, "height"));

    addHeader("Misc");
    addSlider("Rain Intensity", "", "rain");
    addSlider("Snow Intensity", "", "snow");

    FlarialGUI::UnsetScrollView();

    resetPadding();
}

void WeatherChanger::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    if (!SDK::clientInstance->getBlockSource())
        return;

    if (this->isEnabled()) {
        if (getOps<float>("rain") > 0.02f)
            SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = this->settings.getSettingByName<float>(
                "rain")->value;
        else SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = 0.0f;
        if (getOps<float>("lighting") < 0.02f)
            SDK::clientInstance->getBlockSource()->getDimension()->weather->lightingLevel = this->settings.getSettingByName<float>(
                "lighting")->value;
        else SDK::clientInstance->getBlockSource()->getDimension()->weather->lightingLevel = 0.0f;

        // TODO: When you set snow, it will stay even if on until game reload
        if (getOps<bool>("snow")) {
            Vec3<float>* pos = event.getActor()->getPosition();
            Vec3<int> e((int)pos->x, (int)pos->y, (int)pos->z);

            SDK::clientInstance->getBlockSource()->getBiome(e)->temparature = 0.0f;
        }
    }
}
