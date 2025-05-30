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

	void onSetup() override {
		Listen(this, MouseEvent, &CPSCounter::onMouse)
	}

	void onEnable() override {
		Listen(this, RenderEvent, &CPSCounter::onRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &CPSCounter::onRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"CPS: {value}");
		if (settings.getSettingByName<bool>("rightcps") == nullptr) settings.addSetting("rightcps", false);
		Module::defaultConfig("all");
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		this->addHeader("CPS Counter");
		this->defaultAddSettings("main");
		this->addToggle("Right Click CPS", "", this->settings.getSettingByName<bool>("rightcps")->value);
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

	static inline double lastLeftAllowed = 0.0;
	static inline double lastRightAllowed = 0.0;

	double getCurrentTime() {
		using namespace std::chrono;
		return duration<double>(high_resolution_clock::now().time_since_epoch()).count();
	}

	void onMouse(MouseEvent& event) {
		auto limiter = ModuleManager::getModule("CPS Limiter");
		if (limiter == nullptr) return;

		double now = getCurrentTime();

		if (event.getButton() == MouseButton::Left) {
			if (!MC::held) {
				leftClickHeld = false;
			}
			else {
				leftClickHeld = true;

				if (limiter->settings.getSettingByName<bool>("enabled")->value) {
					float leftCpsLimit = limiter->settings.getSettingByName<float>("Left")->value;
					double leftInterval = 1.0 / leftCpsLimit;

					if ((now - lastLeftAllowed) < leftInterval) {
						event.cancel();
						return;
					}
					lastLeftAllowed = now;
				}
				AddLeftClick();
			}
		}

		if (event.getButton() == MouseButton::Right) {
			if (!MC::held) {
				rightClickHeld = false;
			}
			else {
				rightClickHeld = true;

				if (limiter->settings.getSettingByName<bool>("enabled")->value) {
					float rightCpsLimit = limiter->settings.getSettingByName<float>("Right")->value;
					double rightInterval = 1.0 / rightCpsLimit;

					if ((now - lastRightAllowed) < rightInterval) {
						event.cancel();
						return;
					}
					lastRightAllowed = now;
				}
				AddRightClick();
			}
		}
	}
	//
	void onRender(RenderEvent& event) {
		if (this->isEnabled()) {
			if (!this->settings.getSettingByName<bool>("rightcps")->value) {
				std::string leftCPS = FlarialGUI::cached_to_string(GetLeftCPS());
				this->normalRender(1, leftCPS);
			}
			else {
				std::string leftAndRightCPS = FlarialGUI::cached_to_string(GetLeftCPS()) + " | " + FlarialGUI::cached_to_string(GetRightCPS());
				this->normalRender(1, leftAndRightCPS);
			}

		}
	}

	static void AddLeftClick() {
		ClickData click{};
		click.timestamp = Microtime();
		leftClickList.insert(leftClickList.begin(), click);

		if (leftClickList.size() >= 100) {
			leftClickList.pop_back();
		}
	}

	static void AddRightClick() {
		ClickData click{};
		click.timestamp = Microtime();
		rightClickList.insert(rightClickList.begin(), click);

		if (rightClickList.size() >= 100) {
			rightClickList.pop_back();
		}
	}

	[[nodiscard]] static int GetLeftCPS() {
		if (leftClickList.empty()) {
			return 0;
		}

		double currentMicros = Microtime();
		auto count = std::count_if(leftClickList.begin(), leftClickList.end(), [currentMicros](const ClickData& click) {
			return (currentMicros - click.timestamp <= 1.0);
			});

		return (int)std::round(count);
	}

	[[nodiscard]] static int GetRightCPS() {
		if (rightClickList.empty()) {
			return 0;
		}

		double currentMicros = Microtime();
		auto count = std::count_if(rightClickList.begin(), rightClickList.end(),
			[currentMicros](const ClickData& click) {
				return (currentMicros - click.timestamp <= 1.0);
			});

		return (int)std::round(count);
	}

	[[nodiscard]] static bool GetLeftHeld() {
		return leftClickHeld;
	}

	[[nodiscard]] static bool GetRightHeld() {
		return rightClickHeld;
	}

private:
	[[nodiscard]] static double Microtime() {
		return (double(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
	}
};