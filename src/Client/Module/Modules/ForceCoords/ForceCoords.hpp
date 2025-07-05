#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"

class ForceCoords : public Module {
private:
	// 1.20.12 - static inline uintptr_t option = Memory::findSig("80 78 ? ? 74 ? b0 ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 32 c0"); // cmp byte ptr [rax+04],00
	static inline uintptr_t option;
	bool mojanged = false;
	bool patched = false;

	static inline std::vector<uint8_t> original_option;
	static inline std::vector<uint8_t> patched_option;
public:
	ForceCoords() : Module("Coordinates", "Shows your ingame position. (XYZ)", IDR_COORDINATES_PNG,
		"") {
		option = GET_SIG_ADDRESS("ForceCoordsOption");

		original_option.resize(4);
		memcpy(original_option.data(), (LPVOID)option, 4);

		// 4 nops
		patched_option.push_back(0x90);
		patched_option.push_back(0x90);
		patched_option.push_back(0x90);
		patched_option.push_back(0x90);
		

		loadSettings();
	};

	void onEnable() override;

	void onDisable() override;

	void patch();

	void unpatch();

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
};

