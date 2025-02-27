#include "FlarialScript.hpp"

#include <Utils/Logger/Logger.hpp>

#include "ScriptSettings/ScriptSettingManager.hpp"
#include "ScriptEvents/ScriptEventManager.hpp"
#include "ScriptLibs/ScriptLib.hpp"

#include "ScriptLibs/FSLib.hpp"
#include "ScriptLibs/SettingsLib.hpp"
#include "ScriptLibs/ClientLib.hpp"
#include "ScriptLibs/PlayerLib.hpp"

static int customPrint(lua_State* L) {
    int args = lua_gettop(L);

    std::string msg;
    for (int i = 1; i <= args; i++) {
        const char* arg = luaL_checkstring(L, i);
        msg += arg;
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

    ScriptEventManager::registerEventAPI(mState);
    ScriptLib::registerLib<FSLib>(mState);
    ScriptLib::registerLib<SettingsLib>(mState);
    ScriptLib::registerLib<ClientLib>(mState);
    ScriptLib::registerLib<PlayerLib>(mState);

    // Setting system is so scuffed rn
    ScriptSettingManager::createBoolSetting(mState);
}

bool FlarialScript::compile() {
    try {
        // Load and validate the code
        if (luaL_loadstring(mState, mCode.c_str()) != LUA_OK) {
            Logger::error("Syntax error in {}: {}", mFilePath, lua_tostring(mState, -1));
            lua_pop(mState, 1);
            return false;
        }

        // Execute the script
        if (lua_pcall(mState, 0, 0, 0) != LUA_OK) {
            Logger::error("Runtime error in {}: {}", mFilePath, lua_tostring(mState, -1));
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

        // Easily get script name :))
        lua_pushstring(mState, mName.c_str());
        lua_setglobal(mState, "__scriptName");

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