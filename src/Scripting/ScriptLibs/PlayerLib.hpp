#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <SDK/SDK.hpp>
#include <SDK/Client/Actor/Actor.hpp>

class PlayerLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<Actor>("player")
                .addFunction("name", &Actor::getNametag)
                .addFunction("hurtTime", &Actor::getHurtTime)
                .addFunction("position", [](Actor* actor) {
                    return *actor->getPosition();
                })
            .endClass();
    }
};