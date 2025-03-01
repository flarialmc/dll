#pragma once

#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingTypes.hpp>
#include <Scripting/FlarialScript.hpp>

#include <Modules/ClickGUI/ClickGUI.hpp>
#include <Client/Module/Modules/Module.hpp>

#include <lua.hpp>

inline ScriptSettingManager gScriptSettingManager;

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



    void terminate() override {
        Deafen(this, KeyEvent, &ScriptModuleBase::onKey);
        Deafen(this, MouseEvent, &ScriptModuleBase::onMouse);
        Deafen(this, PacketEvent, &ScriptModuleBase::onPacketReceive);
        Deafen(this, TickEvent, &ScriptModuleBase::onTick);
        Deafen(this, RenderEvent, &ScriptModuleBase::onRender);
        Deafen(this, SetupAndRenderEvent, &ScriptModuleBase::onSetupAndRender);
        Module::terminate();
    }

    [[nodiscard]] bool isEnabled() const { return linkedScript && linkedScript->isEnabled(); }
    void onEnable() override;

    void onDisable() override;

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

    void onKey(KeyEvent& event);
    void onMouse(MouseEvent& event);
    void onPacketReceive(PacketEvent &event);
    void onTick(TickEvent& event);
    void onRender(RenderEvent& event);
    void onSetupAndRender(SetupAndRenderEvent& event);
};
