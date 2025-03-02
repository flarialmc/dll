#pragma once

#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>
#include <GUI/Engine/Engine.hpp>
#include <Scripting/ScriptManager.hpp>

class FlarialGUILib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<FlarialGUILib>("FlarialGUI")
                .addStaticFunction("RoundedRect", [](float x, float y, D2D_COLOR_F color, float width = 160.0f, float height = 75.0, float radiusX = 10.0f, float radiusY = 10.0f, ImDrawFlags flags = ImDrawFlags_RoundCornersAll) {
                    FlarialGUI::RoundedRect(x, y, color, width, height, radiusX, radiusY, flags);
                })
                .addStaticFunction("RoundedButton", [](int index, float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor, const wchar_t *text, float width = 160.0f, float height = 100.0f, float radiusX = 10.0f, float radiusY = 10.0f, bool glow = false) {
                    return FlarialGUI::RoundedButton(index, x, y, color, textColor, text, width, height, radiusX, radiusY, glow);
                })
                .addStaticFunction("Button", [](float x, float y, D2D_COLOR_F color, D2D_COLOR_F textColor, const wchar_t *text,float width = 100.0f, float height = 160.0f) {
                    return FlarialGUI::Button(x, y, color, textColor, text, width, height);
                })
                .addStaticFunction("image", [](const std::string &imageName, const LuaRef& rect) {
                    FlarialGUI::image(imageName, toD2D1Rect(rect));
                })
                .addStaticFunction("FlarialText", [](float x, float y, const wchar_t *text, float width, float height, DWRITE_TEXT_ALIGNMENT alignment) {
                    FlarialGUI::FlarialText(x, y, text, width, height, alignment);
                })
                .addStaticFunction("FlarialTextWithFont", [](float x, float y, std::string text, float width, float height, float fontSize) {
                    return FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), width, height,
                        static_cast<DWRITE_TEXT_ALIGNMENT>(static_cast<int>(DWRITE_TEXT_ALIGNMENT_LEADING)),
                        fontSize,
                        static_cast<DWRITE_FONT_WEIGHT>(static_cast<int>(DWRITE_FONT_WEIGHT_NORMAL)));
                })
                .addStaticFunction("SetScrollView", [](float x, float y, float width, float height) {
                    FlarialGUI::SetScrollView(x, y, width, height);
                })
                .addStaticFunction("UnsetScrollView", []() {
                    FlarialGUI::UnsetScrollView();
                })
                .addStaticFunction("GetCenterXY", [](float rectWidth = 160.0f, float rectHeight = 75.0f) {
                    return FlarialGUI::GetCenterXY(rectWidth, rectHeight);
                })
                .addStaticFunction("Scrollbar", [](float x, float y, float width, float height, float radius) {
                    FlarialGUI::ScrollBar(x, y, width, height, radius);
                })
                .addStaticFunction("SetWindowRect", [](float x, float y, float width, float height, int currentNum, float fixer = 0) {
                    FlarialGUI::SetWindowRect(x, y, width, height, currentNum, fixer);
                })
                .addStaticFunction("UnsetWindowRect", []() {
                    FlarialGUI::UnsetWindowRect();
                })
                .addStaticFunction("NormalRender", [](int index, std::string text, lua_State* L) {
                    auto* script = ScriptManager::getModuleByState(L);
                    script->normalRender(index, text);
                })
            .endClass();
    }
};