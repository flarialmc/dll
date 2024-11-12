#include "KeyHook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

KeyHook::KeyHook() : Hook("key_hook", GET_SIG_ADDRESS("Keyboard::feed")) {}

void KeyHook::enableHook() {
    this->autoHook((void *) keyCallback, (void **) &funcOriginal);
}

void KeyHook::keyCallback(int key, bool state) {
    keys[key] = state;

    auto event = nes::make_holder<KeyEvent>(key, state ? 1 : 0, keys);

    eventMgr.trigger(event);

//    auto currentScreen = SDK::getCurrentScreen();
//    if (currentScreen != "hud_screen" &&
//        currentScreen != "f1_screen" &&
//        currentScreen != "zoom_screen"&&
//        currentScreen != "start_screen"&&
//        currentScreen != "play_screen" && state) return funcOriginal(key, state); // if were on those screens and key is pressed

    if (!event->isCancelled())
        funcOriginal(event->getKey(), state);
}