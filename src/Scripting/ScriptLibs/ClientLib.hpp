#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Client/GUI/Engine/Engine.hpp>

class ClientLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        registerFunction(state, [](lua_State* L) -> int {
            const char* message = luaL_checkstring(L, 1);
            FlarialGUI::Notify(message);
            return 1;
        }, "notify", "client");
    }
};