#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class HitboxListener : public Listener {

	Module* module;
public:

    static inline std::unordered_map<std::string*, bool> canSeeArrXD;
	void onRender(RenderEvent& event) override;

	explicit HitboxListener(const char string[5], Module* module) {
		this->name = string;
		this->module = module;
	}
};
