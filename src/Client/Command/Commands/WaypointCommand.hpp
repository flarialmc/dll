#pragma once

#include <Command/Command.hpp>

class WaypointCommand : public Command {
public:
	WaypointCommand() : Command("waypoints", "Adds a waypoint.", {"waypoint", "wp"}) {}

	static bool checkValid(const std::string& s);
	void execute(const std::vector<std::string>& args) override;
};