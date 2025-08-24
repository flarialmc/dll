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
	initSettingsPage();

	addHeader("Glint Color");
	addColorPicker("Color", "", "glint");

	FlarialGUI::UnsetScrollView();
	resetPadding();
}

void GlintColor::patchCol(ActorShaderParamsEvent& event) {
	D2D1_COLOR_F color = getColor("glint");
	event.setGlintColorFromD2D(color, getOps<float>("glintOpacity"));
}