#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#include "Manager.hpp"

#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <unknwn.h>
#include "Assets/Assets.hpp"

#pragma comment(lib, "Ws2_32.lib")



class MumbleLink : public Module {
public:
	Vec3<float> Pos = Vec3<float>();
	float yaw, pitch = 0;
	std::string Context, PlayerName = "";

	MumbleLink(): Module("Mumble Link", "Use proximity chat in Flarial with the help of Mumble",
		IDR_MUMBLE_PNG, "") {

	}

	void onEnable() override;;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	 void SendDataToServer();
};