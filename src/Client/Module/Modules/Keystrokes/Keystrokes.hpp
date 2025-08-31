#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <windows.h>
#include <unknwn.h>

#include "Events/Render/RenderEvent.hpp"

class Keystrokes : public Module {

public:

	enum Strokes {
		W,
		A,
		S,
		D,
		SPACEBAR,
		LMB,
		RMB
	};

	std::vector<D2D1_COLOR_F> states;
	float glowAmountModifier[7] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	std::vector<D2D1_COLOR_F> textStates;
	std::vector<D2D1_COLOR_F> shadowStates;


	Keystrokes() : Module("Keystrokes", "Displays real-time information about your\nWASD and mouse inputs.",
		IDR_KEYSTROKES_PNG, "") {
		
	};

	void onSetup() override;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	static std::pair<float, float>
		centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight);

	void normalRender(int index, std::string& value) override;

	void onRender(RenderEvent& event);
};
