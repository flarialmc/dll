#pragma once
#include "../Hook.hpp"
#include "../../../../SDK/Client/Actor/Actor.hpp"

class AnimationComponent_playAnimation : public Hook {

private:
    static void callback(void* ExpressionNode, HashedString* a2, HashedString* a3, HashedString* a4, HashedString* a5);

public:
    typedef void(__thiscall *original)(void* ExpressionNode, HashedString* a2, HashedString* a3, HashedString* a4, HashedString* a5);
    static inline original funcOriginal = nullptr;

    AnimationComponent_playAnimation();

    void enableHook() override;
};