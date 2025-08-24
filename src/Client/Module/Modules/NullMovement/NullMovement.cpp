#include "NullMovement.hpp"

#include "Events/EventManager.hpp"
#include "Hook/Hooks/Input/KeyHook.hpp"

NullMovement::NullMovement(): Module("Null Movement", "Only registers the latest movement key.",
                                     IDR_SPEED_PNG, "")
{

    
}

void NullMovement::onEnable()
{
    parser.parseOptionsFile();
    Listen(this, KeyEvent, &NullMovement::onKey)
    Listen(this, TickEvent, &NullMovement::onTick)

    Module::onEnable();
}

void NullMovement::onDisable()
{
    Deafen(this, KeyEvent, &NullMovement::onKey)
    Deafen(this, TickEvent, &NullMovement::onTick)
    Module::onDisable();
}

void NullMovement::defaultConfig()
{
    Module::defaultConfig("core");
    setDef("horizontal", true);
    setDef("vertical", false);
    
}

void NullMovement::settingsRender(float settingsOffset)
{
    initSettingsPage();

    addHeader("Misc");
    addToggle("Vertical Nulling", "W & S keys", "vertical");
    addToggle("Horizontal Nulling", "A & D keys", "horizontal");

    FlarialGUI::UnsetScrollView();

    resetPadding();

}

void NullMovement::onTick(TickEvent& event)
{
    if (!this->isEnabled()) return;
    
    static int forwardKey = safe_stoi(parser.options["keyboard_type_0_key.forward"]);
    static int backwardKey = safe_stoi(parser.options["keyboard_type_0_key.back"]);
    static int leftKey = safe_stoi(parser.options["keyboard_type_0_key.left"]);
    static int rightKey = safe_stoi(parser.options["keyboard_type_0_key.right"]);

    if (SDK::clientInstance != nullptr && SDK::clientInstance->getLocalPlayer() != nullptr && (
        SDK::getCurrentScreen() == "hud_screen" ||
        SDK::getCurrentScreen() == "zoom_screen" ||
        SDK::getCurrentScreen() == "f3_screen"
        )) {
        auto* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
        if (handler != nullptr) {
            if (!movementKeyStack.empty()) {
                int lastKey = movementKeyStack.back();
                
                if (getOps<bool>("vertical")) {
                    if (lastKey == forwardKey) {
                        handler->mInputState.forward = true;
                        handler->mRawInputState.forward = true;
                        handler->mInputState.backward = false;
                        handler->mRawInputState.backward = false;
                    } else if (lastKey == backwardKey) {
                        handler->mInputState.forward = false;
                        handler->mRawInputState.forward = false;
                        handler->mInputState.backward = true;
                        handler->mRawInputState.backward = true;
                    }
                }
                
                if (getOps<bool>("horizontal")) {
                    if (lastKey == leftKey) {
                        handler->mInputState.left = true;
                        handler->mRawInputState.left = true;
                        handler->mInputState.right = false;
                        handler->mRawInputState.right = false;
                    } else if (lastKey == rightKey) {
                        handler->mInputState.left = false;
                        handler->mRawInputState.left = false;
                        handler->mInputState.right = true;
                        handler->mRawInputState.right = true;
                    }
                }
            } else {
                if (getOps<bool>("vertical")) {
                    handler->mInputState.forward = false;
                    handler->mRawInputState.forward = false;
                    handler->mInputState.backward = false;
                    handler->mRawInputState.backward = false;
                }
                
                if (getOps<bool>("horizontal")) {
                    handler->mInputState.left = false;
                    handler->mRawInputState.left = false;
                    handler->mInputState.right = false;
                    handler->mRawInputState.right = false;
                }
            }
        }
    }
}

void NullMovement::onKey(KeyEvent& event)
{
    if (!this->isEnabled() && (
        SDK::getCurrentScreen() != "hud_screen" &&
        SDK::getCurrentScreen() != "zoom_screen" &&
        SDK::getCurrentScreen() != "f3_screen"
        )) return;
    lastKey = event.getKey();
    lastKeys = event.keys;
    lastAction = event.getAction();

    static int forwardKey = safe_stoi(parser.options["keyboard_type_0_key.forward"]);
    static int backwardKey = safe_stoi(parser.options["keyboard_type_0_key.back"]);
    static int leftKey = safe_stoi(parser.options["keyboard_type_0_key.left"]);
    static int rightKey = safe_stoi(parser.options["keyboard_type_0_key.right"]);

    bool isMovementKey = (lastKey == forwardKey ||
        lastKey == backwardKey ||
        lastKey == leftKey ||
        lastKey == rightKey);

    if (lastAction == ActionType::Pressed && isMovementKey) unpresser = true;

    if (!isMovementKey)
        return;
        
    if (lastAction == ActionType::Pressed) {
        auto it = std::find(movementKeyStack.begin(), movementKeyStack.end(), lastKey);
        if (it != movementKeyStack.end()) {
            movementKeyStack.erase(it);
        }
        movementKeyStack.push_back(lastKey);
    }
    else if (lastAction == ActionType::Released) {
        movementKeyStack.erase(
            std::remove(movementKeyStack.begin(), movementKeyStack.end(), lastKey),
            movementKeyStack.end()
        );
    }
}

int NullMovement::safe_stoi(const std::string& str)
{
    char* end;
    long value = std::strtol(str.c_str(), &end, 10);
    if (*end != '\0') {
        throw std::invalid_argument("Invalid integer: " + str);
    }
    return static_cast<int>(value);
}
