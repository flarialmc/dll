#pragma once

#include "../Module.hpp"
#include <chrono>
#include <vector>
#include <deque>


class TimedObj {
public:
	double timestamp;
	int value;
};

class JavaDebugMenu : public Module {

private:
	std::string lookingAt = "minecraft:empty";
	std::string lastLookingAt = "";
	std::vector<std::string> lookingAtTags = {};

	Vec3<float> PrevPos{};
	float xVelo = 0.f;
	float yVelo = 0.f;
	float zVelo = 0.f;

	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

	float lerpYaw = 0.0f;
	float lerpPitch = 0.0f;

	static inline std::vector<TimedObj> tickList;

	std::string versionName;
	std::string cpuName;
	Perspective curPerspective;
	float lastBreakProgress = 0.0f;
	float currentBreakProgress = 0.0f;
	float cached1PercLow = 0.f;
	static inline auto last1PercLowUpdate = std::chrono::steady_clock::now();

	std::deque<float> prevFrameTimes;

	static double Microtime() {
		return (double(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
	}

public:

	JavaDebugMenu() : Module("Java Debug Menu", "Displays Java-style debug information.\nSimilar to F3 menu in Minecraft Java Edition.",
		IDR_F3_PNG, "F3") {
			
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void customToggle(std::string text, std::string subtext, std::string settingName);

	void customConditionalToggle(bool condition, std::string text, std::string subtext, std::string settingName);

	void customConditionalSlider(bool condition, std::string text, std::string subtext, std::string settingName, float maxVal = 100.0f, float minVal = 0.0f, bool zerosafe = true);

	void settingsRender(float settingsOffset) override;

	bool isOn(std::string settingName);

	void updateTimedVector(std::vector<TimedObj>& vec, float diff);

	static int GetTicks();

	void getOnePercLows();

	std::string getFacingDirection(LocalPlayer* player);

	std::string getCPU();

	std::string getDimensionName();

	std::string getTime();

	std::string getFormattedTime(long long seconds);

	void onTick(TickEvent& event);

	void onSetupAndRender(SetupAndRenderEvent& event);

	void onRender(RenderEvent& event);

	void onKey(KeyEvent& event);

	void onMouse(MouseEvent& event);

	void onHudCursorRendererRender(HudCursorRendererRenderEvent& event);

	void onSetTopScreenName(SetTopScreenNameEvent& event);

	void onGetViewPerspective(PerspectiveEvent& event);

	void drawVector(ImDrawList* drawList, ImVec2 center, ImVec2 endPos, ImU32 col, float lineWidth, float lineLength, float guiscale);
};