#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Input/KeyEvent.hpp"
#include "Events/Render/SetTopScreenNameEvent.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"

class ModernKeybindHandling : public Module {
private:
    OptionsParser parser;
    std::string lastScreen;
    std::array<bool, 256> heldKeys;
    std::vector<int> movementKeys;
    bool wasInGame;
    bool sprintToggleState;
    
    void restoreMovementInput();
    void updateMovementInputHandler();
    void updateMovementKeys();
    bool isMovementKey(int key);
    bool isInGameScreen();
    
public:
    ModernKeybindHandling();
    
    void onEnable() override;
    void onDisable() override;
    void defaultConfig() override;
    void settingsRender(float settingsOffset) override;
    
    void onTick(TickEvent& event);
    void onKey(KeyEvent& event);
    void onSetTopScreenName(SetTopScreenNameEvent& event);
    
    static int safe_stoi(const std::string& str);
};