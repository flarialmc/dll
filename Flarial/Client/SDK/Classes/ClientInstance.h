#pragma once
#include "LocalPlayer.h"
#include "GuiData.h"
#include "MinecraftGame.h"
#include "LevelRenderer.h"
#include "MoveInputHandler.h"
#include "LoopbackPacketSender.h"

class ClientInstance {
	uintptr_t** vTable;
public:
	auto getLocalPlayer(void) -> LocalPlayer* {
		using getLocalPlayer = LocalPlayer * (__thiscall*)(void*);
		getLocalPlayer _getLocalPlayer = (getLocalPlayer)(this->vTable[24]);
		return _getLocalPlayer(this);

		// return Utils::CallVFunc<24, LocalPlayer*>(this);
	};
public:
	auto getGuiData(void) -> GuiData* {
		return *(GuiData**)((uintptr_t)(this) + 0x500);
	};
public:
	auto getMcGame(void) -> MinecraftGame* {
		return *(MinecraftGame**)((uintptr_t)(this) + 0xA8);
	};
public:
	auto getLevelRenderer(void) -> LevelRenderer* {
		return *(LevelRenderer**)((uintptr_t)(this) + 0xD0);
	};
public:
	auto getMoveInputHandler(void) -> MoveInputHandler* {
		return *(MoveInputHandler**)((uintptr_t)(this) + 0xF8);
	};
public:
	auto getLpSender(void) -> LoopbackPacketSender* {
		return *(LoopbackPacketSender**)((uintptr_t)(this) + 0xE0);
	};
public:
	auto getFov(void) -> Vec2<float> {
		return Vec2<float>(*(float*)((uintptr_t)(this) + 0x690), *(float*)((uintptr_t)(this) + 0x6A4));
	};
public:
	auto getbadrefdef(void) -> glmatrixf* {
		return (glmatrixf*)((uintptr_t)(this) + 0x2F0);
	};
public:
	void grabMouse() {
		return Utils::CallVFunc<303, void>(this);
	}

	void releaseMouse() {
		return Utils::CallVFunc<304, void>(this);
	}

	void refocusMouse() {
		return Utils::CallVFunc<305, void>(this);
	}
public:
	std::string getTopScreenName() {

		std::string out;
		Utils::CallVFunc<240, void, std::string*>(this, &out);

		return out;

	};
};