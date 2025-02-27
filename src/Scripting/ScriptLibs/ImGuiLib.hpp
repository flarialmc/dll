#pragma once

#include "ScriptLib.hpp"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

class ImGuiLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        registerFunction(state, [](lua_State* L) -> int {
            const char* title = luaL_checkstring(L, 1);
            bool open = true;
            if (lua_gettop(L) >= 2) {
                open = lua_toboolean(L, 2);
            }
            bool ret = ImGui::Begin(title, &open);
            lua_pushboolean(L, ret);
            return 1;
        }, "Begin", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            ImGui::End();
            return 0;
        }, "End", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            const char* text = luaL_checkstring(L, 1);
            ImGui::Text("%s", text);
            return 0;
        }, "Text", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            const char* label = luaL_checkstring(L, 1);
            bool pressed = ImGui::Button(label);
            lua_pushboolean(L, pressed);
            return 1;
        }, "Button", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            const char* label = luaL_checkstring(L, 1);
            bool value = lua_toboolean(L, 2);
            ImGui::Checkbox(label, &value);
            lua_pushboolean(L, value);
            return 1;
        }, "Checkbox", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            const char* label = luaL_checkstring(L, 1);
            float value = (float)luaL_checknumber(L, 2);
            float min = (float)luaL_checknumber(L, 3);
            float max = (float)luaL_checknumber(L, 4);
            ImGui::SliderFloat(label, &value, min, max);
            lua_pushnumber(L, value);
            return 1;
        }, "SliderFloat", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            const char* label = luaL_checkstring(L, 1);
            const char* text = luaL_checkstring(L, 2);
            char buffer[256];
            std::strncpy(buffer, text, sizeof(buffer));
            buffer[255] = '\0';
            if (ImGui::InputText(label, buffer, sizeof(buffer))) {
                lua_pushstring(L, buffer);
            } else {
                lua_pushstring(L, text);
            }
            return 1;
        }, "InputText", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            float x = (float)luaL_checknumber(L, 1);
            float y = (float)luaL_checknumber(L, 2);
            ImGui::SetCursorPos(ImVec2(x, y));
            return 0;
        }, "SetCursorPos", "ImGui");

        registerFunction(state, [](lua_State* L) -> int {
            ImDrawList* drawList = ImGui::GetForegroundDrawList();

            float x1 = (float)luaL_checknumber(L, 2);
            float y1 = (float)luaL_checknumber(L, 3);
            float x2 = (float)luaL_checknumber(L, 4);
            float y2 = (float)luaL_checknumber(L, 5);

            luaL_checktype(L, 6, LUA_TTABLE);
            lua_rawgeti(L, 6, 1);
            int r = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 6, 2);
            int g = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 6, 3);
            int b = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 6, 4);
            int a = luaL_optinteger(L, -1, 255);
            lua_pop(L, 1);
            float thickness = (float)luaL_optnumber(L, 7, 1.0f);

            ImU32 col = IM_COL32(r, g, b, a);
            drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), col, thickness);

            return 0;
        }, "AddLine", "ImGui.DrawList");

        registerFunction(state, [](lua_State* L) -> int {
            ImDrawList* drawList = ImGui::GetForegroundDrawList();

            float x1 = (float)luaL_checknumber(L, 2);
            float y1 = (float)luaL_checknumber(L, 3);
            float x2 = (float)luaL_checknumber(L, 4);
            float y2 = (float)luaL_checknumber(L, 5);

            luaL_checktype(L, 6, LUA_TTABLE);
            lua_rawgeti(L, 6, 1);
            int r = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 6, 2);
            int g = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 6, 3);
            int b = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 6, 4);
            int a = luaL_optinteger(L, -1, 255);
            lua_pop(L, 1);
            float rounding = (float)luaL_optnumber(L, 7, 0.0f);
            int flags = (int)luaL_optnumber(L, 8, 0);
            float thickness = (float)luaL_optnumber(L, 9, 1.0f);

            ImU32 col = IM_COL32(r, g, b, a);
            drawList->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), col, rounding, flags, thickness);

            return 0;
        }, "AddRect", "ImGui.DrawList");

        registerFunction(state, [](lua_State* L) -> int {
            ImDrawList* drawList = ImGui::GetForegroundDrawList();

            float x = (float)luaL_checknumber(L, 2); // Start at 2, since self is at 1
            float y = (float)luaL_checknumber(L, 3);

            luaL_checktype(L, 4, LUA_TTABLE);
            lua_rawgeti(L, 4, 1);
            int r = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 4, 2);
            int g = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 4, 3);
            int b = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
            lua_rawgeti(L, 4, 4);
            int a = luaL_optinteger(L, -1, 255);
            lua_pop(L, 1);
            const char* text = luaL_checkstring(L, 5);

            ImU32 col = IM_COL32(r, g, b, a);
            drawList->AddText(ImVec2(x, y), col, text);

            return 0;
        }, "AddText", "ImGui.DrawList");

        registerFunction(state, [](lua_State* L) -> int {
            ImGuiIO& io = ImGui::GetIO();
            lua_pushnumber(L, io.Framerate);
            return 1;
        }, "FPS", "ImGui.IO");

        registerFunction(state, [](lua_State* L) -> int {
            ImGuiIO& io = ImGui::GetIO();
            lua_pushnumber(L, io.DeltaTime);
            return 1;
        }, "DeltaTime", "ImGui.IO");

        registerFunction(state, [](lua_State* L) -> int {
            ImGuiIO& io = ImGui::GetIO();
            lua_newtable(L);
            lua_pushnumber(L, io.DisplaySize.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, io.DisplaySize.y);
            lua_setfield(L, -2, "y");
            return 1;
        }, "DisplaySize", "ImGui.IO");

        registerFunction(state, [](lua_State* L) -> int {
            ImGuiIO& io = ImGui::GetIO();
            lua_newtable(L);
            lua_pushnumber(L, io.MousePos.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, io.MousePos.y);
            lua_setfield(L, -2, "y");
            return 1;
        }, "MousePos", "ImGui.IO");
    }
};