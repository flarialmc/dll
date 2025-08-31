#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/CommandRequestPacket.hpp"

class CommandHotkey : public Module {

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> last_used;

public:

	CommandHotkey() : Module("Command Hotkey", "Send command with a\nclick of a button!",
		IDR_TEXT_BOX_PNG, "") {
	};

	void addHotkey(int index);

	void onSetup() override;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onKey(KeyEvent& event);

	void onMouse(MouseEvent& event);
};