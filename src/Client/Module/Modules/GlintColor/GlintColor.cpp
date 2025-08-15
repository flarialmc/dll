#pragma once

#include "../Module.hpp"
#include "Events/Render/ActorShaderParamsEvent.hpp"
#include "GlintColor.hpp"

#include "Events/EventManager.hpp"

void GlintColor::onEnable(){
	Listen(this, ActorShaderParamsEvent, &GlintColor::patchCol)
	Module::onEnable();
}

void GlintColor::onDisable(){
	Deafen(this, ActorShaderParamsEvent, &GlintColor::patchCol)
	Module::onDisable();
}

void GlintColor::defaultConfig(){
	settings.renameSetting("color", "colorOpacity", "color_rgb", "glint");
	Module::defaultConfig("core");
	setDef("glint", "FFFFFF", 0.65f, false);
}

void GlintColor::settingsRender(float settingsOffset){

	float x = Constraints::PercentageConstraint(0.019, "left");
	float y = Constraints::PercentageConstraint(0.10, "top");

	const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

	FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
	FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
							  Constraints::RelativeConstraint(1.0, "width"),
							  Constraints::RelativeConstraint(0.88f, "height"));

	addHeader("Glint Color");
	addColorPicker("Color", "", "glint");

	FlarialGUI::UnsetScrollView();
	resetPadding();
}

void GlintColor::patchCol(ActorShaderParamsEvent& event) {
	D2D1_COLOR_F color = getColor("glint");
	event.setGlintColorFromD2D(color, getOps<float>("glintOpacity"));
}