#include <lua.hpp>
#include <d2d1helper.h>
#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../Client/GUI/Engine/Constraints.hpp"
#include <string>
#include <codecvt>

namespace LuaConstraints {
    int lua_GUI_RelativeConstraint(lua_State *L) {
        float percent = luaL_checknumber(L, 1);
        std::string dimension = luaL_checkstring(L, 2);

        float value = Constraints::RelativeConstraint(percent, dimension, true);

        lua_pushnumber(L, value);
        return 1;
    }

    void registerConstraints(lua_State *L) {
        lua_newtable(L);

        lua_pushcfunction(L, lua_GUI_RelativeConstraint);
        lua_setfield(L, -2, "RelativeConstraint");
        
        lua_setglobal(L, "Constraints");
    }
}
