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
            .endNamespace();
    }
};