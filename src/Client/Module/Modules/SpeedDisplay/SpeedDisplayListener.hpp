#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include <Windows.h>

class SpeedDisplayListener : public Listener {

	Module* module;

	Vec3<float> PrevPos;

	void onRender(RenderEvent& event) override {
		if (
			SDK::CurrentScreen == "hud_screen" &&
			module->settings.getSettingByName<bool>("enabled")->value &&
			SDK::hasInstanced && SDK::clientInstance != nullptr &&
			SDK::clientInstance->getLocalPlayer() != nullptr
			) {

			std::string speed = std::format("{:.2f}", SDK::clientInstance->getLocalPlayer()->stateVector->Pos.dist(PrevPos) * 20);

			this->module->NormalRender(15, module->settings.getSettingByName<std::string>("text")->value, speed);

		}
	}

	void onLocalTick(TickEvent& event) override {
		if (
			SDK::CurrentScreen == "hud_screen" &&
			module->settings.getSettingByName<bool>("enabled")->value &&
			SDK::hasInstanced && SDK::clientInstance != nullptr &&
			SDK::clientInstance->getLocalPlayer() != nullptr
			)
			PrevPos = SDK::clientInstance->getLocalPlayer()->stateVector->Pos;
	}

public:
	explicit SpeedDisplayListener(const char string[5], Module* module) {
		this->name = string;
		this->module = module;
	}

};

