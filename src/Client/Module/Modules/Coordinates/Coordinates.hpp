#pragma once


#include "../Module.hpp"

#include "../../../../Assets/Assets.hpp"
#include "Events/Events.hpp"

typedef std::unordered_map<std::string, std::string> StringMap;

class Coordinates : public Module {

public:

	Coordinates() : Module("Coordinates", "Shows your XYZ position in game.",
		IDR_COORDINATES_PNG, "", false, {"position"}) {
		
	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	StringMap getCoords(float multiplier);

	std::string formattedDimension(std::string dimension);

	std::string sigma(float val);

	void onRender(RenderEvent& event);

};
