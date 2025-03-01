#include "ScriptManager.hpp"

#include <Utils/Utils.hpp>
#include <Client/Module/Manager.hpp>

#include "ScriptModuleBase.hpp"

std::vector<std::shared_ptr<FlarialScript>> ScriptManager::mLoadedScripts;
std::vector<std::shared_ptr<ScriptModuleBase>> ScriptManager::mLoadedModules;

void ScriptManager::initialize() {
    const std::filesystem::path p(Utils::getClientPath() + "\\Scripts");
    const std::vector scriptPaths = {
    p / "Modules",
    p / "Commands"
    };

    for (const auto& scriptPath : scriptPaths) {
        if (!exists(scriptPath)) {
            create_directory(scriptPath);
        }
    }

    loadScripts();
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

void ScriptManager::loadScripts() {
    mLoadedScripts.clear();

    std::filesystem::path modulePath = Utils::getClientPath() + "\\Scripts\\Modules\\";

    for (const auto& entry : std::filesystem::directory_iterator(modulePath)) {
        if (entry.path().extension() != ".lua") continue;

        std::ifstream scriptFile(entry.path());
        std::string code((std::istreambuf_iterator<char>(scriptFile)), std::istreambuf_iterator<char>());

        auto script = std::make_shared<FlarialScript>(
            entry.path().string(),
            code
        );

        if (script->compile()) {
            mLoadedScripts.push_back(script);
            Logger::info("Successfully loaded script '{}'", script->getName());

            auto mod = ModuleManager::makeModule<ScriptModuleBase>(
                script->getName(),
                script->getDescription(),
                script->getState(),
                script.get());

            mLoadedModules.emplace_back(mod);
        }
        ModuleManager::cguiRefresh = true;
    }
}

void ScriptManager::executeFunction(lua_State *L, const char* functionName) {
    lua_getglobal(L, functionName);

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        std::string err = lua_tostring(L, -1);
        Logger::error("Error calling Lua function '{}': {}", functionName, err);
        lua_pop(L, 1);
    }
}

void ScriptManager::saveSettings() {

}

void ScriptManager::reloadScripts() {
    for (const auto& mod : mLoadedModules) {
        mod->terminate();
    }

    mLoadedModules.clear();
    mLoadedScripts.clear();
    loadScripts();
}

bool ScriptManager::toggleScript(const std::string& scriptName) {
    std::string normalizedInput = scriptName;
    normalizedInput = String::replaceAll(normalizedInput, " ", "");
    normalizedInput = String::toLower(normalizedInput);

    for (const auto& script : mLoadedScripts) {
        std::string scriptStoredName = script->getName();
        scriptStoredName = String::replaceAll(scriptStoredName, " ", "");
        scriptStoredName = String::toLower(scriptStoredName);

        if (scriptStoredName == normalizedInput) {
            script->setEnabled(!script->isEnabled());
            return true;
        }
    }
    return false;
}

std::shared_ptr<Module> ScriptManager::FindModuleByName(
    const std::vector<std::shared_ptr<ScriptModuleBase>>& modules,
    const std::string& moduleName)
{
    for (const auto& mod : modules) {
        if (mod && mod->name == moduleName) {
            return std::static_pointer_cast<Module>(mod);
        }
    }
    return nullptr;
}