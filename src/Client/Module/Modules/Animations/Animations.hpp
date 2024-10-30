#pragma once

#include "../Module.hpp"


class Animations : public Module {

public:

    Animations() : Module("Animations", "Animate your selected slot square\nwhile you switch slots.",
                          IDR_ANIMATIONS_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        Listen(this, DrawImageEvent, &Animations::onDrawImage)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, DrawImageEvent, &Animations::onDrawImage)
        Module::onDisable();
    }

    void defaultConfig() override {

        if (settings.getSettingByName<float>("hotbarSpeed") == nullptr) settings.addSetting("hotbarSpeed", 7.0f);

    }

    inline static float animate(float endPoint, float current, float speed) {
        if (speed < 0.0) speed = 0.0;
        else if (speed > 1.0) speed = 1.0;

        float dif = (((endPoint) > (current)) ? (endPoint) : (current)) - (((endPoint) < (current)) ? (endPoint) : (current));
        float factor = dif * speed;
        return current + (endPoint > current ? factor : -factor);
    }

    void onDrawImage(DrawImageEvent &event) {
        if (strcmp(event.getTexturePath().c_str(), "textures/ui/selected_hotbar_slot") == 0) {
            auto pos = event.getImagePos();
            static float lerpedPos = pos.x; // old pos
            lerpedPos = animate(pos.x, lerpedPos, (0.016f * this->settings.getSettingByName<float>("hotbarSpeed")->value) * FlarialGUI::frameFactor);
            event.setImagePos(Vec2<float>{lerpedPos, pos.y});
        }
    }

    void settingsRender() override {

        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addButton("troll", "troll", "troll", []() {

        });
        this->addButton("troll", "troll", "even more", []() {

        });
        this->addSlider("Hotbar", "", this->settings.getSettingByName<float>("hotbarSpeed")->value);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }
};
