#include "KeyHook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../../Utils/UserActionLogger.hpp"
#include "../../../Module/Manager.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

KeyHook::KeyHook() : Hook("key_hook", GET_SIG_ADDRESS("Keyboard::feed")) {}

void KeyHook::enableHook() {
    this->autoHook((void *) keyCallback, (void **) &funcOriginal);
}

void KeyHook::keyCallback(int key, bool state) {
    keys[key] = state;

    auto event = nes::make_holder<KeyEvent>(key, state ? 1 : 0, keys);

    eventMgr.trigger(event);

    // Log key press for crash telemetry (but filter hud_screen to only important keys)
    std::string context = SDK::currentScreen.empty() ? "menu" : SDK::currentScreen;
    bool shouldLog = true;

    if (SDK::currentScreen == "hud_screen") {
        // In hud_screen, only log if this is likely a keybind (not regular gameplay keys)
        // Skip common gameplay keys like WASD, mouse buttons, etc.
        shouldLog = (key >= 112 && key <= 123) || // F1-F12 keys
                   (key >= 48 && key <= 57) ||     // Number keys 0-9
                   (key >= 96 && key <= 105) ||    // Numpad 0-9
                   key == 27 ||                    // ESC
                   key == 9 ||                     // TAB
                   key == 160 || key == 161 ||     // Shift keys
                   key == 162 || key == 163 ||     // Ctrl keys
                   key == 164 || key == 165;       // Alt keys

        if (shouldLog) {
            context += "_potential_keybind";
        }
    }

    if (shouldLog) {
        UserActionLogger::logKeyPress(key, state, context);
    }

    if (!event->isCancelled()) funcOriginal(event->getKey(), state);
}