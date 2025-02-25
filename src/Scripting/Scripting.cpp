#include "Scripting.hpp"

#include <Client/GUI/Engine/Engine.hpp>

#include <json/json.hpp>
#include <Client/Module/Manager.hpp>
#include <Scripting/Libs/Flarial/Mixins/LuaMixin.hpp>
#include <Scripting/Libs/ocornut/imgui/ImGui.hpp>
#include <Scripting/Libs/Flarial/Utils/DrawUtil.hpp>
#include <Scripting/Libs/Flarial/Utils/LuaAudio.hpp>
#include "Functions/Player.hpp"
#include "ScriptModuleBase.hpp"
#include "Functions/GUI.hpp"
#include "Functions/General.hpp"
#include "Functions/Constraints.hpp"
#include "Functions/Settings.hpp"
#include "Functions/Player.hpp"
#include "EventManager/ScriptEvents.hpp"
#include "Packets/LuaTextPacket.hpp"
#include "Packets/LuaPlaySoundPacket.hpp"
#include "Packets/LuaSetTitlePacket.hpp"
#include "Functions/OtherSettings.hpp"
#include "Functions/LuaModuleManager.hpp"
#include "Functions/LuaClient.hpp"
#include "Functions/LuaModule.hpp"
#include "Functions/Inventory.hpp"
#include "Console/Console.hpp"
#include "Commands/LuaOnCommand.hpp"
#include "Packets/Packets.hpp"


int apiRevision = 2;

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



void Scripting::unloadModules() {
    ScriptingEventManager::clearHandlers();
    for (auto &scriptModule: Scripting::luaScriptModules) {
        std::shared_ptr<lua_State> L = scriptModule.first;
        Module *module = scriptModule.second.get();
        if (module) {
            std::string name = module->name;
            module->terminate();
            Logger::info("Module '{}' has been unloaded.", name);
        }
        if (L) {
            lua_close(L.get());
        }
    }

    Scripting::luaScriptModules.clear();
    Scripting::scriptsAmount = 0;
    Scripting::scriptsAmountWithoutErrors = 0;
    Scripting::scriptsAmountWithErrors = 0;

    Logger::info("All scripts have been unloaded.");
    Scripting::console.addLog("Scripting", "All scripts have been unloaded.", fg(fmt::color::yellow));
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
    LuaModuleManager::registerModuleManager(L);
    LuaClient::registerClient(L);
    LuaModule::registerModule(L);
    LuaOnCommand::registerLuaOnCommand(L);
    LuaPackets::pushPacketTypesToLua(L);
    LuaInventory::registerFunctions(L);
    LuaItem::registerFunctions(L);

    // Packets
    LuaTextPacket::registerFunctions(L);
    LuaPlaySoundPacket::registerFunctions(L);
    LuaSetTitlePacket::registerFunctions(L);

    //libs
    LuaMixins::registerLib(L);
    LuaImGui::registerLib(L);
    LuaDrawUtil::registerLib(L);
    LuaAudio::registerLib(L);
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

    auto mod = ModuleManager::makeModule<ScriptModuleBase>(name, description, L);
    Scripting::luaScriptModules.emplace_back(L, mod);
    mod->defaultConfig();
    Scripting::scriptsAmountWithoutErrors++;
}

void Scripting::loadModules() {
     for (const auto& entry : std::filesystem::directory_iterator((Utils::getRoamingPath() + "\\Flarial\\scripts").c_str())) {
            
            if (std::filesystem::exists(entry.path().string() + "\\" + "main.json")){
                std::ifstream file(entry.path().string() + "\\" + "main.json");
                nlohmann::json::parser_callback_t parserCallback;
                nlohmann::json jsonData = nlohmann::json::parse(file, nullptr, true, true);

                std::string moduleName = jsonData["name"];
                std::string description = jsonData["description"];
                std::string mainClass = jsonData["main_class"];
                int apiRev = jsonData["api_revision"];
                if(apiRevision != apiRev) {
                    FlarialGUI::Notify(moduleName + " is outdated");
                    Logger::custom(fg(fmt::color::red), "Scripting loader", "{} is outdated", moduleName);
                    Scripting::console.addLog("Scripting loader", moduleName + " is outdated", fg(fmt::color::red));
                    Scripting::scriptsAmountWithErrors++;
                    continue;
                }

                load(moduleName, description, entry.path().string() + "\\" + mainClass);
                
        }else{
            FlarialGUI::Notify("could not find main.json");
            Scripting::console.addLog("Scripting loader", "could not find main.json", fg(fmt::color::red));
        }
    }

    ModuleManager::cguiRefresh = true;
    Logger::custom(fg(fmt::color::aqua), "Scripting", "Found {} scripts! Loaded {} scripts without errors. Found errors in {} scripts.", Scripting::scriptsAmount, Scripting::scriptsAmountWithoutErrors, Scripting::scriptsAmountWithErrors);
    Scripting::console.addLog("Scripting", "Found " + std::to_string(Scripting::scriptsAmount) + " scripts! Loaded "+ std::to_string(Scripting::scriptsAmountWithoutErrors) +" scripts without errors. Found errors in "+ std::to_string(Scripting::scriptsAmountWithErrors) +" scripts.", fg(fmt::color::aqua));
}


void Scripting::SaveSettings() {
    for (auto &scriptModule: luaScriptModules) {
        Module *module = scriptModule.second.get();
        module->saveSettings();
    }
}
