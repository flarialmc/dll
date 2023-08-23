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
#include <string>

class BlockBreakIndicatorListener : public Listener {

	Module* module;

	void onRender(RenderEvent& event) override {
		if (
			SDK::CurrentScreen == "hud_screen" &&
			module->settings.getSettingByName<bool>("enabled")->value &&
			SDK::hasInstanced && SDK::clientInstance != nullptr &&
			SDK::clientInstance->getLocalPlayer() != nullptr
			) {

			std::string progress = std::format("{:.0f}%", (*SDK::clientInstance->getLocalPlayer()->getgamemode()).lastBreakProgress*100);
			this->module->NormalRender(16, module->settings.getSettingByName<std::string>("text")->value, progress);

		}
	}

public:
	explicit BlockBreakIndicatorListener(const char string[5], Module* module) {
		this->name = string;
		this->module = module;
	}
};

