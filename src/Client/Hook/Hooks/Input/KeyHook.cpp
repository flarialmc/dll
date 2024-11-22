#include "KeyHook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

KeyHook::KeyHook() : Hook("key_hook", GET_SIG("Keyboard::feed")) {}

void KeyHook::enableHook() {
    this->autoHook((void *) keyCallback, (void **) &funcOriginal);
}

void KeyHook::keyCallback(int key, bool state) {
    keys[key] = state;

    KeyEvent event(key, state ? 1 : 0, keys);
    EventHandler::onKey(event);

    if (!event.isCancelled())
        funcOriginal(event.getKey(), state);
}