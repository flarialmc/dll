#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class FPSListener : public Listener {

    Module* module;

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == this->module->keybind) { this->module->enabled = !this->module->enabled; }
    };

    void onRender(RenderEvent &event) override {

        if(this->module->enabled){
            float textWidth = Constraints::RelativeConstraint(0.3);
            float textHeight = Constraints::RelativeConstraint(0.1);


            Vec2<float> realcenter = Constraints::CenterConstraint(textWidth, textHeight);

            float rectWidth = Constraints::RelativeConstraint(0.215);
            Vec2<float> rounde = Constraints::RoundingConstraint(40, 40);

            float realspacing = Constraints::SpacingConstraint(0.34, textWidth);

            FlarialGUI::SetWindowRect(realcenter.x, realcenter.y, rectWidth, textHeight, 0);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(realcenter.x, realcenter.y, 0);

            realcenter.x = vec2.x;
            realcenter.y = vec2.y;

            Vec2<float> percentages = Constraints::CalculatePercentage(realcenter.x, realcenter.y);

            this->module->settings.setValue("percentageX", percentages.x);
            this->module->settings.setValue("percentageY", percentages.y);

            //FIND NEW WORKAROUND LATER!!!!!!
            std::to_string(realcenter.x);

            FlarialGUI::RoundedRect(realcenter.x, realcenter.y,
                                    D2D1::ColorF(18.0f / 255.0f, 14.0f / 255.0f, 15.0f / 255.0f), rectWidth, textHeight,
                                    rounde.x, rounde.x);
            FlarialGUI::FlarialText(realcenter.x - realspacing, realcenter.y,
                                    FlarialGUI::to_wide(std::format("FPS: {}", MC::fps)).c_str(),
                                    D2D1::ColorF(63.0f / 255.0f, 42.0f / 255.0f, 45.0f / 255.0f), textWidth,
                                    textHeight);

            FlarialGUI::UnsetWindowRect();

        }

    }

public:
    explicit FPSListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

