
#pragma once

#include <array>
#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"

enum class ActionType {
    RELEASED = 0,
    PRESSED = 1
};

class KeyEvent : public Event, public Cancellable {

public:
    int key;
    int action;
    std::array<bool, 256> keys;

    KeyEvent(int key, int action, const std::array<bool, 256>& keys) : Event() {
        this->key = key;
        this->action = action;
        this->keys = keys;
    };


    [[nodiscard]] int GetKey() const {
        return key;
    }

    void SetKey(int e) {
        this->key = e;
    }

    [[nodiscard]] int GetAction() const {
        return this->action;
    }

    std::string GetKeyAsString(bool isCapital) {

        return Utils::GetKeyAsString(key, isCapital, false);

    }


    int SetAction(int e) {
        this->action = e;
    }

    std::string GetPressedKeysAsString() {

        std::string result;
        int i = 0;
        bool found = false;

        for(bool b : keys) {

            if (b) {
                found = true;
                if (!result.empty()) {
                    result += "+";
                }
                result += Utils::GetKeyAsString(i, true);
            }

            i++;
        }

        if(found)
        return result;
        else return "no";

    }

    std::array<bool, 256> GetPressedKeysAsArray() {
        return this->keys;
    }
};



