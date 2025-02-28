#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Utils/WinrtUtils.hpp>

class UtilLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("util")
                .addFunction("keyToString", [](int key, bool isCapital, bool isKeybind) -> std::string {
                    return Utils::getKeyAsString(key, isCapital, isKeybind);
                })
                .addFunction("setClipboard", [](const std::string& text) {
                    WinrtUtils::setClipboard(text);
                })
                .addFunction("getClipboard", []() -> std::string {
                    return WinrtUtils::getClipboard();
                })
            .endNamespace();
    }
};
