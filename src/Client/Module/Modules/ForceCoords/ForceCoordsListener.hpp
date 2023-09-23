#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include <Windows.h>

class ForceCoordsListener : public Listener {

	Module* module;
private:
	// 1.20.12 - static inline uintptr_t option = Memory::findSig("80 78 ? ? 74 ? b0 ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 32 c0"); // cmp byte ptr [rax+04],00
	static inline uintptr_t option = Memory::findSig("80 78 ? ? 0F 95 C0 48 8B 5C 24");
	bool mojanged = false;

	static inline std::vector<uint8_t> original_option;
	static inline std::vector<uint8_t> patched_option;

public:
	void onRender(RenderEvent& event) override {

		if (SDK::CurrentScreen == "hud_screen")
			if (module->settings.getSettingByName<bool>("enabled")->value) {

				if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

					if (SDK::clientInstance->getLocalPlayer() != nullptr) {

						if (module->settings.getSettingByName<bool>("MojangStyle")->value && !mojanged) {
							patch();
							mojanged = true;
						}
						else if (!module->settings.getSettingByName<bool>("MojangStyle")->value) {
							if (mojanged) {
								unpatch();
								mojanged = false;
							}

							Vec3<float> Pos = SDK::clientInstance->getLocalPlayer()->stateVector->Pos;
							//Vec3<float> PrevPos = SDK::clientInstance->getLocalPlayer()->stateVector->PrevPos;
							//Vec3<float> vel = SDK::clientInstance->getLocalPlayer()->stateVector->velocity;

							std::string cords = std::to_string(static_cast<int>(Pos.x)) + ", " + std::to_string(static_cast<int>(Pos.y)) + ", " + std::to_string(static_cast<int>(Pos.z));
							//std::string cords1 = std::to_string(static_cast<int>(PrevPos.x)) + ", " + std::to_string(static_cast<int>(PrevPos.y)) + ", " + std::to_string(static_cast<int>(PrevPos.z));
							//std::string cords2 = std::to_string(static_cast<int>(vel.x)) + ", " + std::to_string(static_cast<int>(vel.y)) + ", " + std::to_string(static_cast<int>(vel.z));
							this->module->NormalRender(6, module->settings.getSettingByName<std::string>("text")->value, cords);
						}
					}
				}
			}
			else if (mojanged) {
				unpatch();
				mojanged = false;
			}
	}
	
	void patch() {
		DWORD oldProtect;
		VirtualProtect((LPVOID)option, patched_option.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy((LPVOID)option, patched_option.data(), patched_option.size());
		VirtualProtect((LPVOID)option, patched_option.size(), oldProtect, &oldProtect);
	};

	static void unpatch() {
		DWORD oldProtect;
		VirtualProtect((LPVOID)option, original_option.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy((LPVOID)option, original_option.data(), original_option.size());
		VirtualProtect((LPVOID)option, original_option.size(), oldProtect, &oldProtect);
	};

public:
	explicit ForceCoordsListener(const char string[5], Module* module) {
		this->name = string;
		this->module = module;

		original_option.resize(4);
		memcpy(original_option.data(), (LPVOID)option, 4);

		// 4 nops
		patched_option.push_back(0x90);
		patched_option.push_back(0x90);
		patched_option.push_back(0x90);
		patched_option.push_back(0x90);
	}
};

