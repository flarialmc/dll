#pragma once

#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/Module/Manager.hpp"
#include "../ScriptModuleBase.hpp"
#include <string>
#include <codecvt>


namespace LuaSettings {

    int lua_CreateSetting(lua_State *L) {
        std::string name = luaL_checkstring(L, 1);
        float a = luaL_checkinteger(L, 2);

        for (const auto &pair: ModuleManager::moduleMap) {
            if (pair.second != nullptr) {
                try {
                    auto script = reinterpret_cast<ScriptModuleBase *>(pair.second.get());
                    if(!script) continue;

                    if (script->module_lua_state == L) {
                        if (script->settings.getSettingByName<float>(name) == nullptr)
                            script->settings.addSetting(name, a);
                        break;
                    }
                } catch (const std::bad_cast &e) {

                }

            }
        }

        return 0;
    }

    void registerSetting(lua_State *L) {
        lua_newtable(L);
        lua_pushcfunction(L, lua_CreateSetting);
        lua_setfield(L, -2, "CreateSetting");


        lua_setglobal(L, "Settings");
    }
}