#pragma once

#include "../HUDModule.hpp"
#include <Events/Game/TimeEvent.hpp>

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

class Time : public HUDModule {

public:

	Time(): HUDModule(3, "Clock", "Displays your current local or ingame time.",
		IDR_TIME_PNG, "", {"time"}) {
	}

	static inline float curTime;

	void onSetup() override;

	void onTimeEvent(TimeEvent& event);

	[[nodiscard]] static std::string formatMCTime(float time, bool military);

protected:
	std::string getDisplayValue() override;

	void customConfig() override;

	void customSettings() override;
};