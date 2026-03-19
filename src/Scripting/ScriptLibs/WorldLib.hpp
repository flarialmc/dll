#pragma once

/**
 * @file WorldLib.hpp
 * @brief Lua bindings for read-only world/environment information
 *
 * Provides scripts with access to world state like blocks and dimensions.
 * All functions are read-only to prevent cheating.
 *
 * ## Available Functions
 *
 * ### Block Functions
 * - `world.getBlock(x, y, z)` - Get block name at position
 * - `world.getLightLevel(x, y, z)` - Get light level at position (0-15)
 *
 * ### World Info
 * - `world.getDimension()` - Get current dimension name
 * - `world.getWorldName()` - Get world/level name
 *
 * @example
 * ```lua
 * -- Check block under player
 * function onTick()
 *     local x, y, z = player.position()
 *     local block = world.getBlock(x, y - 1, z)
 *     if block == "minecraft:lava" then
 *         client.notify("Warning: Standing on lava!")
 *     end
 * end
 *
 * -- Display world info
 * function onEnable()
 *     client.notify("World: " .. world.getWorldName())
 *     client.notify("Dimension: " .. world.getDimension())
 * end
 * ```
 */

#include "ScriptLib.hpp"
#include <SDK/SDK.hpp>
#include <SDK/Client/Level/Level.hpp>
#include <SDK/Client/Block/Block.hpp>
#include <SDK/Client/Block/BlockSource.hpp>

class WorldLib : public ScriptLib {
public:
    void initialize(lua_State* state) override {
        using namespace luabridge;

        getGlobalNamespace(state)
            .beginNamespace("world")
                // Block functions
                .addFunction("getBlock", [](double x, double y, double z) -> std::string {
                    if (!SDK::clientInstance) return "minecraft:air";
                    auto* blockSource = SDK::clientInstance->getBlockSource();
                    if (!blockSource) return "minecraft:air";

                    BlockPos pos(static_cast<int>(std::floor(x)),
                                static_cast<int>(std::floor(y)),
                                static_cast<int>(std::floor(z)));

                    auto* block = blockSource->getBlock(pos);
                    if (!block) return "minecraft:air";

                    return block->getName();
                })
                .addFunction("getLightLevel", [](double x, double y, double z) -> int {
                    if (!SDK::clientInstance) return 0;
                    auto* blockSource = SDK::clientInstance->getBlockSource();
                    if (!blockSource) return 0;

                    BlockPos pos(static_cast<int>(std::floor(x)),
                                static_cast<int>(std::floor(y)),
                                static_cast<int>(std::floor(z)));

                    return blockSource->getBrightness(pos);
                })

                // Dimension info
                .addFunction("getDimension", []() -> std::string {
                    if (!SDK::clientInstance) return "unknown";
                    auto* blockSource = SDK::clientInstance->getBlockSource();
                    if (!blockSource || !blockSource->getDimension()) return "unknown";
                    return blockSource->getDimension()->getName();
                })

                // World name
                .addFunction("getWorldName", []() -> std::string {
                    if (!SDK::clientInstance) return "";
                    auto* level = SDK::clientInstance->getLevel();
                    if (!level) return "";
                    auto* levelData = level->getLevelData();
                    if (!levelData) return "";
                    return levelData->getLevelName();
                })

            .endNamespace();
    }
};
