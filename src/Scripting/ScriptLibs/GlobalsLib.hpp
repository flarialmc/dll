#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Utils/WinrtUtils.hpp>

class GlobalsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        registerFunction(state, [](lua_State* L) -> int {
            const char* text = luaL_checkstring(L, 1);
            WinrtUtils::setClipboard(std::string(text));

            return 0;
        }, "setClipboard");

        registerFunction(state, [](lua_State* L) -> int {
            std::string clip = WinrtUtils::getClipboard();
            lua_pushstring(L, clip.c_str());

            return 1;
        }, "getClipboard");
    }
};