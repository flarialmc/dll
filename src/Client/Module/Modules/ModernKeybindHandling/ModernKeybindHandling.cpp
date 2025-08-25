#include "ModernKeybindHandling.hpp"

#include "Events/EventManager.hpp"
#include "Hook/Hooks/Input/KeyHook.hpp"

ModernKeybindHandling::ModernKeybindHandling() : Module("Modern Handling",
    "Fixes the keybind issue where movement doesn't resume after exiting GUI screens while holding keys.",
    IDR_KEYBOARD_PNG, "")
{
    heldKeys.fill(false);
    wasInGame = true;
    lastScreen = "";
    restoreQueued = false;
    shouldContinueTracking = false;
}

void ModernKeybindHandling::onEnable()
{
    parser.parseOptionsFile();
    updateMovementKeys();
    
    Listen(this, TickEvent, &ModernKeybindHandling::onTick)
    Listen(this, KeyEvent, &ModernKeybindHandling::onKey)
    Listen(this, SetTopScreenNameEvent, &ModernKeybindHandling::onSetTopScreenName)
    
    Module::onEnable();
}

void ModernKeybindHandling::onDisable()
{
    Deafen(this, TickEvent, &ModernKeybindHandling::onTick)
    Deafen(this, KeyEvent, &ModernKeybindHandling::onKey)
    Deafen(this, SetTopScreenNameEvent, &ModernKeybindHandling::onSetTopScreenName)
    
    Module::onDisable();
}

void ModernKeybindHandling::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("fixInventoryMovement", true);
    setDef("fixPauseMovement", true);
    setDef("fixChatMovement", true);
    setDef("fixAllScreens", false);
    
}

void ModernKeybindHandling::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Modern Keybind Handling");
    addToggle("Fix Inventory Movement", "Restore movement and sprint input after exiting inventory screens", "fixInventoryMovement");
    addToggle("Fix Pause Movement", "Restore movement and sprint input after exiting pause menu", "fixPauseMovement");
    addToggle("Fix Chat Movement", "Restore movement and sprint input after exiting chat screen", "fixChatMovement");
    addToggle("Fix All Screens", "Restore movement and sprint input after exiting any GUI screen", "fixAllScreens");

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ModernKeybindHandling::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    
    bool currentlyInGame = isInGameScreen();
    
    if (restoreQueued && currentlyInGame) {
        restoreMovementInput();
        restoreQueued = false;
        shouldContinueTracking = true;
    }
    
    if (shouldContinueTracking && currentlyInGame) {
        updateMovementInputHandler();
        
        bool anyKeyHeld = false;
        for (int key : movementKeys) {
            if (heldKeys[key]) {
                anyKeyHeld = true;
                break;
            }
        }
        
        if (!anyKeyHeld) {
            shouldContinueTracking = false;
        }
    }
    
    wasInGame = currentlyInGame;
}

void ModernKeybindHandling::onKey(KeyEvent& event)
{
    if (!this->isEnabled()) return;
    
    int key = event.getKey();
    ActionType action = event.getAction();
    
    if (isMovementKey(key)) {
        if (action == ActionType::Pressed) {
            heldKeys[key] = true;
        } else if (action == ActionType::Released) {
            heldKeys[key] = false;
        }
    }
}

void ModernKeybindHandling::onSetTopScreenName(SetTopScreenNameEvent& event)
{
    if (!this->isEnabled()) return;
    
    std::string newScreen = event.getLayer();
    std::string previousScreen = lastScreen;
    lastScreen = newScreen;
    
    // Check if changing from a non-game screen to hud_screen (game)
    if (previousScreen != "hud_screen" && newScreen == "hud_screen") {
        bool shouldRestore = false;
        
        if (getOps<bool>("fixAllScreens")) {
            shouldRestore = true;
        } else {
            if (getOps<bool>("fixInventoryMovement") && 
                (previousScreen.find("inventory") != std::string::npos || 
                 previousScreen.find("chest") != std::string::npos ||
                 previousScreen.find("furnace") != std::string::npos ||
                 previousScreen.find("crafting") != std::string::npos)) {
                shouldRestore = true;
            }
            if (getOps<bool>("fixPauseMovement") && previousScreen.find("pause") != std::string::npos) {
                shouldRestore = true;
            }
            if (getOps<bool>("fixChatMovement") && previousScreen.find("chat") != std::string::npos) {
                shouldRestore = true;
            }
        }
        
        if (shouldRestore) {
            restoreQueued = true;
        }
    }
}

