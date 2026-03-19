#pragma once

/**
 * @file ClientLib.hpp
 * @brief Lua bindings for Flarial client functions
 *
 * Provides scripts with access to client features like notifications,
 * screen info, and basic controls.
 *
 * ## Available Functions
 *
 * ### Notifications
 * - `client.notify(message)` - Display a toast notification
 * - `client.displayLocalMessage(message)` - Display a chat message (client-side only)
 *
 * ### Screen & UI
 * - `client.getScreenName()` - Get current screen name (e.g., "hud_screen")
 * - `client.freeMouse()` - Release mouse cursor
 * - `client.grabMouse()` - Lock mouse to window
 *
 * ### Client Info
 * - `client.fps()` - Get current FPS
 * - `client.version()` - Get Flarial client version
 * - `client.mcVersion()` - Get Minecraft version
 * - `client.isInGame()` - Check if player is in a world
 * - `client.isSingleplayer()` - Check if in singleplayer
 *
 * ### Modules
 * - `client.isModuleEnabled(name)` - Check if a module is enabled
 * - `client.getModules()` - Get list of all module names
 *
 * ### Misc
 * - `client.exit()` / `client.crash()` - Exit the client
 *
 * @example
 * ```lua
 * -- FPS monitor script
 * function onTick()
 *     local fps = client.fps()
 *     if fps < 30 then
 *         client.notify("Low FPS warning: " .. fps)
 *     end
 * end
 *
 * -- Screen-specific logic
 * function onRender()
 *     if client.getScreenName() == "hud_screen" then
 *         -- Only render on HUD
 *     end
 * end
 * ```
 */

#include "ScriptLib.hpp"

#include <Client/GUI/Engine/Engine.hpp>
#include <Client/Command/CommandManager.hpp>
#include <Client/Module/Manager.hpp>
#include <Client/Client.hpp>
#include <Utils/Telemetry.hpp>

class ClientLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("client")
                // === Notifications ===
                .addFunction("notify", [](const std::string& message) {
                    if (message.empty()) return;
                    FlarialGUI::Notify(message);
                })
                .addFunction("displayLocalMessage", [](const std::string& message) {
                    if (message.empty()) return;
                    if (!SDK::clientInstance || !SDK::clientInstance->getGuiData()) return;
                    SDK::clientInstance->getGuiData()->displayClientMessage(message);
                })

                // === Screen & UI ===
                .addFunction("getScreenName", []() -> std::string {
                    if (!SDK::clientInstance) return "";
                    return SDK::clientInstance->getScreenName();
                })
                .addFunction("freeMouse", []() {
                    if (!SDK::clientInstance) return;
                    SDK::clientInstance->releaseMouse();
                })
                .addFunction("grabMouse", []() {
                    if (!SDK::clientInstance) return;
                    SDK::clientInstance->grabMouse();
                })

                // === Client Info ===
                .addFunction("fps", []() -> int {
                    return static_cast<int>(MC::fps);
                })
                .addFunction("version", []() -> std::string {
                    return std::string(FLARIAL_VERSION);
                })
                .addFunction("mcVersion", []() -> std::string {
                    return Telemetry::getClientVersion();
                })
                .addFunction("isInGame", []() -> bool {
                    if (!SDK::clientInstance) return false;
                    return SDK::clientInstance->getLocalPlayer() != nullptr;
                })
                .addFunction("isSingleplayer", []() -> bool {
                    std::string ip = SDK::getServerIP();
                    return ip == "world";
                })

                // === Module access (read-only) ===
                .addFunction("isModuleEnabled", [](const std::string& name) -> bool {
                    auto module = ModuleManager::getModule(name);
                    if (!module) return false;
                    return module->isEnabled();
                })
                .addFunction("getModules", [](lua_State* L) -> int {
                    lua_newtable(L);
                    int index = 1;
                    for (const auto& mod : ModuleManager::modulesVector) {
                        lua_newtable(L);

                        lua_pushstring(L, mod->name.c_str());
                        lua_setfield(L, -2, "name");

                        lua_pushstring(L, mod->description.c_str());
                        lua_setfield(L, -2, "description");

                        lua_pushboolean(L, mod->isEnabled());
                        lua_setfield(L, -2, "enabled");

                        lua_rawseti(L, -2, index++);
                    }
                    return 1;
                })

                // === Misc ===
                .addFunction("exit", []() {
                    std::exit(0);
                })
                .addFunction("crash", []() {
                    std::exit(0);
                })
            .endNamespace();
    }
};