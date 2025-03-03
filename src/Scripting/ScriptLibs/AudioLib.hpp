#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Utils/Audio.hpp>

class AudioLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("audio")
                .addFunction("play", [](const std::string& path) {
                    std::string file = Utils::getClientPath() + "\\" + file;
                    if (!std::filesystem::exists(file)) return;
                    Audio::play(file);
                })
            .endNamespace();
    }
};
