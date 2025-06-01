#pragma once

#include "ScriptLib.hpp"
#include <Scripting/ScriptUtils.hpp>
#include <Scripting/ScriptManager.hpp>
#include <Scripting/ScriptSettings/ScriptSettingManager.hpp>

class SettingsLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginClass<ToggleSetting>("BoolSetting")
                .addProperty("value", &ToggleSetting::value)
            .endClass()
            .beginClass<SliderSetting>("SliderSetting")
                .addProperty("value", &SliderSetting::value)
            .endClass()
            .beginClass<TextBoxSetting>("TextBoxSetting")
                .addProperty("value", &TextBoxSetting::value)
            .endClass()
            .beginClass<KeybindSetting>("KeybindSetting")
                .addProperty("value",
                    [](const KeybindSetting* self) {
                        int key = Utils::getStringAsKey(self->value);
                        return ScriptUtils::isKeyDown(key);
                    },
                    [](KeybindSetting* self, const std::string& key) {
                        self->value = key;
                    })
            .endClass();

        getGlobalNamespace(state)
            .beginNamespace("settings")
                .addFunction("addHeader", [state](const std::string& text) {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script) return;

                    gScriptSettingManager.addHeader(script, text);
                })
                .addFunction("extraPadding", [state]() {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script) return;

                    gScriptSettingManager.extraPadding(script);
                })
                .addFunction("addToggle", [state](const std::string& name, const std::string& desc, bool defValue) -> ToggleSetting* {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script) return nullptr;

                    auto* setting = gScriptSettingManager.addSetting<ToggleSetting>(script, name, desc, defValue);
                    return setting;
                })
                .addFunction("addButton", [state](const std::string& name, const std::string& desc, const std::string& buttonText, const LuaRef& func) -> ButtonSetting* {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script || !func.isFunction()) return nullptr;

                    // Push the function to the stack and create a registry reference
                    func.push(state);
                    int ref = luaL_ref(state, LUA_REGISTRYINDEX);

                    // Define the action to call the referenced function
                    std::function action = [state, ref]() {
                        lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
                        if (lua_isfunction(state, -1)) {
                            if (lua_pcall(state, 0, 0, 0) != LUA_OK) {
                                LOG_ERROR("Error in button callback: {}", lua_tostring(state, -1));
                                lua_pop(state, 1);
                            }
                        } else {
                            LOG_ERROR("Button callback function not found.");
                            lua_pop(state, 1);
                        }
                    };

                    auto* setting = gScriptSettingManager.addButton(script, name, desc, buttonText, action);
                    return setting;
                })
                .addFunction("addSlider", [state](const std::string& name, const std::string& desc, float defValue, float maxVal, float minVal, bool zerosafe) -> SliderSetting* {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script) return nullptr;

                    auto* setting = gScriptSettingManager.addSetting<SliderSetting>(script, name, desc, defValue, maxVal, minVal, zerosafe);
                    return setting;
                })
                .addFunction("addTextBox", [state](const std::string& name, const std::string& desc, const std::string& defaultValue, int limit) -> TextBoxSetting* {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script) return nullptr;

                    auto* setting = gScriptSettingManager.addTextBox(script, name, desc, defaultValue, limit);
                    return setting;
                })
                .addFunction("addKeybind", [state](const std::string& name, const std::string& desc) -> KeybindSetting* {
                    auto* script = ScriptManager::getScriptByState(state);
                    if (!script) return nullptr;

                    int numArgs = lua_gettop(state);
                    std::string defaultKey = (numArgs >= 3) ? lua_tostring(state, 3) : "";

                    auto* setting = gScriptSettingManager.addKeybind(script, name, desc, defaultKey);
                    return setting;
                })
            .endNamespace();
    }
};