void ModernKeybindHandling::restoreMovementInput()
{
    updateMovementKeys();
    
    if (SDK::clientInstance != nullptr && SDK::clientInstance->getLocalPlayer() != nullptr) {
        auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
        if (handler != nullptr) {
            updateMovementInputHandler();
        }
    }
}

void ModernKeybindHandling::updateMovementInputHandler()
{
    if (SDK::clientInstance == nullptr || SDK::clientInstance->getLocalPlayer() == nullptr) return;
    
    auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
    if (handler == nullptr) return;

    auto module = ModuleManager::getModule("Toggle Sprint");
    if (module == nullptr) return;
    
    updateMovementKeys();
    
    for (int i = 0; i < movementKeys.size(); i++) {
        int key = movementKeys[i];
        bool isKeyHeld = heldKeys[key];
        
        if (i == 0) {
            handler->mInputState.forward = isKeyHeld;
            handler->mRawInputState.forward = isKeyHeld;
        }
        else if (i == 1) {
            handler->mInputState.backward = isKeyHeld;
            handler->mRawInputState.backward = isKeyHeld;
        }
        else if (i == 2) {
            handler->mInputState.left = isKeyHeld;
            handler->mRawInputState.left = isKeyHeld;
        }
        else if (i == 3) {
            handler->mInputState.right = isKeyHeld;
            handler->mRawInputState.right = isKeyHeld;
        }
        else if (i == 4) {
            handler->jumping = isKeyHeld;
            handler->mInputState.mJumpDown = isKeyHeld;
            handler->mRawInputState.mJumpDown = isKeyHeld;
        }
        else if (i == 5) {
            handler->sneaking = isKeyHeld;
            handler->mInputState.mSneakDown = isKeyHeld;
            handler->mRawInputState.mSneakDown = isKeyHeld;
        }
        else if (i == 6 && !module->isEnabled()) {
            handler->sprinting = isKeyHeld;
            handler->mInputState.mSprintDown = isKeyHeld;
            handler->mRawInputState.mSprintDown = isKeyHeld;
        }
    }
}

void ModernKeybindHandling::updateMovementKeys()
{
    movementKeys.clear();
    
    try {
        movementKeys.push_back(safe_stoi(parser.options["keyboard_type_0_key.forward"]));
        movementKeys.push_back(safe_stoi(parser.options["keyboard_type_0_key.back"]));
        movementKeys.push_back(safe_stoi(parser.options["keyboard_type_0_key.left"]));
        movementKeys.push_back(safe_stoi(parser.options["keyboard_type_0_key.right"]));
        movementKeys.push_back(safe_stoi(parser.options["keyboard_type_0_key.jump"]));
        movementKeys.push_back(safe_stoi(parser.options["keyboard_type_0_key.sneak"]));
        movementKeys.push_back(safe_stoi(parser.options["keyboard_type_0_key.sprint"]));
    } catch (const std::exception& e) {
        movementKeys = {87, 83, 65, 68, 32, 16, 17};
    }
}

bool ModernKeybindHandling::isMovementKey(int key)
{
    return std::find(movementKeys.begin(), movementKeys.end(), key) != movementKeys.end();
}

bool ModernKeybindHandling::isInGameScreen()
{
    std::string currentScreen = SDK::getCurrentScreen();
    return currentScreen == "hud_screen";
}

int ModernKeybindHandling::safe_stoi(const std::string& str)
{
    char* end;
    long value = std::strtol(str.c_str(), &end, 10);
    if (*end != '\0') {
        throw std::invalid_argument("Invalid integer: " + str);
    }
    return static_cast<int>(value);
}