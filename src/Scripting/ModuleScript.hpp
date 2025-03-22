#pragma once

#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingTypes.hpp>
#include <Scripting/Script.hpp>

#include <Modules/ClickGUI/ClickGUI.hpp>
#include <Client/Module/Modules/Module.hpp>

#include <lua.hpp>

inline ScriptSettingManager gScriptSettingManager;

class ModuleScript : public Module, public std::enable_shared_from_this<ModuleScript> {
public:
    lua_State* moduleLuaState = nullptr;
    std::weak_ptr<Script> linkedScript;

    ModuleScript(const std::string& name, const std::string& description, lua_State* luaState, std::shared_ptr<Script> script)
    : Module(name, description, IDR_SCRIPT_PNG, "", true), moduleLuaState(luaState), linkedScript(script) {

        Module::setup();
        Listen(this, KeyEvent, &ModuleScript::onKey);
        Listen(this, MouseEvent, &ModuleScript::onMouse);
        Listen(this, PacketEvent, &ModuleScript::onPacketReceive);
        Listen(this, PacketSendEvent, &ModuleScript::onPacketSend);
        Listen(this, TickEvent, &ModuleScript::onTick);
        Listen(this, RenderEvent, &ModuleScript::onRender);
        Listen(this, SetupAndRenderEvent, &ModuleScript::onSetupAndRender);
        Listen(this, AttackEvent, &ModuleScript::onAttack);
        Listen(this, FOVEvent, &ModuleScript::onFOV);
        Listen(this, PerspectiveEvent, &ModuleScript::onPerspective);
    }

    void terminate() override {
        if (const auto script = linkedScript.lock()) {
            gScriptSettingManager.saveSettings(script.get());
            gScriptSettingManager.clearSettingsForScript(script.get());
        }

        Deafen(this, KeyEvent, &ModuleScript::onKey);
        Deafen(this, MouseEvent, &ModuleScript::onMouse);
        Deafen(this, PacketEvent, &ModuleScript::onPacketReceive);
        Deafen(this, PacketSendEvent, &ModuleScript::onPacketSend);
        Deafen(this, TickEvent, &ModuleScript::onTick);
        Deafen(this, RenderEvent, &ModuleScript::onRender);
        Deafen(this, SetupAndRenderEvent, &ModuleScript::onSetupAndRender);
        Deafen(this, AttackEvent, &ModuleScript::onAttack);
        Deafen(this, FOVEvent, &ModuleScript::onFOV);
        Deafen(this, PerspectiveEvent, &ModuleScript::onPerspective);
        Module::terminate();
    }

    void onEnable() override;
    void onDisable() override;

    void saveSettings() {
        if (const auto script = linkedScript.lock()) gScriptSettingManager.saveSettings(script.get());
        try {
            std::ofstream outputFile(settingspath);
            if (!outputFile.is_open()) {
                Logger::error("Failed to open file: {}", settingspath.string());
                return;
            }
            outputFile << settings.ToJson();
        } catch (const std::exception& e) {
            Logger::error("An error occurred while saving settings: {}", e.what());
        }
    }

    void defaultConfig() override { Module::defaultConfig();
        if (const auto script = linkedScript.lock()) {
            gScriptSettingManager.loadSettings(script.get());
        }

        if (settings.getSettingByName<std::string>("text") == nullptr) {
            settings.addSetting("text", static_cast<std::string>("{VALUE}"));
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
                        case ScriptSettingType::Toggle: {
                            auto* toggleSet = dynamic_cast<ToggleSetting*>(settingPtr.get());
                            if (!toggleSet) return;

                            this->addToggle(toggleSet->name, toggleSet->description, toggleSet->value);
                            break;
                        }
                        case ScriptSettingType::Button: {
                            auto* buttonSet = dynamic_cast<ButtonSetting*>(settingPtr.get());
                            if (!buttonSet) return;

                            this->addButton(buttonSet->name, buttonSet->description, buttonSet->buttonText, buttonSet->action);
                            break;
                        }
                        case ScriptSettingType::Slider: {
                            auto* sliderSet = dynamic_cast<SliderSetting*>(settingPtr.get());
                            if (!sliderSet) return;

                            this->addSlider(sliderSet->name, sliderSet->description, sliderSet->value, sliderSet->maxValue, sliderSet->minValue, sliderSet->zerosafe);
                            break;
                        }
                        case ScriptSettingType::TextBox: {
                            auto* textBoxSet = dynamic_cast<TextBoxSetting*>(settingPtr.get());
                            if (!textBoxSet) return;

                            this->addTextBox(textBoxSet->name, textBoxSet->description, textBoxSet->value, textBoxSet->characterLimit);
                            break;
                        }
                        case ScriptSettingType::Keybind: {
                            auto* keybindSet = dynamic_cast<KeybindSetting*>(settingPtr.get());
                            if (!keybindSet) return;

                            this->addKeybind(keybindSet->name, keybindSet->description, keybindSet->value);
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                }
            }
        }

        FlarialGUI::UnsetScrollView();
        this->resetPadding();
    }

    void onKey(KeyEvent& event);
    void onMouse(MouseEvent& event);
    void onPacketReceive(PacketEvent& event);
    void onPacketSend(PacketSendEvent& event);
    void onTick(TickEvent& event);
    void onRender(RenderEvent& event);
    void onSetupAndRender(SetupAndRenderEvent& event);
    void onAttack(AttackEvent& event);
    void onFOV(FOVEvent& event);
    void onPerspective(PerspectiveEvent& event);
};
