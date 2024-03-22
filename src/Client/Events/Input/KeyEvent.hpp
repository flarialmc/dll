
#pragma once

#include <array>
#include "../Event.hpp"
#include "../Cancellable.hpp"
#include "../../../Utils/Utils.hpp"

enum class ActionType {
    Released = 0,
    Pressed = 1
};

class KeyEvent : public Event, public Cancellable {

public:
    int key;
    int action;
    std::array<bool, 256> keys{};

    KeyEvent(int key, int action, const std::array<bool, 256> &keys) : Event() {
        this->key = key;
        this->action = action;
        this->keys = keys;
    };


    [[nodiscard]] int getKey() const {
        return key;
    }

    void setKey(int e) {
        this->key = e;
    }

    [[nodiscard]] int getAction() const {
        return this->action;
    }

    [[nodiscard]] std::string getKeyAsString(bool isCapital) const {

        return Utils::getKeyAsString(key, isCapital, false);

    }


    int setAction(int e) {
        this->action = e;
    }

    std::string getPressedKeysAsString() {

        std::string result;
        int i = 0;
        bool found = false;

        for (bool b: keys) {

            if (b) {
                found = true;
                if (!result.empty()) {
                    result += "+";
                }
                result += Utils::getKeyAsString(i, true);
            }

            i++;
        }

        if (found)
            return result;
        else return "no";

    }

    [[nodiscard]] std::array<bool, 256> getPressedKeysAsArray() const {
        return this->keys;
    }
};



