#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"


class IPDisplay : public Module {

public:


	IPDisplay() : Module("IP Display", "Displays the current server IP you're playing on.",
		IDR_SERVER_IP_PNG, "", false, {"server", "address"}) {

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};

