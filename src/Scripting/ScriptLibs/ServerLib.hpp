#pragma once

#include "ScriptLib.hpp"

class ServerLib : public ScriptLib {
public:
    void initialize(lua_State* L) override {
        using namespace luabridge;

        getGlobalNamespace(L)
            .beginNamespace("server")
                .addFunction("ip", []() {
                    return SDK::getServerIP();
                })
                .addFunction("ping", []() {
                    return SDK::getServerPing();
                })
            .endNamespace();
    }
};