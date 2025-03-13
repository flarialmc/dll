#pragma once

#include "FlarialScript.hpp"
#include "ScriptModuleBase.hpp"
#include <Module/Modules/Module.hpp>

class ScriptManager {
public:
    static void initialize();
    static void shutdown();
    static void loadScripts();
    static void executeFunction(lua_State* L, const char* functionName);
    static void reloadScripts();
    static void saveSettings();

    static std::vector<std::shared_ptr<FlarialScript>> getLoadedScripts() {
        return mLoadedScripts;
    }

    static std::vector<std::shared_ptr<ScriptModuleBase>> getLoadedModules() {
        return mLoadedModules;
    }

    static FlarialScript* getScriptByState(lua_State* L);
    static Module* getModuleByState(lua_State* L);
    static std::shared_ptr<Module> getModuleByName(const std::vector<std::shared_ptr<ScriptModuleBase>>& modules, const std::string& moduleName);
    static inline bool initialized = false;

private:
    static std::vector<std::shared_ptr<FlarialScript>> mLoadedScripts;
    static std::vector<std::shared_ptr<ScriptModuleBase>> mLoadedModules;
};