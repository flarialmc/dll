#pragma once

#include "../Module.hpp"


class NullMovement : public Module {
public:
    NullMovement() : Module("Null Movement", "Only registers the latest movement key.",
                        IDR_SPEED_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        Listen(this, TickEvent, &NullMovement::onTick)
        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, TickEvent, &NullMovement::onTick)
        Module::onDisable();
    }

    void defaultConfig() override {
    }

 enum class LastPressed {
    None,
    Left,
    Right,
    Forward,
    Backward
};

LastPressed lastHorizontalPressed = LastPressed::None;
LastPressed lastVerticalPressed = LastPressed::None;

void updateLastPressedDirection(bool leftPressed, bool rightPressed, bool forwardPressed, bool backwardPressed) {
    if (rightPressed && !leftPressed) {
        lastHorizontalPressed = LastPressed::Right;
    } else if (leftPressed && !rightPressed) {
        lastHorizontalPressed = LastPressed::Left;
    } else if (leftPressed && rightPressed) {
        lastHorizontalPressed = (lastHorizontalPressed == LastPressed::Left) ? LastPressed::Right : LastPressed::Left;
    }

    if (forwardPressed && !backwardPressed) {
        lastVerticalPressed = LastPressed::Forward;
    } else if (backwardPressed && !forwardPressed) {
        lastVerticalPressed = LastPressed::Backward;
    } else if (forwardPressed && backwardPressed) {
        lastVerticalPressed = (lastVerticalPressed == LastPressed::Forward) ? LastPressed::Backward : LastPressed::Forward;
    }
}

void onTick(TickEvent &event) {
    auto *handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

    if (handler != nullptr) {
        auto* inputComponent = reinterpret_cast<MoveInputComponent*>(handler);

        bool leftPressed = inputComponent->left;
        bool rightPressed = inputComponent->right;
        bool forwardPressed = inputComponent->forward;
        bool backwardPressed = inputComponent->backward;

        updateLastPressedDirection(leftPressed, rightPressed, forwardPressed, backwardPressed);

        if (leftPressed && rightPressed) {
            if (lastHorizontalPressed == LastPressed::Right) {
                inputComponent->left = false;
            } else {
                inputComponent->right = false;
            }
        }

        if (forwardPressed && backwardPressed) {
            if (lastVerticalPressed == LastPressed::Forward) {
                inputComponent->backward = false;
            } else {
                inputComponent->forward = false;
            }
        }
    }
}



};
