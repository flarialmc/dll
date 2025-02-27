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

    int vkKey = MapVirtualKeyA(key, MAPVK_VSC_TO_VK);
    auto event = nes::make_holder<KeyEvent>(key, vkKey, state ? 1 : 0, keys);

    eventMgr.trigger(event);

    if (!event->isCancelled())
        funcOriginal(event->getKey(), state);
}