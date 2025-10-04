#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include <Events/Game/TimeEvent.hpp>
#include "Assets/Assets.hpp"
#include "Events/Events.hpp"

inline std::tm localtime_xp(std::time_t timer) {
	std::tm bt{};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *std::localtime(&timer);
#endif
	return bt;
}

class Time : public Module {

public:

	Time(): Module("Clock", "Displays your current local or ingame time.",
		IDR_TIME_PNG, "", false, {"time"}) {
	}

	static inline float curTime;

	void onEnable() override;

	void onSetup() override;

	void onDisable() override;

	void onRender(RenderEvent& event);

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onTimeEvent(TimeEvent& event);

	[[nodiscard]] static std::string formatMCTime(float time, bool military);
};