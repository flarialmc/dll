#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "Keystrokes.hpp"
#include <Windows.h>
#include <chrono>

bool rrightClickHeld;
bool lleftClickHeld;
int rRightCPS;
int lLeftCPS;

class KeystrokesListener : public Listener {

    Module* module;

    [[nodiscard]] double Microtime() const {
        return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
    }

    void onMouse(MouseEvent& event) {

        if (event.GetButton() == MouseButton::Left) {
            if (MC::held) {
                leftClickHeld = true;
            }
            else
            {
                leftClickHeld = false;
                AddLeftClick();
            }
        }
        if (event.GetButton() == MouseButton::Right) {
            if (MC::held) {
                rightClickHeld = true;
            }
            else
            {
                rightClickHeld = false;
                AddRightClick();
            }
        }

    }

    void onRender(RenderEvent& event) override {

        if(SDK::CurrentScreen == "hud_screen")
        if (module->settings.getSettingByName<bool>("enabled")->value) {
            rrightClickHeld = rightClickHeld;
            lleftClickHeld = leftClickHeld;
            rRightCPS = GetRightCPS();
            lLeftCPS = GetLeftCPS();
            this->module->NormalRender(7, "", "");

        }
    }

public:

    std::vector<D2D1_COLOR_F> states;
    std::vector<ClickData> leftClickList;
    std::vector<ClickData> rightClickList;
    bool rightClickHeld;
    bool leftClickHeld;

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

    [[nodiscard]] bool GetRightheld() const {
        return rightClickHeld;
    }

    [[nodiscard]] bool GetLeftheld() const {
        return leftClickHeld;
    }

    explicit KeystrokesListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

