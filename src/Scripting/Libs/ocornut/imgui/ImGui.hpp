/*
 * ImGui implementation for Flarial Scripting
 * Original ImGui repo: https://github.com/ocornut/imgui
*/

#pragma once
#include <lua.hpp>
#include <vector>
#include "Scripting/LUAHelper.hpp"
#include "ImGui/imgui.h"

namespace LuaImGui {
    void registerLib(lua_State* L) {
        LUAHelper(L)
            .getLib("ocornut/imgui")
                .registerLambdaFunction("Begin", [](lua_State* L) -> int {
                    const char* name = luaL_checkstring(L, 1);
                    bool open = lua_toboolean(L, 2);
                    bool* p_open = open ? &open : nullptr;

                    bool result = ImGui::Begin(name, p_open);
                    lua_pushboolean(L, result);
                    return 1;
                })
                .registerLambdaFunction("End", [](lua_State* L) -> int {
                    ImGui::End();
                    return 0;
                })
                .registerLambdaFunction("Text", [](lua_State* L) -> int {
                    const char* text = luaL_checkstring(L, 1);
                    ImGui::Text("%s", text);
                    return 0;
                })
                .registerLambdaFunction("Button", [](lua_State* L) -> int {
                    const char* label = luaL_checkstring(L, 1);
                    bool pressed = ImGui::Button(label);
                    lua_pushboolean(L, pressed);
                    return 1;
                })
                .registerLambdaFunction("SliderFloat", [](lua_State* L) -> int {
                    const char* label = luaL_checkstring(L, 1);
                    float value = luaL_checknumber(L, 2);
                    float min = luaL_checknumber(L, 3);
                    float max = luaL_checknumber(L, 4);

                    if (ImGui::SliderFloat(label, &value, min, max)) {
                        lua_pushnumber(L, value);
                        return 1;
                    }
                    return 0;
                })

                .registerLambdaFunction("SliderInt", [](lua_State* L) -> int {
                    const char* label = luaL_checkstring(L, 1);
                    int value = luaL_checkinteger(L, 2);
                    int min = luaL_checkinteger(L, 3);
                    int max = luaL_checkinteger(L, 4);

                    if (ImGui::SliderInt(label, &value, min, max)) {
                        lua_pushinteger(L, value);
                        return 1;
                    }
                    return 0;
                })
                .registerLambdaFunction("Checkbox", [](lua_State* L) -> int {
                    const char* label = luaL_checkstring(L, 1);
                    bool state = lua_toboolean(L, 2);

                    if (ImGui::Checkbox(label, &state)) {
                        lua_pushboolean(L, state);
                        return 1;
                    }
                    return 0;
                })
                .registerLambdaFunction("CollapsingHeader", [](lua_State* L) -> int {
                    const char* label = luaL_checkstring(L, 1);
                    bool open = ImGui::CollapsingHeader(label);
                    lua_pushboolean(L, open);
                    return 1;
                })
                .registerLambdaFunction("ProgressBar", [](lua_State* L) -> int {
                    float fraction = luaL_checknumber(L, 1);
                    const char* overlay = luaL_optstring(L, 2, nullptr);
                    ImGui::ProgressBar(fraction, ImVec2(-FLT_MIN, 0.0f), overlay);
                    return 0;
                })
                .registerLambdaFunction("Separator", [](lua_State* L) -> int {
                    ImGui::Separator();
                    return 0;
                })
                .registerLambdaFunction("Spacing", [](lua_State* L) -> int {
                    ImGui::Spacing();
                    return 0;
                })
                .registerLambdaFunction("SameLine", [](lua_State* L) -> int {
                    ImGui::SameLine();
                    return 0;
                });
    }
}