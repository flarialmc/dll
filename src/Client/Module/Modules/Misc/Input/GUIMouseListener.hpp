#pragma once

#include <format>
#include "../../../Manager.hpp"
#include "Utils/Utils.hpp"
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>

class ClickData {
public:
    double timestamp;
};

class RateLimiter {
public:
    using Clock = std::chrono::high_resolution_clock;

    RateLimiter() : cpsLimit(0) {}

    void setRate(int cps) {
        cpsLimit = cps;
    }

    bool allow() {
        auto now = Clock::now();
        auto oneSecAgo = now - std::chrono::seconds(1);

        while (!q.empty() && q.front() < oneSecAgo) q.pop_front();

        if (static_cast<int>(q.size()) >= cpsLimit) return false;

        q.push_back(now);
        return true;
    }

private:
    int cpsLimit;
    std::deque<std::chrono::time_point<Clock>> q;
};

class GUIMouseListener {
public:
    GUIMouseListener() {
        Listen(this, MouseEvent, &GUIMouseListener::onMouse);

        leftLimiter.setRate(10);
        rightLimiter.setRate(10);
    }

    static inline std::vector<ClickData> leftClickList;
    static inline std::vector<ClickData> rightClickList;

    static inline double lastLeftClickTime = 0.0;
    static inline double lastRightClickTime = 0.0;

    static inline RateLimiter leftLimiter;
    static inline RateLimiter rightLimiter;

    void onMouse(MouseEvent &event) {
        handleMouse(event);
    };

    static void handleMouse(MouseEvent& event) {
        if (event.getMouseX() != 0) MC::mousePos.x = event.getMouseX();
        if (event.getMouseY() != 0) MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();
        if (event.getButton() != None) MC::lastMouseButton = event.getButton();

        if (event.getButton() != None && event.getAction() == Press) {
            MC::held = true;
            if (event.getButton() == Left) MC::heldLeft = true;
            if (event.getButton() == Right) MC::heldRight = true;
        }
        if (event.getButton() != None && event.getAction() == Release) {
            MC::held = false;
            if (event.getButton() == Left) MC::heldLeft = false;
            if (event.getButton() == Right) MC::heldRight = false;
        }

        bool limiterEnabled = false;
        int leftRate = 10;
        int rightRate = 10;
        if (auto limiterMod = ModuleManager::getModule("CPS Limiter")) {
            limiterEnabled = limiterMod->getOps<bool>("enabled");
            leftRate = limiterMod->getOps<int>("Left");
            rightRate = limiterMod->getOps<int>("Right");
        }

        if (limiterEnabled) {
            leftLimiter.setRate(leftRate);
            rightLimiter.setRate(rightRate);
        }

        using MB = MouseButton;

        if (event.getButton() == Left && MC::held) {
            if (limiterEnabled) {
                if (!leftLimiter.allow()) {
                    event.cancel();
                    return;
                }
            }
            AddLeftClick();
        }
        else if (event.getButton() == Right && MC::held) {
            if (limiterEnabled) {
                if (!rightLimiter.allow()) {
                    event.cancel();
                    return;
                }
            }
            AddRightClick();
        }
    }

    static void AddLeftClick() {
        double now = Utils::Microtime();

        auto limiterMod = ModuleManager::getModule("CPS Limiter");
        int debounceMs = limiterMod->getOps<int>("debounce");
        double debounceSec = static_cast<double>(debounceMs) / 1000.0;

        if (lastLeftClickTime > 0.0 && (now - lastLeftClickTime) < debounceSec && limiterMod->getOps<bool>("enabled")) return;
        lastLeftClickTime = now;

        ClickData click{};
        click.timestamp = now;
        leftClickList.insert(leftClickList.begin(), click);

        if (leftClickList.size() >= 100) {
            leftClickList.pop_back();
        }
    }

    static void AddRightClick() {
        double now = Utils::Microtime();

        auto limiterMod = ModuleManager::getModule("CPS Limiter");
        int debounceMs = limiterMod->getOps<int>("debounce");
        double debounceSec = static_cast<double>(debounceMs) / 1000.0;

        if (lastRightClickTime > 0.0 && (now - lastRightClickTime) < debounceSec && limiterMod->getOps<bool>("enabled")) return;
        lastRightClickTime = now;

        ClickData click{};
        click.timestamp = now;
        rightClickList.insert(rightClickList.begin(), click);

        if (rightClickList.size() >= 100) {
            rightClickList.pop_back();
        }
    }

    static int GetLeftCPS() {
        if (leftClickList.empty()) {
            return 0;
        }

        double currentMicros = Utils::Microtime();
        auto count = std::count_if(leftClickList.begin(), leftClickList.end(), [currentMicros](const ClickData& click) {
            return (currentMicros - click.timestamp <= 1.0);
        });

        return (int)std::round(count);
    }

    static int GetRightCPS() {
        if (rightClickList.empty()) {
            return 0;
        }

        double currentMicros = Utils::Microtime();
        auto count = std::ranges::count_if(rightClickList,
                                           [currentMicros](const ClickData& click) {
                                               return (currentMicros - click.timestamp <= 1.0);
                                           });

        return (int)std::round(count);
    }

    static double getCurrentTime() {
        using namespace std::chrono;
        using Clock = high_resolution_clock;
        auto now = Clock::now();
        auto us = duration_cast<microseconds>(now.time_since_epoch()).count();
        return static_cast<double>(us) / 1000000.0;
    }
};
