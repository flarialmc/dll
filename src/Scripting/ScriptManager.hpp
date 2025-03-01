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
    static void saveSettings();
    static void reloadScripts();
    static bool toggleScript(const std::string& scriptName);

    static std::vector<std::shared_ptr<FlarialScript>> getLoadedScripts() {
        return mLoadedScripts;
    }

    static std::vector<std::shared_ptr<ScriptModuleBase>> getLoadedModules() {
        return mLoadedModules;
    }

    static inline bool initialized = false;
    static std::shared_ptr<Module> FindModuleByName(const std::vector<std::shared_ptr<ScriptModuleBase>>& modules, const std::string& moduleName);

private:
    static std::vector<std::shared_ptr<FlarialScript>> mLoadedScripts;
    static std::vector<std::shared_ptr<ScriptModuleBase>> mLoadedModules;
};