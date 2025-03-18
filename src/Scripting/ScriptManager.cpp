#include "ScriptManager.hpp"

#include "ModuleScript.hpp"
#include "CommandScript.hpp"
#include <Utils/Utils.hpp>
#include <Client/Module/Manager.hpp>
#include <Client/Module/Modules/Module.hpp>
#include <Client/Command/CommandManager.hpp>

std::vector<std::shared_ptr<Script>> ScriptManager::mLoadedScripts;
std::vector<std::shared_ptr<ModuleScript>> ScriptManager::mLoadedModules;
std::vector<std::shared_ptr<CommandScript>> ScriptManager::mLoadedCommands;

void ScriptManager::initialize() {
    const std::filesystem::path p(Utils::getClientPath() + "\\Scripts");
    const std::vector scriptPaths = {
    p / "Modules",
    p / "Commands",
    p / "Data"
    };

    for (const auto& scriptPath : scriptPaths) {
        if (!exists(scriptPath)) {
            create_directory(scriptPath);
        }
    }

    loadModuleScripts();
    loadCommandScripts();
    initialized = true;
}

void ScriptManager::shutdown() {
    for (const auto& mod : mLoadedModules) {
        mod->terminate();
    }

    mLoadedModules.clear();
    mLoadedCommands.clear();
    mLoadedScripts.clear();
    initialized = false;
}

void ScriptManager::loadModuleScripts() {
    std::filesystem::path path = Utils::getClientPath() + "\\Scripts\\Modules\\";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() != ".lua") continue;

        std::ifstream scriptFile(entry.path());
        std::string code((std::istreambuf_iterator(scriptFile)), std::istreambuf_iterator<char>());

        auto script = std::make_shared<Script>(entry.path().filename().string(), code);
        mLoadedScripts.push_back(script);

        if (!script->compile()) {
            mLoadedScripts.pop_back();
            continue;
        }

        auto mod = std::make_shared<ModuleScript>(
            script->getName(),
            script->getDescription(),
            script->getState(),
            script);

        mLoadedModules.emplace_back(mod);
        mod->defaultConfig();
        ModuleManager::cguiRefresh = true;
        Logger::info("Loaded module script '{}'", script->getName());

        executeFunction(script->getState(), "onLoad");
    }
}

void ScriptManager::loadCommandScripts() {
    std::filesystem::path path = Utils::getClientPath() + "\\Scripts\\Commands\\";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() != ".lua") continue;

        std::ifstream scriptFile(entry.path());
        std::string code((std::istreambuf_iterator(scriptFile)), std::istreambuf_iterator<char>());

        auto script = std::make_shared<Script>(
            entry.path().filename().string(),
            code
        );

        if (!script->compile()) return;

        if (script->getName().find(' ') != std::string::npos) {
            Logger::error("Command script '{}' has a space", script->getName());
            continue;
        }

        std::vector<std::string> aliases; {
            lua_State* L = script->getState();
            lua_getglobal(L, "aliases");

            if (lua_istable(L, -1)) {
                lua_pushnil(L);
                while (lua_next(L, -2) != 0) {
                    if (lua_isstring(L, -1)) {
                        aliases.emplace_back(lua_tostring(L, -1));
                    }
                    lua_pop(L, 1);
                }
            }
            lua_pop(L, 1);
        }

        bool hasSpace = false;
        for (const auto& alias : aliases) {
            if (alias.find(' ') != std::string::npos) {
                Logger::error("Command script '{}' has an alias containing a space: '{}'", script->getName(), alias);
                hasSpace = true;
                break;
            }
        }

        if (hasSpace) {
            continue;
        }

        auto command = std::make_shared<CommandScript>(
            script->getName(),
            script->getDescription(),
            aliases,
            script);

        mLoadedScripts.push_back(script);
        mLoadedCommands.emplace_back(command);
        CommandManager::Commands.push_back(command);
        Logger::info("Loaded command script '{}'", script->getName());

        executeFunction(script->getState(), "onLoad");
    }
}

void ScriptManager::executeFunction(lua_State *L, const char* functionName) {
    Script* script = getScriptByState(L);
    if (!script) return;

    lua_getglobal(L, functionName);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    script->safeCall(0, 0);
}

void ScriptManager::reloadScripts() {
    pendingReload = true;
}

void ScriptManager::_reloadScripts() {
    if (!pendingReload) return;
    initialized = false;

    CommandManager::terminate();

    for (auto& mod : mLoadedModules) {
        if (mod) {
            mod->terminate();
        }
    }

    mLoadedModules.clear();
    mLoadedCommands.clear();
    mLoadedScripts.clear();

    loadModuleScripts();
    loadCommandScripts();

    CommandManager::initialize();

    initialized = true;
    pendingReload = false;
}

void ScriptManager::saveSettings() {
    for (const auto& module : mLoadedModules) {
        if (!module) return;
        module->saveSettings();
    }
}

Script* ScriptManager::getScriptByState(lua_State* L) {
    for (const auto& script : mLoadedScripts) {
        if (script->getState() == L) {
            return script.get();
        }
    }
    return nullptr;
}

Module* ScriptManager::getModuleByState(lua_State* L) {
    for (const auto& script : mLoadedScripts) {
        if (script->getState() == L) {
            for (const auto& module : mLoadedModules) {
                if (auto sp = module->linkedScript.lock()) {
                    if (sp.get() == script.get()) {
                        return module.get();
                    }
                }
            }
        }
    }
    return nullptr;
}

std::shared_ptr<Module> ScriptManager::getModuleByName(const std::vector<std::shared_ptr<ModuleScript>>& modules,const std::string& moduleName) {
    for (const auto& mod : modules) {
        if (mod && mod->name == moduleName) {
            return std::static_pointer_cast<Module>(mod);
        }
    }
    return nullptr;
}