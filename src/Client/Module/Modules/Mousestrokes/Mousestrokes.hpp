#pragma once

#include "../Module.hpp"


class Mousestrokes : public Module
{
public:
    Mousestrokes() : Module("Mouse Strokes", "Visualizes your camera/mouse movement.",
        IDR_CURSOR_PNG, "") {

    }

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    void onRender(RenderEvent& event);

    struct CircleTrail
    {
        float x, y, opacity;
    };

    std::vector<CircleTrail> trails = {};

    void normalRender(int index, std::string& value) override;

    float X = 0;  // Accumulated rotation delta (yaw)
    float Y = 0;  // Accumulated rotation delta (pitch)

    Vec2<float> CurrentCursorPos = Vec2<float>(0, 0);
};
