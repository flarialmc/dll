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

#pragma comment(lib, "Ws2_32.lib")



class MumbleLink : public Module {
public:
	Vec3<float> Pos = Vec3<float>();
	float yaw, pitch = 0;
	std::string Context, PlayerName = "";
	MumbleLink();

	void onEnable() override;;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	 void SendDataToServer();
};