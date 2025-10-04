#pragma once

#include <cstdint>
#include <string>

// Forward declarations
class LocalPlayer;
class MinecraftGame;
class BlockSource;
class GuiData;
class LoopbackPacketSender;
class RaknetConnector;
class LevelRender;

struct GLMatrix;

namespace mce {
    class Camera;
}

template<typename T>
struct Vec2;

class ClientInstance {
public:
    MinecraftGame* getMinecraftGame();

    GuiData *getGuiData();

    GLMatrix getViewMatrix();

    mce::Camera& getCamera();

    LocalPlayer *getLocalPlayer();

    BlockSource *getBlockSource();

    void grabMouse(int delay = 0);

    void releaseMouse();

    static std::string getTopScreenName();

    std::string getScreenName();

    LevelRender *getLevelRender();

    float getFovX();

    float getFovY();

    Vec2<float> getFov();

    LoopbackPacketSender *getPacketSender();

    RaknetConnector *getRakNetConnector();

    void _updateScreenSizeVariables(Vec2<float> *totalScreenSize, Vec2<float> *safeZone, float forcedGuiScale);
};
