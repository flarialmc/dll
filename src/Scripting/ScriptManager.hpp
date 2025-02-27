#pragma once

#include "FlarialScript.hpp"

class ScriptModuleBase;

class ScriptManager {
public:
    static void initialize();
    static void shutdown();
    static void loadScripts();
    static void executeFunction(lua_State* L, const char* functionName);
    static void saveSettings();
    static void reloadScripts();

    static std::vector<std::shared_ptr<FlarialScript>> getLoadedScripts() {
        std::lock_guard lock(mMutex);
        return mLoadedScripts;
    }

    static std::vector<std::shared_ptr<ScriptModuleBase>> getLoadedModules() {
        std::lock_guard lock(mMutex);
        return mLoadedModules;
    }

    static inline bool initialized = false;

private:
    static std::vector<std::shared_ptr<FlarialScript>> mLoadedScripts;
    static std::vector<std::shared_ptr<ScriptModuleBase>> mLoadedModules;
    static std::mutex mMutex;
};