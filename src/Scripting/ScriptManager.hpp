#pragma once

#include <lua.hpp>

class Script;
class Module;
class ModuleScript;
class CommandScript;
class ScriptManager {
public:
    static void initialize();
    static void shutdown();
    static void loadModuleScripts();
    static void loadCommandScripts();
    static void executeFunction(lua_State* L, const char* functionName);
    static void reloadScripts();
    static void _reloadScripts();
    static void saveSettings();

    static std::vector<std::shared_ptr<Script>> getLoadedScripts() {
        return mLoadedScripts;
    }

    static std::vector<std::shared_ptr<ModuleScript>> getLoadedModules() {
        return mLoadedModules;
    }

    static Script* getScriptByState(lua_State* L);
    static Module* getModuleByState(lua_State* L);
    static std::shared_ptr<Module> getModuleByName(const std::vector<std::shared_ptr<ModuleScript>>& modules, const std::string& moduleName);
    static inline bool initialized = false;

private:
    static inline std::vector<std::shared_ptr<Script>> mLoadedScripts;
    static inline std::vector<std::shared_ptr<ModuleScript>> mLoadedModules;
    static inline std::vector<std::shared_ptr<CommandScript>> mLoadedCommands;
    static inline std::atomic<bool> pendingReload = false;
};