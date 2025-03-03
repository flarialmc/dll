#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Utils/Audio.hpp>

class AudioLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<AudioLib>("audio")
                .addFunction("play", [](const std::string& path) {
                    std::string fullPath = Utils::getClientPath() + path;
                    Audio::play(&fullPath[0]);
                })
            .endClass();
    }
};
