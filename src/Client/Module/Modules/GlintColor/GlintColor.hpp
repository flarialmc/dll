#pragma once

#include "../Module.hpp"
#include "Events/Render/ActorShaderParamsEvent.hpp"
#include "GlintColor.hpp"

#include "Events/EventManager.hpp"

class GlintColor : public Module {
public:
	GlintColor() : Module("Glint Color", "Change the glint color of enchanted items.", IDR_SWORD_PNG, "") {};
	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void patchCol(ActorShaderParamsEvent& event);
};
