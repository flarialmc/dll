#pragma once

#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../Client.hpp"
#include "../../Utils/VersionUtils.hpp"
#include <windows.h>
#include <unknwn.h>
#include <chrono>


class TickData {
public:
	double timestamp;
};

class JavaDebugMenu : public Module {

private:
	std::string lookingAt = "minecraft:empty";
	Vec3<float> PrevPos{};
	std::string speed = "0";
	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
	float lerpYaw = 0.0f;
	float lerpPitch = 0.0f;
	static inline std::vector<TickData> tickList;
	std::string versionName;
	std::string cpuName;

	static double Microtime() {
		return (double(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
	}

public:

	JavaDebugMenu() : Module("Java Debug Menu", "Displays Java-style debug information.\nSimilar to F3 menu in Minecraft Java Edition.",
		IDR_F3_PNG, "F3") {
		Module::setup();
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	static int GetTicks();

	std::string getFacingDirection(LocalPlayer* player);

	std::string getCPU();

	std::string getDimensionName();

	std::string getTime();

	std::string getFormattedTime(long long seconds);

	void onTick(TickEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);

	void onRender(RenderEvent& event);

	void onKey(KeyEvent& event);

	void onHudCursorRendererRender(HudCursorRendererRenderEvent& event);
};