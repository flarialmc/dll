#include "KeyHook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../Client.hpp"

KeyHook::KeyHook() : Hook("key_hook", "48 83 EC ? ? ? C1 4C 8D 05") {}

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