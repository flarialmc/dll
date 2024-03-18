#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include "../../Manager.hpp"
#include "../../../../SDK/Client/Core/Options.hpp"

class FOVChangerListener : public Listener {

    Module *module;

    void onGetFOV(FOVEvent &event) override {
        bool inserver;

        std::string serverIP = SDK::getServerIP();

        if (serverIP.find("world") != std::string::npos) inserver = true;
        else inserver = false;

        auto fovSetting = module->settings.getSettingByName<float>("fovvalue")->value;

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


public:
    static inline bool notified150Fov = false;
    explicit FOVChangerListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
