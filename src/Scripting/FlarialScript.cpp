#include "FlarialScript.hpp"

#include <Utils/Logger/Logger.hpp>

#include "ScriptSettings/ScriptSettingManager.hpp"
#include "ScriptLibs/ScriptLib.hpp"
#include "ScriptManager.hpp"

#include "ScriptLibs/FSLib.hpp"
#include "ScriptLibs/SettingsLib.hpp"
#include "ScriptLibs/ClientLib.hpp"
#include "ScriptLibs/PlayerLib.hpp"
#include "ScriptLibs/ImGuiLib.hpp"
#include "ScriptLibs/GlobalsLib.hpp"
#include "ScriptLibs/UtilLib.hpp"
#include "ScriptLibs/StructsLib.hpp"
#include "ScriptLibs/FlarialGUILib.hpp"
#include "ScriptLibs/ConstraintsLib.hpp"
#include "ScriptLibs/PacketsLib.hpp"
#include "ScriptLibs/AudioLib.hpp"
#include "ScriptLibs/NetworkLib.hpp"

static int customPrint(lua_State* L) {
    int args = lua_gettop(L);
    std::string msg;

    for (int i = 1; i <= args; i++) {
        if (lua_isnil(L, i)) {
            msg += "nil";
        }
        else if (lua_isboolean(L, i)) {
            msg += lua_toboolean(L, i) ? "true" : "false";
        }
        else if (lua_isnumber(L, i)) {
            msg += std::to_string(lua_tonumber(L, i));
        }
        else if (lua_isstring(L, i)) {
            msg += lua_tostring(L, i);
        }
        else {
            msg += "[Unsupported Type]";
        }

        if (i < args) {
            msg += " ";
        }
    }

    if (SDK::clientInstance && SDK::clientInstance->getGuiData()) {
        SDK::clientInstance->getGuiData()->displayClientMessage(msg);
    }

    return 0;
}

FlarialScript::FlarialScript(std::string filePath, std::string code)
    : mFilePath(std::move(filePath)), mCode(std::move(code)) {

    mState = luaL_newstate();
    luaL_openlibs(mState);

    lua_pushcfunction(mState, customPrint);
    lua_setglobal(mState, "print");

    ScriptLib::registerLib<FSLib>(mState);
    ScriptLib::registerLib<SettingsLib>(mState);
    ScriptLib::registerLib<ClientLib>(mState);
    ScriptLib::registerLib<PlayerLib>(mState);
    ScriptLib::registerLib<ImGuiLib>(mState);
    ScriptLib::registerLib<GlobalsLib>(mState);
    ScriptLib::registerLib<UtilLib>(mState);
    ScriptLib::registerLib<StructsLib>(mState);
    ScriptLib::registerLib<FlarialGUILib>(mState);
    ScriptLib::registerLib<ConstraintsLib>(mState);
    ScriptLib::registerLib<PacketsLib>(mState);
    ScriptLib::registerLib<AudioLib>(mState);
    ScriptLib::registerLib<NetworkLib>(mState);
}

bool FlarialScript::compile() {
    try {
        // Load and validate the code
        if (luaL_loadstring(mState, mCode.c_str()) != LUA_OK) {
            Logger::error("Syntax error in {}: {}", mFilePath, lua_tostring(mState, -1));
            ADD_ERROR_MESSAGE("Syntax error: " + std::string(lua_tostring(mState, -1)));
            lua_pop(mState, 1);
            return false;
        }

        registerModuleFunction("onEvent", [](lua_State* L) {
                const char* eventName = luaL_checkstring(L, 1);

                if (!lua_isfunction(L, 2)) {
                    luaL_error(L, "Expected a function as the second argument");
                }

                lua_getglobal(L, "eventHandlers");
                if (lua_isnil(L, -1)) {
                    lua_pop(L, 1);
                    lua_newtable(L);
                    lua_setglobal(L, "eventHandlers");
                    lua_getglobal(L, "eventHandlers");
                }

                lua_pushstring(L, eventName);
                lua_pushvalue(L, 2);
                lua_settable(L, -3);

                lua_pop(L, 1);

                return LUA_OK;
            });

        // Execute the script
        if (lua_pcall(mState, 0, 0, 0) != LUA_OK) {
            Logger::error("Runtime error in {}: {}", mFilePath, lua_tostring(mState, -1));
            ADD_ERROR_MESSAGE("Runtime error: " + std::string(lua_tostring(mState, -1)));
            lua_pop(mState, 1);
            return false;
        }

        lua_getglobal(mState, "name");
        if (!lua_isstring(mState, -1)) {
            Logger::error("Script {} is missing 'name'", mFilePath);
            lua_pop(mState, 1);
            return false;
        }
        mName = lua_tostring(mState, -1);
        lua_pop(mState, 1);

        lua_getglobal(mState, "description");
        if (!lua_isstring(mState, -1)) {
            Logger::error("Script {} is missing 'description'", mFilePath);
            lua_pop(mState, 1);
            return false;
        }
        mDescription = lua_tostring(mState, -1);
        lua_pop(mState, 1);

        lua_getglobal(mState, "author");
        if (lua_isstring(mState, -1)) {
            mAuthor = lua_tostring(mState, -1);
        }
        lua_pop(mState, 1);

        Logger::info("Successfully compiled script '{}'", mName);
        return true;

    } catch (const std::exception& e) {
        Logger::error("Failed to compile script '{}': {}", mFilePath, e.what());
        return false;
    }
}

void FlarialScript::registerEvent(const std::string& eventName) {
    std::lock_guard lock(eventMutex);
    if (mIsDestroyed) return;

    lua_getglobal(mState, "eventHandlers");
    if (!lua_istable(mState, -1)) {
        lua_pop(mState, 1);
        return;
    }

    lua_pushstring(mState, eventName.c_str());
    lua_gettable(mState, -2);

    if (!lua_isfunction(mState, -1)) {
        lua_pop(mState, 2);
        return;
    }

    if (lua_pcall(mState, 0, 0, 0) != LUA_OK) {
        Logger::error("Error executing event {}: {}", eventName, lua_tostring(mState, -1));
        ADD_ERROR_MESSAGE("Error executing event " + eventName + ": " + std::string(lua_tostring(mState, -1)));
        lua_pop(mState, 1);
    }

    lua_pop(mState, 1);
}