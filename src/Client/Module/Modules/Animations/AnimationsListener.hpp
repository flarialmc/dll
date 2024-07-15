#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Client.hpp"
#include <Windows.h>
#include <string>

class AnimationsListener : public Listener {

    Module *module;

    inline static float animate(float endPoint, float current, float speed) {
        if (speed < 0.0) speed = 0.0;
        else if (speed > 1.0) speed = 1.0;

        float dif = (((endPoint) > (current)) ? (endPoint) : (current)) - (((endPoint) < (current)) ? (endPoint) : (current));
        float factor = dif * speed;
        return current + (endPoint > current ? factor : -factor);
    }

    void onDrawImage(DrawImageEvent &event) override {
        if (strcmp(event.getTexturePath().c_str(), "textures/ui/selected_hotbar_slot") == 0)
        {
            static float lerpedPos = event.getImagePos().x; // old pos
            lerpedPos = animate(event.getImagePos().x, lerpedPos, (0.016f * module->settings.getSettingByName<float>("hotbarSpeed")->value) * FlarialGUI::frameFactor);
            event.getImagePos().x = lerpedPos;
        }
    }

public:
    explicit AnimationsListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};

