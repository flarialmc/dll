#include <iostream>
#include <lua.hpp>
#include "../Client/GUI/Engine/Engine.hpp"
#include "Scripting.hpp"
#include "../Config/json/json.hpp"
#include "../Client/Module/Manager.hpp"
#include "Functions/Player.hpp"
#include "ScriptModuleBase.hpp"
#include "Functions/GUI.hpp"
#include "Functions/General.hpp"
#include "Functions/Constraints.hpp"
#include "Functions/Settings.hpp"
#include "Functions/Player.hpp"
#include "EventManager/ScriptEvents.hpp"
#include "Packets/LuaTextPacket.hpp"
#include "Functions/OtherSettings.hpp"

int lua_register_event_handler(lua_State* L) {
    const int eventName = luaL_checkinteger(L, 1);

    if (!lua_isfunction(L, 2)) {
        lua_pushstring(L, "Expected function as second argument");
        std::string err = std::string( "Expected function as second argument: ") + lua_tostring(L, -1);
        Logger::error(err);
        lua_error(L);
        return 0;
    }

    ScriptingEventManager::registerHandler(L, eventName);
    return 0;
}

void Scripting::executeFunction(lua_State* L, std::string functionName, bool shitInLogsIfFunctionIsNotFound) {
    lua_getglobal(L, functionName.c_str());
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        if(!shitInLogsIfFunctionIsNotFound) return;
        std::string err = "Error calling " + functionName + ": " + lua_tostring(L, -1);
        Logger::error(err);
        lua_pop(L, 1);
    }
}


void registerFunctions(lua_State* L){
    lua_register(L, "onEvent", lua_register_event_handler);

    General::registerGeneral(L);
    GUI::registerGUI(L);
    LuaConstraints::registerConstraints(L);
    LuaSettings::registerSetting(L);
    LuaOtherSettings::registerSetting(L);
    player::registerPlayer(L);
    ScriptEvents::pushEventTypesToLua(L);


    LuaTextPacket::registerFunctions(L);
}





void load(std::string name, std::string description, std::string mainclass) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_getglobal(L, "utf8");

    registerFunctions(L);

    const std::string path = mainclass;
    Scripting::scriptsAmount++;

    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        std::string err = std::string("Error loading script: ") + lua_tostring(L, -1);
        Logger::error(err);
        lua_close(L);
        Scripting::scriptsAmountWithErrors++;
        return;
    }

    ModuleManager::addModule<ScriptModuleBase>(name, description, L);
    auto mod = ModuleManager::getModule(name).get();
    Scripting::luaScriptModules.emplace_back(L, mod);
    mod->defaultConfig();
    Scripting::scriptsAmountWithoutErrors++;
}

void Scripting::loadModules() {
     for (const auto& entry : std::filesystem::directory_iterator(Utils::getRoamingPath() + "\\Flarial\\scripts")) { 
            
            if (std::filesystem::exists(entry.path().string() + "\\" + "main.json")){
                std::ifstream file(entry.path().string() + "\\" + "main.json");
                nlohmann::json jsonData;
                file >> jsonData;

                std::string moduleName = jsonData["name"];
                std::string description = jsonData["description"];
                std::string mainClass = jsonData["main_class"];

                load(moduleName, description, entry.path().string() + "\\" + mainClass);
                
        }else{
            FlarialGUI::Notify("could not find main.json");
        }
    }

    Logger::custom(fg(fmt::color::aqua), "Scripting", "Found {} scripts! Loaded {} scripts without errors. Found errors in {} scripts.", Scripting::scriptsAmount, Scripting::scriptsAmountWithoutErrors, Scripting::scriptsAmountWithErrors);
}
