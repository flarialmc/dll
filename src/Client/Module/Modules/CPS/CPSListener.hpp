#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

namespace CPS {

    int inline leftcps = 0;
    int inline rightcps = 0;
};

class ClickData {
public:
    double timestamp;  // Milliseconds since some reference point
    // ... other click data members ...
};

class CPSListener : public Listener {

private:
    std::vector<ClickData> leftClickList;
    std::vector<ClickData> rightClickList;
    Module* module;

    void onMouse(MouseEvent &event) override {

        if(event.GetButton() == MouseButton::Left && !MC::held) AddLeftClick();
        if(event.GetButton() == MouseButton::Right && !MC::held) AddRightClick();

    }

    void onRender(RenderEvent &event) override {

        if(SDK::CurrentScreen == "hud_screen")
        if(module->settings.getSettingByName<bool>("enabled")->value){
            if(!module->settings.getSettingByName<bool>("rightcps")->value)
            this->module->NormalRender(1, module->settings.getSettingByName<std::string>("text")->value, std::to_string(GetLeftCPS()));
            else this->module->NormalRender(1, module->settings.getSettingByName<std::string>("text")->value, std::to_string(GetLeftCPS()) + " | " + std::to_string(GetRightCPS()));

        }

    }

public:
    explicit CPSListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

    void AddLeftClick() {
        ClickData click{};
        click.timestamp = Microtime();
        leftClickList.insert(leftClickList.begin(), click);

        if (leftClickList.size() >= 100) {
            leftClickList.pop_back();
        }
    }

    void AddRightClick() {
        ClickData click{};
        click.timestamp = Microtime();
        rightClickList.insert(rightClickList.begin(), click);

        if (rightClickList.size() >= 100) {
            rightClickList.pop_back();
        }
    }

    [[nodiscard]] int GetLeftCPS() const {
        if (leftClickList.empty()) {
            return 0;
        }

        double currentMicros = Microtime();
        int count = std::count_if(leftClickList.begin(), leftClickList.end(), [currentMicros](const ClickData& click) {
            return (currentMicros - click.timestamp <= 1.0);
        });

        return (int)std::round(count);
    }

    [[nodiscard]] int GetRightCPS() const {
        if (rightClickList.empty()) {
            return 0;
        }

        double currentMicros = Microtime();
        int count = std::count_if(rightClickList.begin(), rightClickList.end(), [currentMicros](const ClickData& click) {
            return (currentMicros - click.timestamp <= 1.0);
            });

        return (int)std::round(count);
    }

private:
    [[nodiscard]] double Microtime() const {
        return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
    }

};

