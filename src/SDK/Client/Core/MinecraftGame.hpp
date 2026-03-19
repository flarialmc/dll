#pragma once

#include "../Render/TextureGroup.hpp"
#include "../Render/GameRenderer.hpp"

class MinecraftGame {
public:
    BUILD_ACCESS(this, bool, mouseGrabbed, GET_OFFSET("MinecraftGame::mouseGrabbed"));
    BUILD_ACCESS(this, std::shared_ptr<TextureGroup>, textureGroup, GET_OFFSET("MinecraftGame::textureGroup"));

    GameRenderer* getGameRenderer() {
        return hat::member_at<GameRenderer*>(this, GET_OFFSET("MinecraftGame::gameRenderer")); // shhh
    }
    void _onResumeWaitReloadActors() {
        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeWaitReloadActors");
        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
        fn(this);
    }
//    void _onResumeWaitPreviousResourceLoads() {
//        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeWaitPreviousResourceLoads");
//        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
//        fn(this);
//    }
//    void _onResumeQueueReloadTextures() {
//        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeQueueReloadTextures");
//        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
//        fn(this);
//    }
//    void _onResumeReloadTextures() {
//        static auto sig = GET_SIG_ADDRESS("MinecraftGame::_onResumeReloadTextures");
//        auto fn = reinterpret_cast<void (__thiscall *)(MinecraftGame *)>(sig);
//        fn(this);
//    }
};
