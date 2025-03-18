#include "Script.hpp"

#include <Utils/Logger/Logger.hpp>

#include "ScriptSettings/ScriptSettingManager.hpp"
#include "ScriptLibs/ScriptLib.hpp"

#include <Command/CommandManager.hpp>
#include "ModuleCommandScript.hpp"

#include "ScriptLibs/FSLib.hpp"
#include "ScriptLibs/SettingsLib.hpp"
#include "ScriptLibs/ClientLib.hpp"
#include "ScriptLibs/PlayerLib.hpp"
#include "ScriptLibs/ImGuiLib.hpp"
#include "ScriptLibs/GlobalsLib.hpp"
#include "ScriptLibs/UtilLib.hpp"
#include "ScriptLibs/ObjectsLib.hpp"
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

static int registerCommand(lua_State* L) {
    Module* mod = ScriptManager::getModuleByState(L);
    if (!mod) {
        lua_pushstring(L, "registerCommand() called, but could not find associated module!");
        lua_error(L);
        return 0;
    }

    auto moduleScript = dynamic_cast<ModuleScript*>(mod);
    if (!moduleScript) {
        lua_pushstring(L, "registerCommand() called on non-ModuleScript!");
        lua_error(L);
        return 0;
    }

    // Parse args
    const char* cmdName = luaL_checkstring(L, 1);
    if (!lua_isfunction(L, 2)) {
        lua_pushstring(L, "registerCommand() requires a function as the second arg");
        lua_error(L);
        return 0;
    }

    // Store the function
    lua_getglobal(L, "commandHandlers");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, "commandHandlers");
        lua_getglobal(L, "commandHandlers");
    }

    lua_pushstring(L, cmdName); // push key
    lua_pushvalue(L, 2); // push copy of user function
    lua_settable(L, -3); // commandHandlers[cmdName] = function

    lua_pop(L, 1);

    auto commandPtr = std::make_shared<ModuleCommand>(
        cmdName,
        std::string("Lua module command"),
        std::vector<std::string>{}, // you can parse alias info as well
        std::weak_ptr<ModuleScript>(moduleScript->shared_from_this())
    );

    CommandManager::Commands.push_back(commandPtr);

    return 0;
}

Script::Script(std::string filePath, std::string code)
    : mFilePath(std::move(filePath)), mCode(std::move(code)) {

    mState = luaL_newstate();
    luaL_openlibs(mState);

    lua_pushcfunction(mState, customPrint);
    lua_setglobal(mState, "print");

    lua_pushcfunction(mState, registerCommand);
    lua_setglobal(mState, "registerCommand");

    ScriptLib::registerLib<FSLib>(mState);
    ScriptLib::registerLib<SettingsLib>(mState);
    ScriptLib::registerLib<ClientLib>(mState);
    ScriptLib::registerLib<PlayerLib>(mState);
    ScriptLib::registerLib<ImGuiLib>(mState);
    ScriptLib::registerLib<GlobalsLib>(mState);
    ScriptLib::registerLib<UtilLib>(mState);
    ScriptLib::registerLib<ObjectsLib>(mState);
    ScriptLib::registerLib<FlarialGUILib>(mState);
    ScriptLib::registerLib<ConstraintsLib>(mState);
    ScriptLib::registerLib<PacketsLib>(mState);
    ScriptLib::registerLib<AudioLib>(mState);
    ScriptLib::registerLib<NetworkLib>(mState);
}

bool Script::compile() {
    try {
        // Load and validate the code
        if (luaL_loadstring(mState, mCode.c_str()) != LUA_OK) {
            Logger::error("Error in {}: {}", mFilePath, lua_tostring(mState, -1));
            ADD_ERROR_MESSAGE("Error: " + std::string(lua_tostring(mState, -1)));
            lua_pop(mState, 1);
            return false;
        }

        registerFunction("onEvent", [](lua_State* L) {
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
        if (safeCall(0, 0) != LUA_OK) {
            return false;
        }

        lua_getglobal(mState, "name");
        if (!lua_isstring(mState, -1)) {
            Logger::error("Script '{}' is missing 'name'", mFilePath);
            lua_pop(mState, 1);
            return false;
        }
        mName = lua_tostring(mState, -1);
        lua_pop(mState, 1);

        lua_getglobal(mState, "description");
        if (!lua_isstring(mState, -1)) {
            Logger::error("Script '{}' is missing 'description'", mFilePath);
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

        lua_getglobal(mState, "debug");
        if (lua_isboolean(mState, -1)) {
            mDebug = lua_toboolean(mState, -1);
        }
        lua_pop(mState, 1);

        return true;
    } catch (const std::exception& e) {
        Logger::error("Failed to compile script '{}': {}", mFilePath, e.what());
        return false;
    }
}

int Script::traceback(lua_State* L) {
    if (!lua_isstring(L, 1)) return 1;

    luaL_traceback(L, L, lua_tostring(L, 1), 1);
    return 1;
}

int Script::safeCall(int nargs, int nresults) {
    if (!mState || mIsDestroyed) return LUA_ERRRUN;

    const bool debugEnabled = isDebugEnabled();
    // Index where we pushed the function we want to call is (top - nargs).
    int base = lua_gettop(mState) - nargs;

    if (debugEnabled) {
        // Push the traceback function above the function/arguments.
        lua_pushcfunction(mState, traceback);
        // Move it just below the function we’re about to call.
        lua_insert(mState, base);
    }

    int status = lua_pcall(mState, nargs, nresults, debugEnabled ? base : 0);

    if (status != LUA_OK) {
        std::string error = lua_tostring(mState, -1);

        for (char& c : error) {
            if (c == '\t') { // Weird formatting in game.
                c = ' ';
            }
        }

        Logger::error("[{}] Error: {}", mName, error);
        ADD_ERROR_MESSAGE("Error: " + error);
        lua_pop(mState, 1);
        if (debugEnabled) {
            lua_remove(mState, base);
        }
    } else {
        if (debugEnabled) {
            lua_remove(mState, base);
        }
    }

    return status;
}

void Script::registerEvent(const std::string& eventName) {
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

    safeCall(0, 0);
    lua_pop(mState, 1);
}