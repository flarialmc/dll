//
// Created by ttf on 3/4/25.
//

#include "ScriptUtils.hpp"

#include "lua.hpp"
#include "Logger/Logger.hpp"

namespace ScriptUtils {
    int lua_pushArray(std::vector<double> &test, lua_State* L) {
        lua_newtable(L);
        //check if the stack has space
        if (!lua_checkstack(L,test.size())){
            Logger::error("Lua stack not big enough to push array");
            return -1;
        }
        try {
            for (int i = 0; i < test.size(); i++) {
                //push item of array
                lua_pushnumber(L, test[i]);
                //set as item of lua array (+1 cuz lua arrays are stupid)
                lua_rawseti(L, -2, i+1);
            }
        } catch(std::exception &e) {
            lua_pushnil(L);
            Logger::error("Failed to push array {}",e.what());
            return -1;
        }

    return 0;
    }
    int lua_pushArray(std::vector<std::string> test, lua_State* L) {
        lua_newtable(L);
        if (!lua_checkstack(L,test.size())){
            Logger::error("Lua stack not big enough to push array");
            return -1;
        }
        try {
            for (int i = 0; i < test.size(); i++) {
                lua_pushstring(L, test[i].c_str());
                lua_rawseti(L, -2, i+1);
            }
        }catch(std::exception &e) {
            lua_pushnil(L);
            Logger::error("Failed to push array {}",e.what());
            return -1;
        }
        return 0;
    }

    }


