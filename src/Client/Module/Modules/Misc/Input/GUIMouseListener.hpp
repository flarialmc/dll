#pragma once

#include <format>
#include <windows.h>
#include <unknwn.h>
#include "../../../../Client.hpp"
#include "../../../Manager.hpp"

class ClickData {
public:
    double timestamp;  // Milliseconds since some reference point
    // ... other click data members ...
};
    
class RateLimiter {
public:
    RateLimiter(float cps) : rate(cps), tokens(cps), lastRefill(std::chrono::steady_clock::now()) {
    }

    bool allow() {
        if (rate <= 0.0f) return true;

        using Clock = std::chrono::steady_clock;
        auto now = Clock::now();
        
        // Calculate time elapsed since last refill
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRefill).count();
        
        // Refill tokens based on elapsed time, but limit refill to prevent bursts
        if (elapsed > 0) {
            // Limit the elapsed time to prevent large token refills after inactivity
            double cappedElapsed = std::min(static_cast<double>(elapsed), 1000.0 / rate); // Max 1 second worth of tokens
            double tokensToAdd = (static_cast<double>(rate) * cappedElapsed) / 1000.0;
            tokens = std::min(static_cast<double>(rate), tokens + tokensToAdd);
            lastRefill = now;
        }

        // Check if we have enough tokens
        if (tokens >= 1.0) {
            tokens -= 1.0;
            return true;
        }
        
        return false;
    }

    void setRate(float cps) {
        rate = cps;
        // Don't reset tokens to avoid sudden bursts when rate changes
        if (tokens > static_cast<double>(rate)) {
            tokens = static_cast<double>(rate); // Cap tokens to new rate
        }
    }

private:
    float rate;
    double tokens;
    std::chrono::steady_clock::time_point lastRefill;
};

class GUIMouseListener : public Listener {
public:
    static inline RateLimiter leftLimiter = { 0.0f };
    static inline RateLimiter rightLimiter = { 0.0f };
    static inline std::vector<ClickData> leftClickList;
    static inline std::vector<ClickData> rightClickList;
    static inline std::deque<std::chrono::steady_clock::time_point> leftClicks;
    static inline std::deque<std::chrono::steady_clock::time_point> rightClicks;
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastRightClick;
    static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastLeftClick;

    void onMouse(MouseEvent &event) {
        GUIMouseListener::handleMouse(event);
    };

    static void handleMouse(MouseEvent& event) {
        if (event.getMouseX() != 0) MC::mousePos.x = event.getMouseX();
        if (event.getMouseY() != 0) MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();
        if (event.getButton() != MouseButton::None) MC::lastMouseButton = event.getButton();

        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) {
            MC::held = true;
            if (event.getButton() == MouseButton::Left) MC::heldLeft = true;
            if (event.getButton() == MouseButton::Right) MC::heldRight = true;
        }
        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) {
            MC::held = false;
            if (event.getButton() == MouseButton::Left) MC::heldLeft = false;
            if (event.getButton() == MouseButton::Right) MC::heldRight = false;
        }

        auto limiterMod = ModuleManager::getModule("CPS Limiter");
        //if (!limiterMod) return;

        GUIMouseListener::leftLimiter.setRate(limiterMod->getOps<int>("Left"));
        GUIMouseListener::rightLimiter.setRate(limiterMod->getOps<int>("Right"));

        using MB = MouseButton;

        if (event.getButton() == MB::Left && MC::held) {
            if (!GUIMouseListener::leftLimiter.allow() && limiterMod->getOps<bool>("enabled")) {
                event.cancel();
                return;
            }
            GUIMouseListener::AddLeftClick();
        }
        else if (event.getButton() == MB::Right && MC::held) {
            if (!GUIMouseListener::rightLimiter.allow() && limiterMod->getOps<bool>("enabled")) {
                event.cancel();
                return;
            }
            GUIMouseListener::AddRightClick();
        }
    }

    GUIMouseListener() {
        Listen(this, MouseEvent, &GUIMouseListener::onMouse);
    }

    ~GUIMouseListener() {
        Deafen(this, MouseEvent, &GUIMouseListener::onMouse);
    }

    static void AddLeftClick() {
        ClickData click{};
        click.timestamp = GUIMouseListener::Microtime();
        GUIMouseListener::leftClickList.insert(GUIMouseListener::leftClickList.begin(), click);

        if (GUIMouseListener::leftClickList.size() >= 100) {
            GUIMouseListener::leftClickList.pop_back();
        }
    }

    static void AddRightClick() {
        ClickData click{};
        click.timestamp = GUIMouseListener::Microtime();
        GUIMouseListener::rightClickList.insert(GUIMouseListener::rightClickList.begin(), click);

        if (GUIMouseListener::rightClickList.size() >= 100) {
            GUIMouseListener::rightClickList.pop_back();
        }
    }

    static double Microtime() {
        return (double(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
    }

    static int GetLeftCPS() {
        if (GUIMouseListener::leftClickList.empty()) {
            return 0;
        }

        double currentMicros = GUIMouseListener::Microtime();
        auto count = std::count_if(GUIMouseListener::leftClickList.begin(), GUIMouseListener::leftClickList.end(), [currentMicros](const ClickData& click) {
            return (currentMicros - click.timestamp <= 1.0);
            });

        return (int)std::round(count);
    }

    static int GetRightCPS() {
        if (GUIMouseListener::rightClickList.empty()) {
            return 0;
        }

        double currentMicros = GUIMouseListener::Microtime();
        auto count = std::count_if(GUIMouseListener::rightClickList.begin(), GUIMouseListener::rightClickList.end(),
            [currentMicros](const ClickData& click) {
                return (currentMicros - click.timestamp <= 1.0);
            });

        return (int)std::round(count);
    }

    static double getCurrentTime() {
        using namespace std::chrono;
        return duration<double>(high_resolution_clock::now().time_since_epoch()).count();
    }

    // Helper to try a click against a given deque + limit.
    static bool tryLimit(std::deque<std::chrono::steady_clock::time_point>& q, float cpsLimit) {
        using Clock = std::chrono::steady_clock;
        auto now = Clock::now();
        auto oneSecAgo = now - std::chrono::seconds(1);

        while (!q.empty() && q.front() < oneSecAgo) {
            q.pop_front();
        }

        if (static_cast<int>(q.size()) >= static_cast<int>(cpsLimit)) {
            return false;
        }

        q.push_back(now);
        return true;
    }
};