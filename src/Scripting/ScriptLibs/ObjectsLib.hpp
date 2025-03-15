#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>

class ObjectsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<Vec2<float>>("Vec2")
                .addConstructor<void(*)(float, float)>()
                .addProperty("x", &Vec2<float>::x)
                .addProperty("y", &Vec2<float>::y)
            .endClass()
            .beginClass<Vec3<float>>("Vec3")
                .addProperty("x", &Vec3<float>::x)
                .addProperty("y", &Vec3<float>::y)
                .addProperty("z", &Vec3<float>::z)
            .endClass();
    }
};