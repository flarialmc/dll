
#pragma once
#include "../Event.hpp"
#include "../Cancellable.hpp"

enum class ActionType {
    RELEASED = 0,
    PRESSED = 1
};

class KeyEvent : public Event, public Cancellable {

public:
    int key;
    int action;

    KeyEvent(int key, int action) : Event() {
        this->key = key;
        this->action = action;
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

    [[nodiscard]] std::string GetKeyAsString() const {
        switch (this->key) {
            // A - Z
            case 65: return "A";
            case 66: return "B";
            case 67: return "C";
            case 68: return "D";
            case 69: return "E";
            case 70: return "F";
            case 71: return "G";
            case 72: return "H";
            case 73: return "I";
            case 74: return "J";
            case 75: return "K";
            case 76: return "L";
            case 77: return "M";
            case 78: return "N";
            case 79: return "O";
            case 80: return "P";
            case 81: return "Q";
            case 82: return "R";
            case 83: return "S";
            case 84: return "T";
            case 85: return "U";
            case 86: return "V";
            case 87: return "W";
            case 88: return "X";
            case 89: return "Y";
            case 90: return "Z";

                // 1 - 9
            case 49: return "1";
            case 50: return "2";
            case 51: return "3";
            case 52: return "4";
            case 53: return "5";
            case 54: return "6";
            case 55: return "7";
            case 56: return "8";
            case 57: return "9";

            default: return ""; // Return empty string for unsupported key codes
        }
    }

    int SetAction(int e) {
        this->action = e;
    }
};



