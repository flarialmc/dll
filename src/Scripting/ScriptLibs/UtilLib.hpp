#pragma once

/**
 * @file UtilLib.hpp
 * @brief Lua bindings for utility functions
 *
 * Provides scripts with various utility functions for math, strings,
 * time, colors, and system interaction.
 *
 * ## Available Functions
 *
 * ### Input
 * - `util.keyToString(keyCode)` - Convert key code to readable name
 *
 * ### Clipboard
 * - `util.setClipboard(text)` - Copy text to clipboard
 * - `util.getClipboard()` - Get clipboard contents
 *
 * ### Math
 * - `util.lerp(a, b, t)` - Linear interpolation
 * - `util.clamp(value, min, max)` - Clamp value to range
 * - `util.map(value, inMin, inMax, outMin, outMax)` - Map value from one range to another
 * - `util.distance(x1, y1, z1, x2, y2, z2)` - 3D distance
 * - `util.distance2d(x1, y1, x2, y2)` - 2D distance
 * - `util.round(value, decimals)` - Round to decimal places
 * - `util.random(min, max)` - Random number in range
 * - `util.randomInt(min, max)` - Random integer in range
 *
 * ### String
 * - `util.split(str, delimiter)` - Split string into table
 * - `util.trim(str)` - Remove leading/trailing whitespace
 * - `util.startsWith(str, prefix)` - Check if string starts with prefix
 * - `util.endsWith(str, suffix)` - Check if string ends with suffix
 * - `util.contains(str, substring)` - Check if string contains substring
 * - `util.replace(str, find, replace)` - Replace all occurrences
 * - `util.lower(str)` - Convert to lowercase
 * - `util.upper(str)` - Convert to uppercase
 *
 * ### Color
 * - `util.hexToRgb(hex)` - Convert hex color to RGB table
 * - `util.rgbToHex(r, g, b)` - Convert RGB to hex string
 * - `util.hsvToRgb(h, s, v)` - Convert HSV to RGB table
 *
 * ### Time
 * - `util.time()` - Current time in milliseconds
 * - `util.formatTime(ms, format)` - Format milliseconds to string
 *
 * @example
 * ```lua
 * -- Smooth value interpolation
 * local smoothValue = 0
 * function onTick()
 *     smoothValue = util.lerp(smoothValue, targetValue, 0.1)
 * end
 *
 * -- Format player distance
 * function onRender()
 *     local x1, y1, z1 = player.position()
 *     local dist = util.distance(x1, y1, z1, targetX, targetY, targetZ)
 *     client.notify("Distance: " .. util.round(dist, 1) .. " blocks")
 * end
 * ```
 */

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include "Utils/PlatformUtils.hpp"
#include <random>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cmath>

class UtilLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("util")
                // === Input ===
                .addFunction("keyToString", [](double key) {
                    int intKey = static_cast<int>(std::floor(key));
                    return Utils::getKeyAsString(intKey);
                })

                // === Clipboard ===
                .addFunction("setClipboard", [](const std::string& text) {
                    PlatformUtils::setClipboard(text);
                })
                .addFunction("getClipboard", []() -> std::string {
                    return PlatformUtils::getClipboard();
                })

                // === Math functions ===
                .addFunction("lerp", [](double a, double b, double t) -> double {
                    return a + (b - a) * t;
                })
                .addFunction("clamp", [](double value, double minVal, double maxVal) -> double {
                    return std::clamp(value, minVal, maxVal);
                })
                .addFunction("map", [](double value, double inMin, double inMax, double outMin, double outMax) -> double {
                    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
                })
                .addFunction("distance", [](double x1, double y1, double z1, double x2, double y2, double z2) -> double {
                    double dx = x2 - x1;
                    double dy = y2 - y1;
                    double dz = z2 - z1;
                    return std::sqrt(dx*dx + dy*dy + dz*dz);
                })
                .addFunction("distance2d", [](double x1, double y1, double x2, double y2) -> double {
                    double dx = x2 - x1;
                    double dy = y2 - y1;
                    return std::sqrt(dx*dx + dy*dy);
                })
                .addFunction("round", [](double value, int decimals) -> double {
                    double multiplier = std::pow(10.0, decimals);
                    return std::round(value * multiplier) / multiplier;
                })
                .addFunction("random", [](double minVal, double maxVal) -> double {
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_real_distribution<double> dis(minVal, maxVal);
                    return dis(gen);
                })
                .addFunction("randomInt", [](int minVal, int maxVal) -> int {
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_int_distribution<int> dis(minVal, maxVal);
                    return dis(gen);
                })

                // === String functions ===
                .addFunction("trim", [](const std::string& str) -> std::string {
                    size_t start = str.find_first_not_of(" \t\n\r");
                    if (start == std::string::npos) return "";
                    size_t end = str.find_last_not_of(" \t\n\r");
                    return str.substr(start, end - start + 1);
                })
                .addFunction("startsWith", [](const std::string& str, const std::string& prefix) -> bool {
                    return str.size() >= prefix.size() &&
                           str.compare(0, prefix.size(), prefix) == 0;
                })
                .addFunction("endsWith", [](const std::string& str, const std::string& suffix) -> bool {
                    return str.size() >= suffix.size() &&
                           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
                })
                .addFunction("contains", [](const std::string& str, const std::string& substring) -> bool {
                    return str.find(substring) != std::string::npos;
                })
                .addFunction("replace", [](const std::string& str, const std::string& find, const std::string& replace) -> std::string {
                    std::string result = str;
                    size_t pos = 0;
                    while ((pos = result.find(find, pos)) != std::string::npos) {
                        result.replace(pos, find.length(), replace);
                        pos += replace.length();
                    }
                    return result;
                })
                .addFunction("lower", [](const std::string& str) -> std::string {
                    std::string result = str;
                    std::transform(result.begin(), result.end(), result.begin(),
                                  [](unsigned char c) { return std::tolower(c); });
                    return result;
                })
                .addFunction("upper", [](const std::string& str) -> std::string {
                    std::string result = str;
                    std::transform(result.begin(), result.end(), result.begin(),
                                  [](unsigned char c) { return std::toupper(c); });
                    return result;
                })

                // === Color functions ===
                .addFunction("hexToRgb", [](lua_State* L) -> int {
                    if (!lua_isstring(L, 1)) {
                        lua_newtable(L);
                        lua_pushnumber(L, 0); lua_setfield(L, -2, "r");
                        lua_pushnumber(L, 0); lua_setfield(L, -2, "g");
                        lua_pushnumber(L, 0); lua_setfield(L, -2, "b");
                        return 1;
                    }

                    std::string hex = lua_tostring(L, 1);
                    if (!hex.empty() && hex[0] == '#') hex = hex.substr(1);

                    int r = 0, g = 0, b = 0;
                    if (hex.length() == 6) {
                        r = std::stoi(hex.substr(0, 2), nullptr, 16);
                        g = std::stoi(hex.substr(2, 2), nullptr, 16);
                        b = std::stoi(hex.substr(4, 2), nullptr, 16);
                    }

                    lua_newtable(L);
                    lua_pushnumber(L, r); lua_setfield(L, -2, "r");
                    lua_pushnumber(L, g); lua_setfield(L, -2, "g");
                    lua_pushnumber(L, b); lua_setfield(L, -2, "b");
                    return 1;
                })
                .addFunction("rgbToHex", [](int r, int g, int b) -> std::string {
                    char hex[8];
                    snprintf(hex, sizeof(hex), "#%02x%02x%02x",
                            std::clamp(r, 0, 255),
                            std::clamp(g, 0, 255),
                            std::clamp(b, 0, 255));
                    return std::string(hex);
                })
                .addFunction("hsvToRgb", [](lua_State* L) -> int {
                    double h = luaL_checknumber(L, 1);  // 0-360
                    double s = luaL_checknumber(L, 2);  // 0-1
                    double v = luaL_checknumber(L, 3);  // 0-1

                    double c = v * s;
                    double x = c * (1 - std::abs(std::fmod(h / 60.0, 2) - 1));
                    double m = v - c;

                    double r = 0, g = 0, b = 0;
                    if (h < 60)      { r = c; g = x; b = 0; }
                    else if (h < 120) { r = x; g = c; b = 0; }
                    else if (h < 180) { r = 0; g = c; b = x; }
                    else if (h < 240) { r = 0; g = x; b = c; }
                    else if (h < 300) { r = x; g = 0; b = c; }
                    else              { r = c; g = 0; b = x; }

                    lua_newtable(L);
                    lua_pushnumber(L, static_cast<int>((r + m) * 255)); lua_setfield(L, -2, "r");
                    lua_pushnumber(L, static_cast<int>((g + m) * 255)); lua_setfield(L, -2, "g");
                    lua_pushnumber(L, static_cast<int>((b + m) * 255)); lua_setfield(L, -2, "b");
                    return 1;
                })

                // === Time functions ===
                .addFunction("time", []() -> double {
                    auto now = std::chrono::high_resolution_clock::now();
                    auto duration = now.time_since_epoch();
                    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
                })
                .addFunction("formatTime", [](double ms, const std::string& format) -> std::string {
                    int totalSeconds = static_cast<int>(ms / 1000);
                    int hours = totalSeconds / 3600;
                    int minutes = (totalSeconds % 3600) / 60;
                    int seconds = totalSeconds % 60;
                    int millis = static_cast<int>(ms) % 1000;

                    std::string result = format;

                    // Replace format tokens
                    auto replaceToken = [&result](const std::string& token, int value, int width) {
                        char buf[16];
                        snprintf(buf, sizeof(buf), "%0*d", width, value);
                        size_t pos = result.find(token);
                        if (pos != std::string::npos) {
                            result.replace(pos, token.length(), buf);
                        }
                    };

                    replaceToken("%H", hours, 2);
                    replaceToken("%M", minutes, 2);
                    replaceToken("%S", seconds, 2);
                    replaceToken("%m", millis, 3);

                    return result;
                })

                // === String split (needs special handling for table return) ===
                .addFunction("split", &UtilLib::split)

            .endNamespace();
    }

    // Split requires raw Lua API for table return
    static int split(lua_State* L) {
        const char* str = luaL_checkstring(L, 1);
        const char* delim = luaL_checkstring(L, 2);

        lua_newtable(L);

        std::string s(str);
        std::string d(delim);
        size_t pos = 0;
        int index = 1;

        while ((pos = s.find(d)) != std::string::npos) {
            lua_pushstring(L, s.substr(0, pos).c_str());
            lua_rawseti(L, -2, index++);
            s.erase(0, pos + d.length());
        }
        lua_pushstring(L, s.c_str());
        lua_rawseti(L, -2, index);

        return 1;
    }
};
