#pragma once

#include "../Module.hpp"

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
	Time() : Module("Clock", "Displays your current local time.", IDR_TIME_PNG, "") {

		Module::setup();

	};

	void onEnable() override {
		Listen(this, RenderEvent, &Time::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &Time::onRender)
			Module::onDisable();
	}

	void onRender(RenderEvent& event) {
		const auto now = std::time(nullptr);
		const std::tm calendarTime = localtime_xp(now);

		std::string meridiem;
		std::string seperator;

		int hour = calendarTime.tm_hour;
		int minute = calendarTime.tm_min;

		if (hour - 12 < 0) {
			meridiem = "AM";
		}
		else if (hour == 0) {
			hour = 12;
			meridiem = "AM";
		}
		else if (hour == 12) {
			hour = 12;
			meridiem = "PM";
		}
		else {
			meridiem = "PM";
			hour -= 12;
		}
		if (this->settings.getSettingByName<bool>("24")->value && meridiem == "PM") {
			hour += 12;
			meridiem = "";
		}
		else if (this->settings.getSettingByName<bool>("24")->value && meridiem == "AM") meridiem = "";

		seperator = minute < 10 ? ":0" : ":";

		if (hour == 24) hour = 0;

		std::string time = FlarialGUI::cached_to_string(hour) + seperator + FlarialGUI::cached_to_string(minute) + " " + meridiem;

		this->normalRender(3, time);
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<bool>("24") == nullptr) settings.addSetting("24", false);
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value}");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		this->addHeader("Main");
		this->defaultAddSettings("main");
		this->extraPadding();

		this->addHeader("Text");
		this->defaultAddSettings("text");
		this->extraPadding();

		this->addHeader("Colors");
		this->defaultAddSettings("colors");
		this->extraPadding();

		this->addHeader("Misc");
		this->defaultAddSettings("misc");

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}
};

