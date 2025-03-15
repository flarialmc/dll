#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <Client/GUI/Engine/Engine.hpp>

class ConstraintsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<ConstraintsLib>("Constraints")
                .addStaticFunction("PercentageConstraint", [](float percentage, const std::string &edge, bool ignore_stack = false) {
                    return Constraints::PercentageConstraint(percentage, edge, ignore_stack);
                })
                .addStaticFunction("RelativeConstraint", [](float percent, const std::string& dimension = "height", bool ignore_stack = false) {
                    return Constraints::RelativeConstraint(percent, dimension, ignore_stack);
                })
                .addStaticFunction("CenterConstraint", [](float width, float height, const std::string &axis = "no", float xModifier = 0.f, float yModifier = 0.f, bool ignoreStack = false) {
                    return Constraints::CenterConstraint(width, height, axis, xModifier, yModifier, ignoreStack);
                })
                .addStaticFunction("RoundingConstraint", [](float radiusX, float radiusY) {
                    return Constraints::CenterConstraint(radiusX, radiusY);
                })
                .addStaticFunction("SpacingConstraint", [](float percentage, float dimension) {
                    return Constraints::SpacingConstraint(percentage, dimension);
                })
                .addStaticFunction("FontScalar", [](float dimension) {
                    return Constraints::FontScaler(dimension);
                })
                .addStaticFunction("CalculatePercentage", [](float x, float y, float adjustWidth = 0, float adjustHeight = 0) {
                    return Constraints::CalculatePercentage(x, y, adjustWidth, adjustHeight);
                })
            .endClass();
    }
};