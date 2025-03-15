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
    mLoadedScripts.clear();
    initialized = false;
}

void ScriptManager::loadModuleScripts() {
    std::filesystem::path path = Utils::getClientPath() + "\\Scripts\\Modules\\";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() != ".lua") continue;

        std::ifstream scriptFile(entry.path());
        std::string code((std::istreambuf_iterator(scriptFile)), std::istreambuf_iterator<char>());

        auto script = std::make_shared<Script>(
            entry.path().filename().string(),
            code
        );

        // Be careful if you try to access fields like getName()
        // before the script compiles. (Won't crash but you get the point)
        mLoadedScripts.push_back(script);

        if (script->compile()) {
            Logger::info("Successfully loaded module script '{}'", script->getName());

            auto mod = std::make_shared<ModuleScript>(
                script->getName(),
                script->getDescription(),
                script->getState(),
                script);

            mLoadedModules.emplace_back(mod);
            mod->defaultConfig();
            ModuleManager::cguiRefresh = true;
        } else {
            mLoadedScripts.pop_back();
        }
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

        mLoadedScripts.push_back(script);

        if (script->compile()) {
            if (script->getName().find(' ') != std::string::npos) {
                Logger::error("Command script '{}' has a space", script->getName());
                return;
            }

            Logger::info("Successfully loaded command script '{}'", script->getName());

            auto command = std::make_shared<CommandScript>(
                script->getName(),
                script->getDescription(),
                std::vector<std::string>{},
                script);

            mLoadedCommands.emplace_back(command);
            CommandManager::Commands.push_back(command);
        } else {
            mLoadedScripts.pop_back();
        }
    }
}


void ScriptManager::executeFunction(lua_State *L, const char* functionName) {
    if (!L || lua_status(L) != LUA_OK) return;
    lua_getglobal(L, functionName);

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        std::string err = lua_tostring(L, -1);
        Logger::error("Error calling Lua function '{}': {}", functionName, err);
        ADD_ERROR_MESSAGE("Error calling Lua function '" + std::string(functionName) + "': " + err);
        lua_pop(L, 1);
    }
}

void ScriptManager::reloadScripts() {
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