#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Client.hpp"
#include <Windows.h>
#include <thread>

class SpeedDisplayListener : public Listener {

	Module* module;

	Vec3<float> PrevPos;
	std::string speed;
	bool toes = false;
	int interval = 50; //ms

	void onRender(RenderEvent& event) override {
		if (
			SDK::CurrentScreen == "hud_screen" &&
			module->settings.getSettingByName<bool>("enabled")->value &&
			SDK::hasInstanced && SDK::clientInstance != nullptr &&
			SDK::clientInstance->getLocalPlayer() != nullptr
			) this->module->NormalRender(15, module->settings.getSettingByName<std::string>("text")->value, speed);
	}

	void update() {
		if (
			SDK::CurrentScreen == "hud_screen" &&
			module->settings.getSettingByName<bool>("enabled")->value &&
			SDK::hasInstanced && SDK::clientInstance != nullptr &&
			SDK::clientInstance->getLocalPlayer() != nullptr
			) {

			speed = std::format("{:.2f}", SDK::clientInstance->getLocalPlayer()->stateVector->Pos.dist(PrevPos) * 20);
		}
	};

	void onLocalTick(TickEvent& event) override {
		if (!SDK::clientInstance->getLocalPlayer())
			return;

		if (!toes) {
			toes = true;
			std::thread lol([&]() {
				while (!Client::disable) {
					update();
					Sleep(interval);
				}
			});
			lol.detach();
		}
		PrevPos = SDK::clientInstance->getLocalPlayer()->stateVector->Pos;
	}

public:
	explicit SpeedDisplayListener(const char string[5], Module* module) {
		this->name = string;
		this->module = module;
	}
};

