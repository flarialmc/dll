#include <iostream>
#include <lua.hpp>
#include "../Client/GUI/Engine/Engine.hpp"
#include "Scripting.hpp"
#include "../Config/json/json.hpp"
#include "../Client/Module/Manager.hpp"
#include "ScriptModuleBase.hpp"

int lua_Notify(lua_State* L) {
    const char* message = luaL_checkstring(L, 1);
    FlarialGUI::Notify(message);
    return 0;
}

int lua_register_event_handler(lua_State* L) {
    const char* eventName = luaL_checkstring(L, 1);

    if (!lua_isfunction(L, 2)) {
        lua_pushstring(L, "Expected function as second argument");
        std::string err = std::string( "Expected function as second argument: ") + lua_tostring(L, -1);
        Logger::error(err);
        lua_error(L);
        return 1;
    }

    ScriptingEventManager::registerHandler(L, eventName);
    return 0;
}

void Scripting::executeFunction(lua_State* L, std::string functionName) {
    lua_getglobal(L, functionName.c_str());
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        std::string err = "Error calling " + functionName + ": " + lua_tostring(L, -1);
        Logger::error(err);
        lua_pop(L, 1);
    }
}
void registerFunctions(lua_State* L){
    lua_register(L, "Notify", lua_Notify);
    lua_register(L, "onEvent", lua_register_event_handler);
}

void load(std::string name, std::string description, std::string mainclass) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    registerFunctions(L);

    const std::string path = Utils::getRoamingPath() + "\\Flarial\\scripts\\" + mainclass;

    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        std::string err = std::string("Error loading script: ") + lua_tostring(L, -1);
        Logger::error(err);
        lua_close(L);
        return;
    }

    ModuleManager::addModule<ScriptModuleBase>(name, description, L);
}

void Scripting::loadModules() {
    for (const auto& entry : std::filesystem::directory_iterator(Utils::getRoamingPath() + "\\Flarial\\scripts")) {
        if (entry.path().extension() == ".json") {
            std::ifstream file(entry.path());
            nlohmann::json jsonData;
            file >> jsonData;

            std::string moduleName = jsonData["name"];
            std::string description = jsonData["description"];
            std::string mainClass = jsonData["main_class"];

            load(moduleName, description, mainClass);
        }
    }
}