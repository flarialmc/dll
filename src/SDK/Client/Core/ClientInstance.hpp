#pragma once

#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include "../Block/BlockSource.hpp"
#include "../Render/GuiData.hpp"
#include <cstdint>
#include <cmath>
#include "../../../Utils/Memory/Memory.hpp"
#include "../Network/Packet/LoopbackPacketSender.hpp"
#include "Minecraft.hpp"
#include "../Render/GLMatrix.hpp"
#include "../Level/LevelRender/LevelRender.hpp"
#include "../Network/Raknet/RaknetConnector.hpp"
#include "../Render/Camera.hpp"

/// Primary interface to the Minecraft client; provides access to player, world, rendering, and network state.
class ClientInstance {
public:
    MinecraftGame* getMinecraftGame() {
        // if (!SDK::clientInstance) return nullptr;
        return hat::member_at<MinecraftGame*>(this, GET_OFFSET("ClientInstance::minecraftGame"));
    };

    GuiData *getGuiData() {
        return hat::member_at<GuiData*>(this, GET_OFFSET("ClientInstance::guiData"));
    };

    GLMatrix getViewMatrix() {
        return hat::member_at<GLMatrix>(this, GET_OFFSET("ClientInstance::viewMatrix"));
    };

    mce::Camera& getCamera() {
        static int off = GET_OFFSET("ClientInstance::camera");
        return hat::member_at<mce::Camera>(this, off);
    }

    /// Returns the local player entity, or nullptr if not in a world.
    LocalPlayer *getLocalPlayer();

    /// Returns the block source for the current dimension; used for block queries.
    BlockSource *getBlockSource();

    /// Captures the mouse cursor (hides it and locks to window).
    void grabMouse(int delay = 0);

    /// Releases the mouse cursor back to normal system control.
    void releaseMouse();

    static std::string getTopScreenName();

    std::string getScreenName();

    /// Returns the level renderer; uses vfunc on 1.21.20+, direct member access on older versions.
    LevelRender *getLevelRender();

    float getFovX() {
        float fov = hat::member_at<float>(this, GET_OFFSET("ClientInstance::getFovX"));
        // Sanity check - FOV should be a reasonable positive value (typically 0.5 to 3.0 radians)
        // Values below 0.1 are definitely invalid (would be less than 6 degrees)
        // If invalid, return a default value based on ~70 degree FOV
        if (fov < 0.1f || fov > 10.0f || std::isnan(fov) || std::isinf(fov)) {
            return 1.22f; // Default ~70 degree FOV in radians
        }
        return fov;
    };

    float getFovY() {
        float fov = hat::member_at<float>(this, GET_OFFSET("ClientInstance::getFovY"));
        // Sanity check - FOV should be a reasonable positive value (typically 0.5 to 3.0 radians)
        // Values below 0.1 are definitely invalid
        if (fov < 0.1f || fov > 10.0f || std::isnan(fov) || std::isinf(fov)) {
            return 1.22f; // Default ~70 degree FOV in radians
        }
        return fov;
    };

    Vec2<float> getFov() {
        return Vec2<float>{getFovX(), getFovY()};
    };

    LoopbackPacketSender *getPacketSender() {
        return hat::member_at<LoopbackPacketSender *>(this, GET_OFFSET("ClientInstance::getPacketSender"));
    }

    RaknetConnector *getRakNetConnector() {
        if (getPacketSender() == nullptr)
            return nullptr;

        return getPacketSender()->networkSystem->remoteConnectorComposite->rakNetConnector;
    }

    /// Forces a recalculation of screen size and GUI scale (used for custom GUI scale overrides).
    void _updateScreenSizeVariables(Vec2<float> *totalScreenSize, Vec2<float> *safeZone, float forcedGuiScale);
};
