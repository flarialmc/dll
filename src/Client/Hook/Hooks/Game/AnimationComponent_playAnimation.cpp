#include "AnimationComponent_playAnimation.hpp"

#include <iostream>

#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "Events/Game/AttackEvent.hpp"

void AnimationComponent_playAnimation::callback(void* ExpressionNode, HashedString* a2, HashedString* a3, HashedString* a4, HashedString* a5) {
    return funcOriginal(ExpressionNode, a2, a3, a4, a5);
}

AnimationComponent_playAnimation::AnimationComponent_playAnimation() : Hook("AnimationComponent_playAnimation", GET_SIG_ADDRESS("AnimationComponent::playAnimation")) {}


void AnimationComponent_playAnimation::enableHook()
{
    this->autoHook((void*)callback, (void**)&funcOriginal);
}
