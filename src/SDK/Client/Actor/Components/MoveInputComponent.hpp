#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

struct MoveInputComponent {
    // ? calculateMoveVector@PlayerMovement
    BUILD_ACCESS(this, bool, forward, GET_OFFSET("MoveInputComponent::forward"));
    BUILD_ACCESS(this, bool, backward, GET_OFFSET("MoveInputComponent::backward"));
    BUILD_ACCESS(this, bool, left, GET_OFFSET("MoveInputComponent::left"));
    BUILD_ACCESS(this, bool, right, GET_OFFSET("MoveInputComponent::right"));

    BUILD_ACCESS(this, bool, sneaking, GET_OFFSET("MoveInputComponent::sneaking")); // ?setSneaking@Actor ?
    BUILD_ACCESS(this, bool, jumping, GET_OFFSET("MoveInputComponent::jumping"));
    BUILD_ACCESS(this, bool, sprinting, GET_OFFSET("MoveInputComponent::sprinting"));

};