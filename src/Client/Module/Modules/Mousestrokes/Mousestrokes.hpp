#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"

class Mousestrokes : public Module
{
public:
    Mousestrokes();;

    void onEnable() override;

    void onDisable() override;

    void defaultConfig() override;

    void settingsRender(float settingsOffset) override;

    std::chrono::steady_clock::time_point previousFrameTime = std::chrono::high_resolution_clock::now();

    void onRender(RenderEvent& event);

    void onMouse(MouseEvent& event);

    struct CircleTrail
    {
        float x, y, opacity;
    };

    std::vector<CircleTrail> trails = {};

    float MousePosX, MousePosY;

    void normalRender(int index, std::string& value) override;

    int X = 0;
    int Y = 0;

    Vec2<float> CurrentCursorPos = Vec2<float>(0, 0);
};
