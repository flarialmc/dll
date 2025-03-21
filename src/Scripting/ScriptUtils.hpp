#pragma once
#include <chrono>
#include <unordered_map>
#include <unordered_set>

class ScriptUtils {
public:
    static bool isKeyDown(const int key) {
        if (!pressedKeys.contains(key)) return false;

        auto now = std::chrono::steady_clock::now();
        auto pressTime = pressTimes[key];
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pressTime).count();

        if (elapsed > 300) { // 5 minutes
            pressedKeys.erase(key); // Force release
            return false;
        }

        return true;
    }

    static void onKeyPressed(const int key) {
        pressedKeys.insert(key);
        pressTimes[key] = std::chrono::steady_clock::now();
    }

    static void onKeyReleased(const int key) {
        pressedKeys.erase(key);
        pressTimes.erase(key);
    }

private:
    static inline std::unordered_set<int> pressedKeys;
    static inline std::unordered_map<int, std::chrono::time_point<std::chrono::steady_clock>> pressTimes;
};
