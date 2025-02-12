#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include <string>
#include <codecvt>
#include "../../SDK/Client/Actor/Player.hpp"
#include "../LUAHelper.hpp"
#include "SDK/SDK.hpp"
#include <algorithm>

namespace LuaInventory {
    int lua_getItemStack(lua_State *L){
        if (SDK::clientInstance == nullptr) {
            return 0;
        }
        auto *player_inventory = static_cast<Inventory*>(lua_touserdata(L, 1));

        int id = luaL_checkinteger(L, 2);

        lua_pushlightuserdata(L, player_inventory->getItem(id));

        return 1;
    }

    void registerFunctions(lua_State *L) {

        LUAHelper(L)
                .getClass("Inventory")
                .registerFunction("getItemStack", lua_getItemStack);
    }
}

namespace LuaItem {
    void registerFunctions(lua_State *L) {
        LUAHelper(L)
        .getClass("ItemStack")
                .registerLambdaFunction("getName", [](lua_State* L) -> int {
                    auto *stack = static_cast<ItemStack*>(lua_touserdata(L, 1));
                    if (stack->getItem() == nullptr) {
                        lua_pushstring(L, "L");
                        return 0;
                    }

                    lua_pushstring(L, stack->getItem()->name.c_str());
                    return 1;
                })
                .registerLambdaFunction("getCount", [](lua_State* L) -> int {
                    auto *stack = static_cast<ItemStack*>(lua_touserdata(L, 1));
                    if (stack->getItem() == nullptr) {
                        return 0;
                    }
                    lua_pushinteger(L, stack->count);
                    return 1;
                }).registerLambdaFunction("getDamage", [](lua_State* L) -> int {
                    auto *stack = static_cast<ItemStack*>(lua_touserdata(L, 1));
                    if (stack->getItem() == nullptr) {
                        return 0;
                    }
                    lua_pushinteger(L, stack->getDamageValue());
                    return 1;
                }).registerLambdaFunction("getMaxDamage", [](lua_State* L) -> int {
                    auto *stack = static_cast<ItemStack*>(lua_touserdata(L, 1));
                    if (stack->getItem() == nullptr) {
                        return 0;
                    }
                    lua_pushinteger(L, stack->getMaxDamage());
                    return 1;
                }).registerLambdaFunction("isEnchanted", [](lua_State* L) -> int {
                    auto *stack = static_cast<ItemStack*>(lua_touserdata(L, 1));
                    if (stack->getItem() == nullptr) {
                        return 0;
                    }
                    lua_pushboolean(L, stack->isEnchanted());
                    return 1;
                });

    }
}