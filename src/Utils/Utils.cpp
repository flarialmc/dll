#include <Windows.h>
#include "Utils.hpp"
#include "Logger/Logger.hpp"
#include <sstream>

std::string Utils::getRoamingPath()
{
    char* path = nullptr;
    size_t length;

    _dupenv_s(&path, &length, "appdata");

    if (path == nullptr)
        return "";

    return std::string(path) + "\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState";
};



std::string Utils::GetKeyAsString(int key, bool isCapital) {

    if (key == 32) { // Spacebar key
        return " ";
    }

    if(key == 18) return "ALT";
    if(key == 17) return "CTRL";
    if(key == 16) return "SHIFT";


    if (isCapital) {
        switch (key) {
            // A - Z
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
        }
    } else {
        switch (key) {
            // a - z
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
            case 65: return "a";
            case 66: return "b";
            case 67: return "c";
            case 68: return "d";
            case 69: return "e";
            case 70: return "f";
            case 71: return "g";
            case 72: return "h";
            case 73: return "i";
            case 74: return "j";
            case 75: return "k";
            case 76: return "l";
            case 77: return "m";
            case 78: return "n";
            case 79: return "o";
            case 80: return "p";
            case 81: return "q";
            case 82: return "r";
            case 83: return "s";
            case 84: return "t";
            case 85: return "u";
            case 86: return "v";
            case 87: return "w";
            case 88: return "x";
            case 89: return "y";
            case 90: return "z";
        }
    }
    // Return empty string for unsupported key codes or non-alphabetic keys
    return "";
};

int Utils::GetStringAsKey(const std::string& str) {


    if(str == "ALT" || str == "alt") return 18;
    if(str == "CTRL" || str == "ctrl") return 17;
    if(str == "SHIFT" || str == "shift") return 16;
    if(str == " ") return 32;

    char c = str[0];

    return static_cast<int>(std::toupper(c));

    // Unsupported character or non-alphabetic key
    return -1;
}

 std::vector<int> Utils::GetStringAsKeys(const std::string str) {
    std::vector<int> keys;

    if (str.empty()) {
        // Empty string
        return keys;
    }

    std::stringstream iss(str);
    std::string token;
    while (std::getline(iss, token, '+')) {
        int key = Utils::GetStringAsKey(token);
        if (key != -1) {
            keys.push_back(key);
        }
    }

    return keys;
}
