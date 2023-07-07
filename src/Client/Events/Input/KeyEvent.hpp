
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
        if (this->key == 32) { // Spacebar key
            return " ";
        }

        if (isCapital) {
            switch (this->key) {
                case 48: return ")";
                case 49: return "!";
                case 50: return "@";
                case 51: return "#";
                case 52: return "$";
                case 53: return "%";
                case 54: return "^";
                case 55: return "&";
                case 56: return "*";
                case 57: return "(";
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
                case 40: return "(";
                case 41: return ")";
                case 37: return "%";
                case 91: return "{";
                case 93: return "}";
                case 18: return "\\";
                case 191: return "?";
                case 186: return ":";
                case 187: return "+";
                case 219: return "{";
                case 221: return "}";
            }
        } else {
            switch (this->key) {
                case 48: return "0";
                case 49: return "1";
                case 50: return "2";
                case 51: return "3";
                case 52: return "4";
                case 53: return "5";
                case 54: return "6";
                case 55: return "7";
                case 56: return "8";
                case 57: return "9";
                case 97: return "a";
                case 98: return "b";
                case 99: return "c";
                case 219: return "[";
                case 221: return "]";
                case 100: return "d";
                case 101: return "e";
                case 102: return "f";
                case 103: return "g";
                case 104: return "h";
                case 105: return "i";
                case 106: return "j";
                case 107: return "k";
                case 108: return "l";
                case 109: return "m";
                case 110: return "n";
                case 111: return "o";
                case 112: return "p";
                case 113: return "q";
                case 114: return "r";
                case 115: return "s";
                case 116: return "t";
                case 117: return "u";
                case 118: return "v";
                case 119: return "w";
                case 120: return "x";
                case 121: return "y";
                case 122: return "z";
                case 18: return "|";
                case 186: (std::string)";";
                case 191: return "/";
                case 187: return "=";
            }
        }

        // Return empty string for unsupported key codes or non-alphabetic keys
        return "";
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
                result += Utils::GetKeyAsString(i);
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



