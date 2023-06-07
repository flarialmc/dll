#include "KeyHook.hpp"
#include "../../../Events/EventHandler.hpp"

KeyHook::KeyHook() : Hook("key_hook", "48 ?? ?? ?? 0F B6 ?? 4C ?? ?? ?? ?? ?? ?? 89 ?? ?? ?? 88")
{
}

void KeyHook::enableHook()
{
    this->autoHook((void *)keyCallback, (void **)&func_original);
}

void KeyHook::keyCallback(int key, int state)
{
    // Sets the key state
    keys[key] = state;

    KeyEvent event(key, state);
    EventHandler::onKey(event);

    if (!event.isCancelled())
        return func_original(event.GetKey(), static_cast<int>(event.GetAction()));
}