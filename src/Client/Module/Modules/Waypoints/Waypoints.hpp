#pragma once

#include "../Module.hpp"
#include "../../../../Client/GUI/Engine/Engine.hpp"

struct Waypoint {
	Vec3<float> position;
	bool rgb;
	float opacity;
	int index;
	bool state;
};

class Waypoints : public Module {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> last_used;
	std::unordered_map<std::string, Waypoint> WaypointList;
public:
	Waypoints() : Module("Waypoints", "Allows you to mark points in your world.", IDR_WAYPOINTS_PNG, "") {
		//Module::setup();
	};

	void onEnable() override;

	void addWaypoint(int index, std::string name, std::string color, Vec3<float> position, bool state, bool config, bool rgb, float opacity);

	Vec3<float> getPos(int index);

	void onSetup() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onRender(RenderEvent& event);
	//get waypoint color D2D1_COLOR_F color = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index))->value);

	void onKey(KeyEvent& event);

};