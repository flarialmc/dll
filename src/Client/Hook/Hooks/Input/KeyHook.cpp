#include <iostream>
#include "KeyHook.hpp"
#include "../../../Client.hpp"
#include "../../../Events/EventHandler.hpp"

std::string window = "Minecraft";
HWND hWnd = FindWindow(nullptr, window.c_str());
RECT currentRect = { 0 };
RECT clientRect = { 0 };
RECT previousRect = { 0 };
bool InHudScreen = false;
bool toes = false;

KeyHook::KeyHook() : Hook("key_hook", "48 ?? ?? ?? 0F B6 ?? 4C ?? ?? ?? ?? ?? ?? 89 ?? ?? ?? 88")
{
}

void KeyHook::enableHook()
{
	this->autoHook((void*)keyCallback, (void**)&func_original);
}

void KeyHook::keyCallback(int key, int state)
{
	if (state == (int)ActionType::PRESSED) keys[key] = true;
	else keys[key] = false;

	KeyEvent event(key, state, keys);
	EventHandler::onKey(event);

	std::to_string(event.GetKey());
	std::to_string(event.GetAction());
	std::to_string(event.isCancelled());

	if (!event.isCancelled())
		func_original(event.GetKey(), event.GetAction());

	// center cursor lol

	if (Client::settings.getSettingByName<bool>("centreCursor")->value && state == (int)ActionType::PRESSED) {
		if (!toes) {
			toes = true;
			std::thread wow([&]() {
				while (!Client::disable && Client::settings.getSettingByName<bool>("centreCursor")->value) {
					GetWindowRect(hWnd, &currentRect);
					GetClientRect(hWnd, &clientRect);

					if (memcmp(&currentRect, &previousRect, sizeof(RECT)) != 0) {
						previousRect = currentRect;
					}
					Sleep(1000);
				};
				if (Client::disable && !Client::settings.getSettingByName<bool>("centreCursor")->value) toes = false;
				});

			wow.detach();
		}

		if (SDK::CurrentScreen == "hud_screen" and !InHudScreen) InHudScreen = true;
		if (SDK::CurrentScreen != "hud_screen" and InHudScreen) {
			GetWindowRect(hWnd, &currentRect);
			GetClientRect(hWnd, &clientRect);

			int windowX = currentRect.left;
			int windowY = currentRect.top;

			int centerX = windowX + (clientRect.right) / 2;
			int centerY = windowY + (clientRect.bottom) / 2;

			SetCursorPos(centerX, centerY);

			std::cout << "cursor centered " << centerX << " " << centerY << std::endl;

			InHudScreen = false;
		}
	}
}