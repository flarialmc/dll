#include "ScriptManager.hpp"

#include <Utils/Utils.hpp>
#include <Client/Module/Manager.hpp>

#include "ScriptModuleBase.hpp"
#include "ScriptEvents/ScriptEventManager.hpp"

std::vector<std::shared_ptr<FlarialScript>> ScriptManager::mLoadedScripts;
std::vector<std::shared_ptr<ScriptModuleBase>> ScriptManager::mLoadedModules;
std::mutex ScriptManager::mMutex;

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
    std::lock_guard lock(mMutex);

    for (const auto& mod : mLoadedModules) {
        mod->terminate();
    }

    mLoadedModules.clear();
    mLoadedScripts.clear();
    ScriptEventManager::clearAll();
    initialized = false;
}

void ScriptManager::loadScripts() {
    std::lock_guard<std::mutex> lock(mMutex);
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
                script->getState());

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
    std::lock_guard lock(mMutex);

    for (const auto& mod : mLoadedModules) {
        mod->terminate();
    }

    Logger::info("1");
    mLoadedModules.clear();
    Logger::info("2");
    mLoadedScripts.clear();
    Logger::info("3");
    ScriptEventManager::clearAll();
    Logger::info("4");
    loadScripts();
    Logger::info("5");
}