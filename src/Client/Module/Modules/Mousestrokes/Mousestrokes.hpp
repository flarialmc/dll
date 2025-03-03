#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class Mousestrokes : public Module {

public:

    Mousestrokes() : Module("Mouse Strokes", "stroking yo shit",
        IDR_FPS_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, RenderEvent, &Mousestrokes::onRender);
        Listen(this, MouseEvent, &Mousestrokes::onMouse);
        Module::onEnable();

        std::thread normalizeCursor([&]() {
            while (!Client::disable) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                X *= 0.01;
                Y *= 0.01;
            }
            });

        normalizeCursor.detach();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &Mousestrokes::onRender);
        Deafen(this, MouseEvent, &Mousestrokes::onMouse);
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string)"FPS: {value}");

        if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
        if (settings.getSettingByName<float>("fpsSpoofer") == nullptr) settings.addSetting("fpsSpoofer", 1.0f);

    }

    void settingsRender(float settingsOffset) override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
            Constraints::RelativeConstraint(1.0, "width"),
            Constraints::RelativeConstraint(0.88f, "height"));


        this->addHeader("Main");
        this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
        this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
        this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>(
            "BlurEffect")->value);
        this->addConditionalSlider(this->settings.getSettingByName<bool>(
            "border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value,
            4.f);
        this->addSlider("Rounding", "Rounding of the rectangle",
            this->settings.getSettingByName<float>("rounding")->value);

        this->extraPadding();

        this->addHeader("Text");
        this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
        this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
        this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"},
            this->settings.getSettingByName<std::string>("textalignment")->value);
        this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value,
            settings.getSettingByName<float>("textOpacity")->value,
            settings.getSettingByName<bool>("textRGB")->value);

        this->extraPadding();

        this->addHeader("Colors");
        this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value,
            settings.getSettingByName<float>("bgOpacity")->value,
            settings.getSettingByName<bool>("bgRGB")->value);
        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value,
            settings.getSettingByName<float>("borderOpacity")->value,
            settings.getSettingByName<bool>("borderRGB")->value);

        this->extraPadding();

        this->addHeader("Misc Customizations");

        this->addToggle("Reverse Padding X", "For Text Position", this->settings.getSettingByName<bool>(
            "reversepaddingx")->value);

        this->addToggle("Reverse Padding Y", "For Text Position", this->settings.getSettingByName<bool>(
            "reversepaddingy")->value);

        this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
        this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);

        this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
        this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);

        this->addToggle("Responsive Rectangle", "Rectangle resizes with text", this->settings.getSettingByName<bool>(
            "responsivewidth")->value);

        this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value,
            360.f, 0, false);

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

    void onRender(RenderEvent& event) {
        if (this->isEnabled()) {
            int fps = (int)round(((float)MC::fps *
                round(this->settings.getSettingByName<float>(
                    "fpsSpoofer")->value)));

            auto fpsStr = std::to_string(fps);

            this->normalRender(0, fpsStr);
        }
    }

    void onMouse(MouseEvent& event) {
        X += event.getMouseMovementX();
        Y += event.getMouseMovementY();
    }

    struct CircleTrail {
        float x, y, opacity;
    };

    std::vector<CircleTrail> trails = {};

    float MousePosX, MousePosY;

    void normalRender(int index, std::string& value) override {
        if (!SDK::hasInstanced) return;
        if (SDK::clientInstance->getLocalPlayer() == nullptr) return;

        const float interpolationFactor = 0.15f * FlarialGUI::frameFactor;

        for (auto it = trails.begin(); it != trails.end(); ) {
            FlarialGUI::lerp(it->opacity, 0.0f, interpolationFactor);

            if (it->opacity <= 0.0f) {
                it = trails.erase(it);
            }
            else {
                ++it;
            }
        }

        Vec2 RectSize = Vec2(Constraints::PercentageConstraint(0.15, "top"), Constraints::PercentageConstraint(0.15, "top"));
        Vec2<float> RectPos(0, 0);

        FlarialGUI::lerp<float>(CurrentCursorPos.x, X * Constraints::PercentageConstraint(1, "top") / 1080, FlarialGUI::frameFactor * 0.25);
        FlarialGUI::lerp<float>(CurrentCursorPos.y, Y * Constraints::PercentageConstraint(1, "top") / 1080, FlarialGUI::frameFactor * 0.25);

        Vec2 CursorPos(RectSize.div(Vec2<float>(2, 2)).add(RectPos).add(CurrentCursorPos));
        Vec2 Centre = RectSize.div(Vec2<float>(2, 2));

        if (CursorPos.x < RectPos.x) CursorPos.x = RectPos.x;
        if (CursorPos.y < RectPos.y) CursorPos.y = RectPos.y;

        if (CursorPos.x > RectPos.add(RectSize).x) CursorPos.x = RectPos.add(RectSize).x;
        if (CursorPos.y > RectPos.add(RectSize).y) CursorPos.y = RectPos.add(RectSize).y;

        CircleTrail balls;

        balls.x = CursorPos.x;
        balls.y = CursorPos.y;
        balls.opacity = 1;

        trails.insert(trails.begin(), balls);

        D2D1_COLOR_F colorR;
        colorR.r = 0;
        colorR.g = 0;
        colorR.b = 0;
        colorR.a = 0.5;

        float Opai = 1;

        FlarialGUI::RoundedRect(RectPos.x, RectPos.y, colorR, RectSize.x, RectSize.y);
        for (CircleTrail BariNudes : trails) {
            D2D1_COLOR_F color;
            color.r = 1;
            color.g = 1;
            color.b = 1;
            color.a = Opai;

            Opai *= 0.9;

            FlarialGUI::Circle(BariNudes.x, BariNudes.y, color, Constraints::PercentageConstraint(0.0074, "top"));

            if (Opai < 0.0001) { 
                trails.pop_back(); 
                return;
            }
        }
        
    }

    int X = 0;
    int Y = 0;

    Vec2<float> CurrentCursorPos = Vec2<float>(0, 0);
};