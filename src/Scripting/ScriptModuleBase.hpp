#pragma once

#include <Scripting/ScriptManager.hpp>
#include <Scripting/ScriptEvents/ScriptEventManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingTypes.hpp>

#include <Modules/ClickGUI/ClickGUI.hpp>
#include <Client/Module/Modules/Module.hpp>

#include <lua.hpp>


inline ScriptSettingManager gScriptSettingManager;

class ScriptModuleBase : public Module {
public:
    lua_State* moduleLuaState;

    ScriptModuleBase(const std::string& name, const std::string& description, lua_State* luaState)
    : Module(name, description, IDR_SCRIPT_PNG, "", true), moduleLuaState(luaState) {

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
        ScriptManager::executeFunction(moduleLuaState, "onEnable");
        Module::onEnable();
    }

    void onDisable() override {
        ScriptManager::executeFunction(moduleLuaState, "onDisable");
    }

    void defaultConfig() override {
    }

    void settingsRender(float settingsOffset) override {
        const float x = Constraints::PercentageConstraint(0.019, "left");
        const float y = Constraints::PercentageConstraint(0.10, "top");
        const float scrollViewWidth = Constraints::RelativeConstraint(0.5, "height", true);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollViewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        auto& settings = gScriptSettingManager.getAllSettings();
        for (auto& [uniqueKey, settingPtr] : settings) {
            if (uniqueKey.rfind(this->name + ".", 0) == 0) {
                switch (settingPtr->type) {
                    case ScriptSettingType::Bool: {
                        auto* boolSet = dynamic_cast<BoolSetting*>(settingPtr.get());
                        this->addToggle(boolSet->name, boolSet->description, boolSet->defaultValue);
                        break;
                    }
                    case ScriptSettingType::Float: {
                        break;
                    }
                    default: {
                    }
                }
            }
        }

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void onKey(KeyEvent& event) {
        if (!ScriptManager::initialized || !enabledState) return;

        bool cancelled = ScriptEventManager::triggerEvent("onKey", event.getKey(), (int)event.getAction());
        if (cancelled) event.cancel();
    }

    void onMouse(MouseEvent& event) {
        if (!ScriptManager::initialized || !enabledState || event.getButton() == 0 || event.getAction() == 0) return;

        bool cancelled = ScriptEventManager::triggerEvent("onMouse", (int)event.getButton(), (int)event.getAction());
        if (cancelled) event.cancel();
    }

    void onPacketReceive(PacketEvent &event) {
        if (!ScriptManager::initialized || !enabledState) return;

        bool cancelled = ScriptEventManager::triggerEvent("onPacketReceive", event.getPacket(), (int)event.getPacket()->getId());
        if (cancelled) event.cancel();
    }

    void onTickEvent(TickEvent& event) {
        if (!ScriptManager::initialized || !enabledState) return;

        ScriptEventManager::triggerEvent("onTick");
    }

    void onRenderEvent(RenderEvent& event) {
        if (!ScriptManager::initialized || !enabledState) return;

        ScriptEventManager::triggerEvent("onRender");
    }
};
