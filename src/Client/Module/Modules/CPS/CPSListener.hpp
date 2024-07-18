#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../Manager.hpp"
#include <windows.h>
#include <chrono>

class ClickData {
public:
    double timestamp;  // Milliseconds since some reference point
    // ... other click data members ...
};

class CPSListener : public Listener {

private:
    static inline std::vector<ClickData> leftClickList;
    static inline std::vector<ClickData> rightClickList;
    static inline bool rightClickHeld;
    static inline bool leftClickHeld;
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastRightClick;
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastLeftClick;
    Module *module;

    void onMouse(MouseEvent &event) override {
        Module *limiter = ModuleManager::getModule("CPS Limiter");
        if(limiter == nullptr) return;
        if (event.getButton() == MouseButton::Left) {
            if (!MC::held) {
                leftClickHeld = false;
            } else {
                leftClickHeld = true;
                if (limiter->settings.getSettingByName<bool>("enabled")) {
                    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - lastLeftClick;
                    float LT = 1.3f / limiter->settings.getSettingByName<float>("Left")->value;
                    if (duration.count() < LT and limiter->isEnabled()) {
                        MC::held = !MC::held;
                        event.cancel();
                        return;
                    }
                }
                AddLeftClick();
                lastLeftClick = std::chrono::high_resolution_clock::now();
            }
        }
        if (event.getButton() == MouseButton::Right) {
            if (!MC::held) {
                rightClickHeld = false;
            } else {
                rightClickHeld = true;
                if (limiter->settings.getSettingByName<bool>("enabled")) {
                    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - lastRightClick;
                    float RT = 1 / limiter->settings.getSettingByName<float>("Right")->value;
                    if (duration.count() < RT and limiter->isEnabled()) {
                        MC::held = !MC::held;
                        event.cancel();
                        return;
                    }
                }
                AddRightClick();
                lastRightClick = std::chrono::high_resolution_clock::now();
            }
        }

    }

    void onRender(RenderEvent &event) override {
            if (module->isEnabled()) {
                if (!module->settings.getSettingByName<bool>("rightcps")->value) {
                    std::string leftCPS = std::to_string(GetLeftCPS());
                    this->module->normalRender(1, leftCPS);
                } else {
                    std::string leftAndRightCPS = std::to_string(GetLeftCPS()) + " | " + std::to_string(GetRightCPS());
                    this->module->normalRender(1, leftAndRightCPS);
                }

            }

    }

public:
    explicit CPSListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

    static void AddLeftClick() {
        ClickData click{};
        click.timestamp = Microtime();
        leftClickList.insert(leftClickList.begin(), click);

        if (leftClickList.size() >= 100) {
            leftClickList.pop_back();
        }
    }

    static void AddRightClick() {
        ClickData click{};
        click.timestamp = Microtime();
        rightClickList.insert(rightClickList.begin(), click);

        if (rightClickList.size() >= 100) {
            rightClickList.pop_back();
        }
    }

    [[nodiscard]] static int GetLeftCPS() {
        if (leftClickList.empty()) {
            return 0;
        }

        double currentMicros = Microtime();
        auto count = std::count_if(leftClickList.begin(), leftClickList.end(), [currentMicros](const ClickData &click) {
            return (currentMicros - click.timestamp <= 1.0);
        });

        return (int) std::round(count);
    }

    [[nodiscard]] static int GetRightCPS() {
        if (rightClickList.empty()) {
            return 0;
        }

        double currentMicros = Microtime();
        auto count = std::count_if(rightClickList.begin(), rightClickList.end(),
                                  [currentMicros](const ClickData &click) {
                                      return (currentMicros - click.timestamp <= 1.0);
                                  });

        return (int) std::round(count);
    }

    [[nodiscard]] static bool GetLeftHeld() {
        return leftClickHeld;
    }

    [[nodiscard]] static bool GetRightHeld() {
        return rightClickHeld;
    }

private:
    [[nodiscard]] static double Microtime() {
        return (double(std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
    }

};

