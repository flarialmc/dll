#pragma once

#include "../Module.hpp"
#include "Events/EventManager.hpp"

class ClickData {
public:
	double timestamp;  // Milliseconds since some reference point
	// ... other click data members ...
};


class RateLimiter {
public:
	RateLimiter(float cps)
	  : tokens(0.0), maxTokens(1.0), rate(cps),
		last(std::chrono::steady_clock::now()) {}

	bool allow() {
		using Clock = std::chrono::steady_clock;
		auto now = Clock::now();
		std::chrono::duration<double> dt = now - last;
		last = now;

		tokens += rate * dt.count();
		if (tokens > maxTokens) tokens = maxTokens;

		if (tokens >= 1.0) {
			tokens -= 1.0;
			return true;
		}
		return false;
	}

	void setRate(float cps) {
		rate = cps;
	}

private:
	double tokens;
	const double maxTokens;
	double rate;
	std::chrono::steady_clock::time_point last;
};

class CPSCounter : public Module {
private:
	static inline std::vector<ClickData> leftClickList;
	static inline std::vector<ClickData> rightClickList;
	static inline bool rightClickHeld;
	static inline bool leftClickHeld;
	static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastRightClick;
	static inline std::chrono::time_point<std::chrono::high_resolution_clock> lastLeftClick;
	RateLimiter leftLimiter = {0.0f};
	RateLimiter rightLimiter = {0.0f};
public:
	CPSCounter() : Module("CPS", "Counts your Clicks per second.", IDR_CURSOR_PNG, "") {
		
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

private:
	[[nodiscard]] static double Microtime();
};