#pragma once

#include "../Module.hpp"

class ClickData {
public:
	double timestamp;  // Milliseconds since some reference point
	// ... other click data members ...
};

class CPSCounter : public Module {
private:
	static inline std::vector<ClickData> leftClickList;
	static inline std::vector<ClickData> rightClickList;
	static inline bool rightClickHeld;
	static inline bool leftClickHeld;
	static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastRightClick;
	static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastLeftClick;
public:
	CPSCounter() : Module("CPS", "Counts your Clicks per second.", IDR_CURSOR_PNG, "") {
		Module::setup();
	};

	void onSetup() override;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	static inline double lastLeftAllowed = 0.0;
	static inline double lastRightAllowed = 0.0;

	double getCurrentTime();

	void onMouse(MouseEvent& event);

	void onRender(RenderEvent& event);

	static void AddLeftClick();

	static void AddRightClick();

	[[nodiscard]] static int GetLeftCPS();

	[[nodiscard]] static int GetRightCPS();

	[[nodiscard]] static bool GetLeftHeld();

	[[nodiscard]] static bool GetRightHeld();

private:
	[[nodiscard]] static double Microtime();
};