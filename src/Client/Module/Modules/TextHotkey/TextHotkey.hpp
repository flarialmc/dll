#pragma once

#include <SDK/Client/Network/Packet/TextPacket.hpp>
#include "../../../Client.hpp"
#include "../Module.hpp"
#include "../../../../Assets/Assets.hpp"

class TextHotkey : public Module {
	std::chrono::time_point<std::chrono::high_resolution_clock> last_used;
public:
	TextHotkey() : Module("Text Hotkey", "Send something in chat with a\nclick of a button!",
		IDR_TEXT_BOX_PNG, "", false, {"chat"}) {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
	void onSetup() override;
	void onKey(KeyEvent& event);
};