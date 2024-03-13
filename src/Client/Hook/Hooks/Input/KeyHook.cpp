#include <iostream>
#include "KeyHook.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../Client.hpp"
#include "../../../Events/EventHandler.hpp"

KeyHook::KeyHook() : Hook("key_hook", "48 83 EC ? ? ? C1 4C 8D 05")
{
}

void KeyHook::enableHook()
{
	this->autoHook((void*)keyCallback, (void**)&func_original);
}

void KeyHook::keyCallback(unsigned char key, int state)
{
	if (Client::disable) return;
	if (state == (int)ActionType::PRESSED) keys[key] = true;
	else keys[key] = false;

	KeyEvent event(key, state, keys);
	EventHandler::onKey(event);

	Sleep(0);

	if (!event.isCancelled())
		func_original(event.GetKey(), event.GetAction());
}