#pragma once

#include <Scripting/ScriptManager.hpp>
#include <Scripting/ScriptEvents/ScriptEventManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingTypes.hpp>

#include <Modules/ClickGUI/ClickGUI.hpp>
#include <Client/Module/Modules/Module.hpp>

#include <lua.hpp>


inline ScriptSettingManager gScriptSettingManager;

// onEnable, onDisable and enabled checks are broken due to unfinished setting system & ClickGUI
// I can't fix it until ClickGUI can properly render scripts, so this bool allows you
// to still use events and reload scripts.
// There's no way to check if the script is enabled or not so events don't fire.
const bool ALWAYS_ENABLE = false;

class ScriptModuleBase : public Module {
public:
    lua_State* moduleLuaState;
    FlarialScript* linkedScript;

    ScriptModuleBase(const std::string& name, const std::string& description, lua_State* luaState, FlarialScript* script)
    : Module(name, description, IDR_SCRIPT_PNG, "", true), moduleLuaState(luaState), linkedScript(script) {

        Module::setup();
        Listen(this, KeyEvent, &ScriptModuleBase::onKey);
        Listen(this, MouseEvent, &ScriptModuleBase::onMouse);
        Listen(this, PacketEvent, &ScriptModuleBase::onPacketReceive);
        Listen(this, TickEvent, &ScriptModuleBase::onTick);
        Listen(this, RenderEvent, &ScriptModuleBase::onRender);
        Listen(this, SetupAndRenderEvent, &ScriptModuleBase::onSetupAndRender);
    }

    bool isEnabled() const { return linkedScript && linkedScript->isEnabled(); }

    void terminate() override {
        Deafen(this, KeyEvent, &ScriptModuleBase::onKey);
        Deafen(this, MouseEvent, &ScriptModuleBase::onMouse);
        Deafen(this, PacketEvent, &ScriptModuleBase::onPacketReceive);
        Deafen(this, TickEvent, &ScriptModuleBase::onTick);
        Deafen(this, RenderEvent, &ScriptModuleBase::onRender);
        Deafen(this, SetupAndRenderEvent, &ScriptModuleBase::onSetupAndRender);
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
        //if (!ScriptManager::initialized || (!ALWAYS_ENABLE && !enabledState)) return;
        if (!ScriptManager::initialized || !isEnabled()) return;

        bool cancelled = ScriptEventManager::triggerEvent("onKey", event.getKey(), (int)event.getAction());
        if (cancelled) event.cancel();
    }

    void onMouse(MouseEvent& event) {
        //if (!ScriptManager::initialized || (!ALWAYS_ENABLE && !enabledState) || event.getButton() == 0 || event.getAction() == 0) return;
        if (!ScriptManager::initialized || !isEnabled() || event.getButton() == 0 || event.getAction() == 0) return;

        bool cancelled = ScriptEventManager::triggerEvent("onMouse", (int)event.getButton(), (int)event.getAction());
        if (cancelled) event.cancel();
    }

    void onPacketReceive(PacketEvent &event) {
        //if (!ScriptManager::initialized || (!ALWAYS_ENABLE && !enabledState)) return;
        if (!ScriptManager::initialized || !isEnabled()) return;

        bool cancelled = ScriptEventManager::triggerEvent("onPacketReceive", event.getPacket(), (int)event.getPacket()->getId());
        if (cancelled) event.cancel();
    }

    void onTick(TickEvent& event) {
        //if (!ScriptManager::initialized || (!ALWAYS_ENABLE && !enabledState)) return;
        if (!ScriptManager::initialized || !isEnabled()) return;

        ScriptEventManager::triggerEvent("onTick");
    }

    void onRender(RenderEvent& event) {
        //if (!ScriptManager::initialized || (!ALWAYS_ENABLE && !enabledState)) return;
        if (!ScriptManager::initialized || !isEnabled()) return;

        ScriptEventManager::triggerEvent("onRender");
    }

    void onSetupAndRender(SetupAndRenderEvent& event) {
        //if (!ScriptManager::initialized || (!ALWAYS_ENABLE && !enabledState)) return;
        if (!ScriptManager::initialized || !isEnabled()) return;

        ScriptEventManager::triggerEvent("onSetupAndRender");
    }
};
