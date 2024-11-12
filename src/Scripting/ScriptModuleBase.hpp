#pragma once


#include "../Client/Module/Modules/Module.hpp"
#include "lua.h"
#include "Scripting.hpp"
#include "../Client/Events/EventManager.hpp"
#include "EventManager/ScriptingEventManager.hpp"

class ScriptModuleBase : public Module {

    lua_State* module_lua_state;
public:
    ScriptModuleBase(std::string name, std::string description, lua_State* lua_state) : Module(name, description, IDR_TIME_PNG, "") {
        this->module_lua_state = lua_state;
        Module::setup();
        Listen(this, KeyEvent, &ScriptModuleBase::onKey)
    };

    void onEnable() override {
        Scripting::executeFunction(module_lua_state, "onEnable");
        Module::onEnable();

    }

    void onDisable() override {
        Scripting::executeFunction(module_lua_state, "onDisable");
        Module::onDisable();
    }

    void onKey(KeyEvent &event) {
        if(!this->isEnabled()) return;
        std::vector<std::any> args = {
                event.getKey(),
                event.getAction()
        };

        ScriptingEventManager::triggerEvent(module_lua_state, "onKeyEvent", args);
    };
};

