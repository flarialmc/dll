#pragma once

#include "../Module.hpp"
#include "Events/Render/ActorShaderParamsEvent.hpp"
#include "GlintColor.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"



class GlintColor : public Module {
public:
	GlintColor() : Module("Glint Color", "Change the glint color of enchanted items.",
		IDR_SWORD_PNG, "", false, {"enchantment"}) {

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void patchCol(ActorShaderParamsEvent& event);
};
