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
                .addStaticFunction("RoundedButton", [](int index, float x, float y, const LuaRef& colorTable, const LuaRef& textColorTable, const std::string& text, float width = 160.0f, float height = 100.0f, float radiusX = 10.0f, float radiusY = 10.0f, bool glow = false) {
                    D2D1_COLOR_F color = toD2D1ColorF(colorTable);
                    D2D1_COLOR_F textColor = toD2D1ColorF(textColorTable);
                    return FlarialGUI::RoundedButton(index, x, y, color, textColor, FlarialGUI::to_wide(text).c_str(), width, height, radiusX, radiusY, glow);
                })
                .addStaticFunction("Button", [](float x, float y, const LuaRef& colorTable, const LuaRef& textColorTable, const std::string& text, float width = 100.0f, float height = 160.0f) {
                    D2D1_COLOR_F color = toD2D1ColorF(colorTable);
                    D2D1_COLOR_F textColor = toD2D1ColorF(textColorTable);
                    return FlarialGUI::Button(x, y, color, textColor, FlarialGUI::to_wide(text).c_str(), width, height);
                })
                .addStaticFunction("Image", [](const std::string &imageName, const LuaRef& rectTable) {
                    D2D1_RECT_F rect = toD2D1Rect(rectTable);
                    FlarialGUI::image(imageName, rect);
                })
                .addStaticFunction("FlarialText", [](float x, float y, const std::string& text, float width, float height) {
                    FlarialGUI::FlarialText(x, y, FlarialGUI::to_wide(text).c_str(), width, height,
                         static_cast<DWRITE_TEXT_ALIGNMENT>(static_cast<int>(DWRITE_TEXT_ALIGNMENT_LEADING)));
                })
                .addStaticFunction("FlarialTextWithFont", [](float x, float y, const std::string& text, float width, float height, float fontSize) {
                    return FlarialGUI::FlarialTextWithFont(x, y, FlarialGUI::to_wide(text).c_str(), width, height,
                        static_cast<DWRITE_TEXT_ALIGNMENT>(static_cast<int>(DWRITE_TEXT_ALIGNMENT_LEADING)),
                        fontSize,
                        static_cast<DWRITE_FONT_WEIGHT>(static_cast<int>(DWRITE_FONT_WEIGHT_NORMAL)));
                })
                .addStaticFunction("NormalRender", [](int index, std::string text, lua_State* L) {
                    auto* script = ScriptManager::getModuleByState(L);
                    script->normalRender(index, text);
                })
            .endClass();
    }
};