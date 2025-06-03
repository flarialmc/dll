#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"

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
	Time();;

	void onEnable() override;

	void onDisable() override;

	void onRender(RenderEvent& event);

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;
};

