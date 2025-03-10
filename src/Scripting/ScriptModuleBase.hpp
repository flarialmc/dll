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
    lua_State* moduleLuaState = nullptr;
    std::weak_ptr<FlarialScript> linkedScript;

    ScriptModuleBase(const std::string& name, const std::string& description, lua_State* luaState, std::shared_ptr<FlarialScript> script)
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
        if (const auto script = linkedScript.lock()) {
            gScriptSettingManager.saveSettings(script.get());
            gScriptSettingManager.clearSettingsForScript(script.get());
        }
        Deafen(this, KeyEvent, &ScriptModuleBase::onKey);
        Deafen(this, MouseEvent, &ScriptModuleBase::onMouse);
        Deafen(this, PacketEvent, &ScriptModuleBase::onPacketReceive);
        Deafen(this, TickEvent, &ScriptModuleBase::onTick);
        Deafen(this, RenderEvent, &ScriptModuleBase::onRender);
        Deafen(this, SetupAndRenderEvent, &ScriptModuleBase::onSetupAndRender);
        Module::terminate();
    }

    void onEnable() override;
    void onDisable() override;

    void defaultConfig() override {
        if (const auto script = linkedScript.lock()) {
            gScriptSettingManager.loadSettings(script.get());
        }
    }

    void settingsRender(float settingsOffset) override {
        const float x = Constraints::PercentageConstraint(0.019, "left");
        const float y = Constraints::PercentageConstraint(0.10, "top");
        const float scrollViewWidth = Constraints::RelativeConstraint(0.5, "height", true);

        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollViewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        if (const auto script = linkedScript.lock()) {
            const auto& settings = gScriptSettingManager.getAllSettings();

            if (const auto it = settings.find(script.get()); it != settings.end()) {
                for (const auto &settingPtr: it->second | std::views::values) {

                    switch (settingPtr->type) {
                        case ScriptSettingType::Bool: {
                            if (auto* boolSet = dynamic_cast<BoolSetting*>(settingPtr.get()); boolSet) {
                                if (auto* setting = gScriptSettingManager.getSetting<BoolSetting>(script.get(), boolSet->name)) {
                                    this->addToggle(boolSet->displayName, boolSet->description, setting->value);
                                } else {
                                    this->addToggle(boolSet->displayName, boolSet->description, boolSet->defaultValue);
                                }
                            }
                            break;
                        }
                        case ScriptSettingType::Float: {
                            break;
                        }
                        default:
                            break;
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
