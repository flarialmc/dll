#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <SDK/SDK.hpp>
#include <SDK/Client/Actor/Actor.hpp>

class sLocalPlayer {
public:
    static std::string name() {
        auto player = SDK::clientInstance->getLocalPlayer();
        return player ? *player->getNametag() : "";
    }
    static Vec3<float> position() {
        auto player = SDK::clientInstance->getLocalPlayer();
        return player ? *player->getPosition() : Vec3<float>(0.0f, 0.0f, 0.0f);
    }
    static int hurtTime() {
        auto player = SDK::clientInstance->getLocalPlayer();
        return player ? player->getHurtTime() : 0;
    }
};

class PlayerLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<sLocalPlayer>("player")
                .addStaticFunction("name", &sLocalPlayer::name)
                .addStaticFunction("hurtTime", &sLocalPlayer::hurtTime)
                .addStaticFunction("position", &sLocalPlayer::position)
            .endClass();
    }
};