#pragma once

#include "../Module.hpp"


class NullMovement : public Module {
    OptionsParser parser;
    int lastKey;
    ActionType lastAction;

public:
    NullMovement() : Module("Null Movement", "Only registers the latest movement key.",
                        IDR_SPEED_PNG, "") {

        Module::setup();
    };

    void onEnable() override {
        parser.parseOptionsFile();
        Listen(this, KeyEvent, &NullMovement::onKey)
        Listen(this, TickEvent, &NullMovement::onTick)

        Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, KeyEvent, &NullMovement::onKey)
        Deafen(this, TickEvent, &NullMovement::onTick)
        Module::onDisable();
    }

    void defaultConfig() override {
    }


    void onKey(KeyEvent &event) {
        lastKey = event.getKey();
        lastAction = event.getAction();
        }

    void onTick(TickEvent &event) {
                    if (SDK::clientInstance && SDK::clientInstance->getLocalPlayer() && SDK::clientInstance->getLocalPlayer()->getMoveInputHandler()){
                auto *handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

                static std::vector<int> movementKeyStack;
                std::string forwardKey  = parser.options["keyboard_type_0_key.forward"];
                std::string backwardKey = parser.options["keyboard_type_0_key.backward"];
                std::string leftKey     = parser.options["keyboard_type_0_key.left"];
                std::string rightKey    = parser.options["keyboard_type_0_key.right"];
                std::string eventkey = std::to_string(lastKey);
                bool isMovementKey = (eventkey == forwardKey ||
                                      eventkey == backwardKey ||
                                      eventkey == leftKey ||
                                      eventkey == rightKey);
                if (!isMovementKey)
                    return;
                if (lastAction == ActionType::Pressed) {
                    auto it = std::find(movementKeyStack.begin(), movementKeyStack.end(), lastKey);
                    if (it != movementKeyStack.end()) {
                        movementKeyStack.erase(it);
                    }
                    movementKeyStack.push_back(lastKey);
                } else if (lastAction == ActionType::Released) {
                    movementKeyStack.erase(
                        std::remove(movementKeyStack.begin(), movementKeyStack.end(), lastKey),
                        movementKeyStack.end()
                    );
                }
                handler->forward  = false;
                handler->backward = false;
                handler->left     = false;
                handler->right    = false;
                if (!movementKeyStack.empty()) {
                    std::string lastKey = std::to_string(movementKeyStack.back());
                    if (lastKey == forwardKey)
                        handler->forward = true;
                    else if (lastKey == backwardKey)
                        handler->backward = true;
                    else if (lastKey == leftKey)
                        handler->left = true;
                    else if (lastKey == rightKey)
                        handler->right = true;
                }
            }
    }



};
