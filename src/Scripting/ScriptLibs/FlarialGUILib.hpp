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
                .addStaticFunction("Button", [](float x, float y, const LuaRef& colorTable, const LuaRef& textColorTable, const std::string& text, float width = 100.0f, float height = 160.0f) {
                    D2D1_COLOR_F color = toD2D1ColorF(colorTable);
                    D2D1_COLOR_F textColor = toD2D1ColorF(textColorTable);
                    return FlarialGUI::Button(x, y, color, textColor, FlarialGUI::to_wide(text).c_str(), width, height);
                })
                .addStaticFunction("Image", [](const std::string& file, const LuaRef& rectTable) {
                    D2D1_RECT_F rect = toD2D1Rect(rectTable);
                    if (!std::filesystem::exists(file)) return;
                    FlarialGUI::image(file, rect);
                })
                .addStaticFunction("Text", [](const LuaRef& posTable, const std::string& text, float width, float height, float fontSize = 200) {
                    ImVec2 pos = toImVec2(posTable);
                    return FlarialGUI::FlarialTextWithFont(pos.x, pos.y, FlarialGUI::to_wide(text).c_str(), width, height,
                        static_cast<DWRITE_TEXT_ALIGNMENT>(static_cast<int>(DWRITE_TEXT_ALIGNMENT_LEADING)),
                        fontSize,
                        static_cast<DWRITE_FONT_WEIGHT>(static_cast<int>(DWRITE_FONT_WEIGHT_NORMAL)));
                })
                .addStaticFunction("Render", [](std::string text, int index = 0, lua_State* L = nullptr) {
                    auto* script = ScriptManager::getModuleByState(L);
                    if (!script) return;
                    script->normalRender(index, text);
                })
            .endClass();
    }
};