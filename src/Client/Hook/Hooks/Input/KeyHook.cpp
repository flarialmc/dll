#include "KeyHook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../Client.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

KeyHook::KeyHook() : Hook("key_hook", GET_SIG("Keyboard::feed")) {}

void KeyHook::enableHook() {
    this->autoHook((void *) keyCallback, (void **) &funcOriginal);
}

void KeyHook::keyCallback(unsigned char  key, int state) {
    if (Client::disable) return;
    if (state == (int) ActionType::Pressed) keys[key] = true;
    else keys[key] = false;

    KeyEvent event(key, state, keys);
    EventHandler::onKey(event);

    Sleep(0);

    if (!event.isCancelled())
        funcOriginal(event.getKey(), event.getAction());
}