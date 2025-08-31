#pragma once

#include "../Module.hpp"
#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Game/UpdatePlayerEvent.hpp"

class FreeLook : public Module {
private:
	static inline uintptr_t yaw1;
	static inline uintptr_t yaw2;
	static inline uintptr_t pitch;
	static inline uintptr_t movement;

	static inline uint8_t nop[4] = { 0x90, 0x90, 0x90, 0x90 };
	// TODO: this might cause a crash if version is updated
	static inline std::array<std::byte, 4> originalYaw1;
	static inline std::array<std::byte, 4> originalYaw2;
	static inline std::array<std::byte, 4> originalPitch;
	static inline std::array<std::byte, 4> originalMovement;
public:

	FreeLook() : Module("FreeLook",
		"Freely move your camera in 3rd person mode\nwhile keeping the player rotation the same.",
		IDR_FREELOOK_PNG, "F") {

		yaw1 = GET_SIG_ADDRESS("CameraYaw");
		yaw2 = GET_SIG_ADDRESS("CameraYaw2");

		originalYaw1 = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) yaw1);
		originalYaw2 = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) yaw2);

		if (!VersionUtils::checkAboveOrEqual(21, 40)) {
			pitch = GET_SIG_ADDRESS("CameraPitch");
			movement = GET_SIG_ADDRESS("CameraMovement");

			originalPitch = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) pitch);
			originalMovement = std::bit_cast<std::array<std::byte, 4>>(*(std::byte(*)[4]) movement);
		}

		
	};

	void onSetup() override;

	void onEnable() override;

	void onDisable() override;

	void patch();

	void unpatch();

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onUpdatePlayer(UpdatePlayerEvent& event);

	void onGetViewPerspective(PerspectiveEvent& event);

	void onKey(KeyEvent& event);

	void onMouse(MouseEvent& event);
};
