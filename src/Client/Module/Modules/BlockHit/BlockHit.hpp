#pragma once

#include "../Module.hpp"
#include "../CPS/CPSCounter.hpp"
#include "../../../Events/Game/RenderItemInHandEvent.hpp"
#include "Events/Game/PerspectiveEvent.hpp"

class BlockHit : public Module {
public:
	Perspective perspective;

	BlockHit() : Module("Block Hit", "Sword Blocking Animation like Java (visual only)",
		IDR_SWORD_PNG, "") {
		
	};

	void onEnable() override;

	void onDisable() override;

	void onPerspectiveChange(PerspectiveEvent& event);

	void onItemInHandRender(RenderItemInHandEvent& event);

	void defaultConfig() override;
};

