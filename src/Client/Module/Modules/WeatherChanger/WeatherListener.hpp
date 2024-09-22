#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>
// TODO: when disabled, does not restore original settings
class WeatherListener : public Listener {

    Module *module;

    void onTick(TickEvent &event) override {
        if (!SDK::clientInstance->getBlockSource())
            return;

        if (module->isEnabled()) {
            if (module->settings.getSettingByName<float>("rain")->value > 0.02f)
                SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = module->settings.getSettingByName<float>(
                        "rain")->value;
            else SDK::clientInstance->getBlockSource()->getDimension()->weather->rainLevel = 0.0f;
            if (module->settings.getSettingByName<float>("lighting")->value < 0.02f)
                SDK::clientInstance->getBlockSource()->getDimension()->weather->lightingLevel = module->settings.getSettingByName<float>(
                        "lighting")->value;
            else SDK::clientInstance->getBlockSource()->getDimension()->weather->lightingLevel = 0.0f;

            // TODO: When you set snow, it will stay even if on until game reload
            if (module->settings.getSettingByName<bool>("snow")->value) {
                Vec3<float> *pos = event.getActor()->getPosition();
                Vec3<int> e((int)pos->x, (int)pos->y, (int)pos->z);

                SDK::clientInstance->getBlockSource()->getBiome(e)->temparature = 0.0f;
            }
        }
    }

public:
    explicit WeatherListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};

