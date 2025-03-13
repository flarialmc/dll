#pragma once

#include <Modules/ClickGUI/ClickGUI.hpp>
#include "../Client/Module/Modules/Module.hpp"
#include "lua.h"
#include "Scripting.hpp"
#include "../Client/Events/EventManager.hpp"
#include "EventManager/ScriptingEventManager.hpp"
#include "EventManager/ScriptEvents.hpp"

class ScriptModuleBase : public Module {
public:
    std::shared_ptr<lua_State> module_lua_state;

    ScriptModuleBase(const std::string& name, const std::string& description, lua_State* lua_state)
            : Module(name, description, IDR_SCRIPT_PNG, "", true), module_lua_state(lua_state) {
        Module::setup();

        Listen(this, KeyEvent, &ScriptModuleBase::onKey);
        Listen(this, MouseEvent, &ScriptModuleBase::onMouse);
        Listen(this, PacketEvent, &ScriptModuleBase::onPacketReceive);
        Listen(this, TickEvent, &ScriptModuleBase::onTickEvent);
        Listen(this, RenderEvent, &ScriptModuleBase::onRenderEvent);
    }

    void terminate() override {
        Deafen(this, KeyEvent, &ScriptModuleBase::onKey);
        Deafen(this, MouseEvent, &ScriptModuleBase::onMouse);
        Deafen(this, PacketEvent, &ScriptModuleBase::onPacketReceive);
        Deafen(this, TickEvent, &ScriptModuleBase::onTickEvent);
        Module::terminate();
    }

    void onEnable() override {
        Scripting::executeFunction(module_lua_state.get(), "onEnable", true);
        Module::onEnable();
    }

    void onDisable() override {
        Scripting::executeFunction(module_lua_state.get(), "onDisable", true);
        Module::onDisable();
    }

    void defaultConfig() override {
        if(settings.getSettingByName<std::string>("text") == nullptr)
            settings.addSetting("text", (std::string) "{VALUE}");

        Scripting::executeFunction(module_lua_state.get(), "defaultConfig", false);
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");
        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));
        Scripting::executeFunction(module_lua_state.get(), "settingsRender", false);
        FlarialGUI::UnsetScrollView();
        this->resetPadding();
        //i am hopeful
    }

    void onKey(KeyEvent& event) {
        if (!Scripting::instalized || !this->enabledState) return;

        bool canceled = ScriptingEventManager::triggerEvent(module_lua_state.get(), ScriptEvents::EventType::onKeyEvent,
                                            event.getKey(), (int)event.getAction());

        if (canceled) {
            event.cancel();
        }
    }

    void onMouse(MouseEvent& event) {
        if (!Scripting::instalized || !this->enabledState || event.getButton() == 0 || event.getAction() == 0) return;

        bool canceled = ScriptingEventManager::triggerEvent(module_lua_state.get(), ScriptEvents::EventType::onMouseEvent,
                                            (int)event.getButton(), (int)event.getAction());
        if (canceled) {
            event.cancel();
        }
    }

    void onPacketReceive(PacketEvent &event) {
        if (!Scripting::instalized || !this->enabledState) return;

        bool canceled = ScriptingEventManager::triggerEvent(module_lua_state.get(), ScriptEvents::EventType::onPacketReceiveEvent,
                                            event.getPacket(), (int)event.getPacket()->getId());
        if (canceled) {
            event.cancel();
        }
    }

    void onTickEvent(TickEvent& event) {
        if (!Scripting::instalized || !this->enabledState) return;

        ScriptingEventManager::triggerEvent(module_lua_state.get(), ScriptEvents::EventType::onTickEvent);
    }

    void onRenderEvent(RenderEvent& event) {
        if (!Scripting::instalized || !this->enabledState) return;

        ScriptingEventManager::triggerEvent(module_lua_state.get(), ScriptEvents::EventType::onRenderEvent);
    }
};
