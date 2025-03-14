#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Client/GUI/Engine/Engine.hpp>

class ClientLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("client")
                .addFunction("notify", [](const std::string& message) {
                    if (message.empty()) return;
                    FlarialGUI::Notify(message);
                })
                .addFunction("crash", []() {
                    __fastfail(0);
                })
                .addFunction("freeMouse", []() {
                    if (!SDK::clientInstance) return;
                    SDK::clientInstance->releaseMouse();
                })
                .addFunction("grabMouse", []() {
                    if (!SDK::clientInstance) return;
                    SDK::clientInstance->grabMouse();
                })
                .addFunction("getScreenName", []() -> std::string {
                    if (!SDK::clientInstance) return "";
                    return SDK::clientInstance->getScreenName();
                })
            .endNamespace();
    }
};