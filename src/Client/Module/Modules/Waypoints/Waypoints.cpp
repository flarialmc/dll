#include "Waypoints.hpp"
#include "Client.hpp"
#include "Manager.hpp"

#include <Utils/Render/DrawUtil3D.hpp>
#include "Utils/Render/MaterialUtils.hpp"

#include <numbers>
#include <random>


void Waypoints::onEnable() {
	Listen(this, RenderEvent, &Waypoints::onRender)
	Listen(this, KeyEvent, &Waypoints::onKey);
	Listen(this, Render3DEvent, &Waypoints::onRender3D)
	Module::onEnable();
}

void Waypoints::onDisable() {
	Deafen(this, RenderEvent, &Waypoints::onRender)
	Deafen(this, KeyEvent, &Waypoints::onKey);
	Deafen(this, Render3DEvent, &Waypoints::onRender3D)
	Module::onDisable();
}


float Waypoints::random() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, 100);
	return static_cast<float>(dis(gen)) / 100.f;
}


void Waypoints::addWaypoint(int index, std::string name, std::string color, Vec3<float> position, bool state, bool config, bool rgb, float opacity) {
	if (config) {
		std::string end = "-" + FlarialGUI::cached_to_string(index);
		setDef("waypoint" + end, (std::string)name);
		setDef("color" + end, (std::string)color);
		setDef("x" + end, position.x);
		setDef("y" + end, position.y);
		setDef("z" + end, position.z);
		setDef("state" + end, (bool)state);
		setDef("rgb" + end, (bool)rgb);
		setDef("opacity" + end, (float)opacity);
		setDef("world" + end, (std::string)SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName());
		setDef("dimension" + end, (std::string)SDK::clientInstance->getBlockSource()->getDimension()->getName());
		setDef("showInnerBeam" + end, true);
		setDef("showOuterBeam" + end, true);
		this->settings.setValue("total", getOps<float>("total") + 1);
		Client::SaveSettings();

		Waypoint wp(position, false, 100.0f, index, state);
		WaypointList[name] = wp;
	}
	else {
		Waypoint wp(position, false, 100.0f, index, state);
		WaypointList[name] = wp;
	}
}

Vec3<float> Waypoints::getPos(int index) {
	return Vec3{ getOps<float>("x-" + FlarialGUI::cached_to_string(index)), getOps<float>("y-" + FlarialGUI::cached_to_string(index)), getOps<float>("z-" + FlarialGUI::cached_to_string(index)) };
}


void Waypoints::onSetup() {
	keybindActions.clear();
	keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
		std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
		if (duration.count() >= 0.1) {
			KeyEvent event = std::any_cast<KeyEvent>(args[0]);
			int index = WaypointList.size();
			std::string col = "FFFFFF";
			if (settings.getSettingByName<bool>("randomizeColor") != nullptr) {
				if (getOps<bool>("randomizeColor")) col = FlarialGUI::ColorFToHex(D2D1_COLOR_F(random(), random(), random(), 1.f));
			}
			addWaypoint(
				index,
				"waypoint-" + FlarialGUI::cached_to_string(index),
				col,
				Vec3{ SDK::clientInstance->getLocalPlayer()->getPosition()->x, SDK::clientInstance->getLocalPlayer()->getPosition()->y - 1, SDK::clientInstance->getLocalPlayer()->getPosition()->z },
				true,
				true,
				false,
				100.0f
			);
			FlarialGUI::Notify("Added waypoint!");
			last_used = std::chrono::high_resolution_clock::now();
		}
		return {};
		});

	if (getOps<float>("total") > 0) {
		for (int i = 0; i < getOps<float>("total"); i++) {
			std::string end = "-" + FlarialGUI::cached_to_string(i);
			if (!this->settings.getSettingByName<std::string>("waypoint" + end)) continue;
			addWaypoint(
				i,
				getOps<std::string>("waypoint" + end),
				getOps<std::string>("color" + end),
				Vec3 {
					getOps<float>("x" + end),
					getOps<float>("y" + end),
					getOps<float>("z" + end)
				},
				getOps<bool>("state" + end),
				false,
				getOps<bool>("rgb" + end),
				getOps<float>("opacity" + end)
			);
		}
	}

}


void Waypoints::defaultConfig() {
	getKeybind();
	Module::defaultConfig("core");

	setDef("distance", 1000.0f);
	setDef("total", 0.0f);

	setDef("bgcolor", (std::string)"000000");
	setDef("bgopacity", 0.25f);
	setDef("bgrounding", 10.0f);
	setDef("bgrgb", false);

	setDef("bordercolor", (std::string)"000000");
	setDef("borderthickness", 2.5f);
	setDef("borderrgb", false);
	setDef("borderopacity", 1.0f);

	setDef("textcolor", (std::string)"000000");
	setDef("textopacity", 1.0f);
	setDef("textrgb", false);

	setDef("textuse", true);
	setDef("bguse", true);
	setDef("borderuse", true);

	setDef("border", true);

	setDef("showmeters", true);

	setDef("innerBeamRadius", 0.25f);
	setDef("outerBeamRadius", 0.33f);
	setDef("sides", 16);

	setDef("randomizeColor", true);
}


void Waypoints::settingsRender(float settingsOffset) {
	initSettingsPage();

	addHeader("Waypoints");
	addButton("Add another Waypoint", "", "Add", [this] {

		int index = WaypointList.size();
		std::string indexStr = FlarialGUI::cached_to_string(index);
		while (true) {
			SettingType<std::string>* s = this->settings.getSettingByName<std::string>("waypoint-" + indexStr);
			if (s == nullptr) break;
			index++;
		}
		std::string col = "FFFFFF";
		if (settings.getSettingByName<bool>("randomizeColor") != nullptr) {
			if (getOps<bool>("randomizeColor")) col = FlarialGUI::ColorFToHex(D2D1_COLOR_F(random(), random(), random(), 1.f));
		}
		addWaypoint(
			index,
			"waypoint-" + indexStr,
			col,
			Vec3{ SDK::clientInstance->getLocalPlayer()->getPosition()->x, SDK::clientInstance->getLocalPlayer()->getPosition()->y - 1, SDK::clientInstance->getLocalPlayer()->getPosition()->z },
			true,
			true,
			false,
		100.0f
		);
		FlarialGUI::Notify("Added! Scroll down for options.");
	});

	addKeybind("Add waypoint keybind", "Hold for 2 seconds to set bind.", getKeybind());
	addSlider("Distance", "Change until which distance waypoints will be drawn.", "distance", 10000.f, 0.f, true);
	addToggle("Randomize Waypoint Color", "", "randomizeColor");
	extraPadding();

	addHeader("Background");
	addSlider("Rounding", "", "bgrounding");
	addToggle("Waypoint color", "Whether the Background should be the waypoint's color.", "bguse");
	addConditionalSlider(getOps<bool>("bguse"), "Opacity", "", "bgopacity", 1.0f, 0.0f);
	addConditionalColorPicker(!getOps<bool>("bguse"), "Color", "", getOps<std::string>("bgcolor"), getOps<float>("bgopacity"), getOps<bool>("bgrgb"));
	addToggle("Border", "Draw a Border?", "border");
	extraPadding();

	if (getOps<bool>("border")) {
		addHeader("Border");
		addSlider("Thickness", "Change the border thickness", "borderthickness", 15.0f);
		addToggle("Waypoint color", "Whether the Border should be the waypoint's color.", "borderuse");
		addConditionalSlider(getOps<bool>("borderuse"), "Opacity", "", "borderopacity", 1.0f, 0.0f);
		addConditionalColorPicker(!getOps<bool>("borderuse"), "Color", "", getOps<std::string>("bordercolor"), getOps<float>("borderopacity"), getOps<bool>("borderrgb"));
		extraPadding();
	}

	addHeader("Text");
	addToggle("Waypoint color", "Whether the Text should be the waypoint's color.", "textuse");
	addConditionalSlider(getOps<bool>("textuse"), "Opacity", "", "textopacity", 1.0f, 0.0f);
	addConditionalColorPicker(!getOps<bool>("textuse"), "Color", "", getOps<std::string>("textcolor"), getOps<float>("textopacity"), getOps<bool>("textrgb"));
	extraPadding();

	addHeader("Beams");
	addSliderInt("Sides of Beam", "", "sides", 32, 3);
	addSlider("Inner Beam Radius", "", "innerBeamRadius", 0.8f, 0.1f);
	addSlider("Outer Beam Radius", "", "outerBeamRadius", 1.f, 0.2f);
	extraPadding();

	for (auto pair : WaypointList) {
		std::string end = "-" + FlarialGUI::cached_to_string(pair.second.index);
		if (!this->settings.getSettingByName<std::string>("waypoint" + end)) continue;
		if (getOps<std::string>("world" + end) != SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName()) continue;
		addHeader(getOps<std::string>("waypoint" + end));
		addToggle("Enabled", "Change if the waypoint should be shown or not.", "state" + end);
		addColorPicker("Color", "Change the color of the waypoint.", getOps<std::string>("color" + end), getOps<float>("opacity" + end), getOps<bool>("rgb" + end));
		addTextBox("Name", "Change the name of the waypoint.", getOps<std::string>("waypoint" + end));

		if (settings.getSettingByName<bool>("showInnerBeam" + end) == nullptr) setDef("showInnerBeam" + end, true);
		if (settings.getSettingByName<bool>("showOuterBeam" + end) == nullptr) setDef("showOuterBeam" + end, true);
		addToggle("Show Inner Beam", "", "showInnerBeam" + end);
		addToggle("Show Outer Beam", "", "showOuterBeam" + end);

		addButton("Delete Waypoint", "", "Delete", [this, index = pair.second.index]() {
			std::string end = "-" + FlarialGUI::cached_to_string(index);
			this->settings.deleteSetting("waypoint" + end);
			this->settings.deleteSetting("color" + end);
			this->settings.deleteSetting("x" + end);
			this->settings.deleteSetting("y" + end);
			this->settings.deleteSetting("z" + end);
			this->settings.deleteSetting("state" + end);
			this->settings.deleteSetting("rgb" + end);
			this->settings.deleteSetting("opacity" + end);
			this->settings.deleteSetting("world" + end);
			this->settings.deleteSetting("dimension" + end);
			this->settings.deleteSetting("showInnerBeam" + end);
			this->settings.deleteSetting("showOuterBeam" + end);
			Client::SaveSettings();
		});
	}

	FlarialGUI::UnsetScrollView();

	resetPadding();
}


void Waypoints::onRender(RenderEvent& event) {
    if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
       !SDK::clientInstance->getLocalPlayer()->getLevel() || !SDK::clientInstance->getBlockSource()->getDimension())
       return;

    if (FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

    Vec2<float> screen;
    D2D1_COLOR_F invis = FlarialGUI::HexToColorF("000000");
    invis.a = 0.0F;
    for (auto pair : WaypointList) {
		if (!this->settings.getSettingByName<bool>("state-" + FlarialGUI::cached_to_string(pair.second.index))) continue; //check if its enabled
		if (getOps<std::string>("world-" + FlarialGUI::cached_to_string(pair.second.index)) != SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName()) continue;
		if (getOps<std::string>("dimension-" + FlarialGUI::cached_to_string(pair.second.index)) != SDK::clientInstance->getBlockSource()->getDimension()->getName()) continue;
		if (getOps<bool>("state-" + FlarialGUI::cached_to_string(pair.second.index)))
		{
			//get name
			std::string name = getOps<std::string>("waypoint-" + FlarialGUI::cached_to_string(pair.second.index));
			Waypoint waypoint = pair.second;

			//get pos
			Vec3<float> waypointPos = getPos(waypoint.index);

			//check if its on screen
			if (Matrix::WorldToScreen(waypointPos, screen)) {
				//get distance
				Vec3<float> origin = MC::Transform.origin;
				float distance = waypointPos.dist(origin);

				//create name/check if meters
				if (getOps<bool>("showmeters")) name += " (" + FlarialGUI::cached_to_string(int(round(distance))) + "m)";
				//wide
				std::wstring widename = std::wstring(name.begin(), name.end());

				//calculate font size
				float minFontSize = Constraints::RelativeConstraint(0.09f, "height", true);
				float maxFontSize = Constraints::RelativeConstraint(0.14f, "height", true);
				float fontSize = maxFontSize - (maxFontSize - minFontSize) * (distance / getOps<float>("distance"));
				fontSize = std::max(minFontSize, std::min(fontSize, maxFontSize));

				//check distance
				if (distance < getOps<float>("distance")) {
				//################ COLORS

				//get the bg color
					D2D1_COLOR_F rect;
					if (getOps<bool>("bguse")) {
						if (getOps<bool>("rgb-" + FlarialGUI::cached_to_string(pair.second.index))) rect = FlarialGUI::rgbColor;
						else rect = FlarialGUI::HexToColorF(getOps<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index)));

					}
					else {
						if (getOps<bool>("bgrgb")) rect = FlarialGUI::rgbColor;
						else rect = FlarialGUI::HexToColorF(getOps<std::string>("bgcolor"));
					}
					rect.a = getOps<float>("bgopacity");

					//get the border color
					D2D1_COLOR_F border;
					if (getOps<bool>("borderuse")) {
						if (getOps<bool>("rgb-" + FlarialGUI::cached_to_string(pair.second.index))) border = FlarialGUI::rgbColor;
						else border = FlarialGUI::HexToColorF(getOps<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index)));
					}
					else {
						if (getOps<bool>("borderrgb")) border = FlarialGUI::rgbColor;
						else border = FlarialGUI::HexToColorF(getOps<std::string>("bordercolor"));
					}
					border.a = getOps<float>("borderopacity");

					//get the text color
					D2D1_COLOR_F text;
					if (getOps<bool>("textuse")) {
						if (getOps<bool>("rgb-" + FlarialGUI::cached_to_string(pair.second.index))) text = FlarialGUI::rgbColor;
						else text = FlarialGUI::HexToColorF(getOps<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index)));
					}
					else {
						if (getOps<bool>("textrgb")) text = FlarialGUI::rgbColor;
						else text = FlarialGUI::HexToColorF(getOps<std::string>("textcolor"));
					}
					text.a = getOps<float>("textopacity");

					//################ DRAWING

					//draw invisible text
					std::string name = FlarialGUI::FlarialTextWithFont(screen.x, screen.y, widename.c_str(), fontSize, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize, DWRITE_FONT_WEIGHT_NORMAL, invis, true);

					//draw bg
					FlarialGUI::RoundedRect(
						screen.x - 5.0F,
						screen.y - 20.0F,
						rect,
						FlarialGUI::TextSizes[name] + 10.0F,
						40.0F,
						getOps<float>("bgrounding"),
						getOps<float>("bgrounding")
					);

					//draw border
					if (getOps<bool>("border"))
					{
						FlarialGUI::RoundedHollowRect(
							screen.x - 5.0F,
							screen.y - 20.0F,
							getOps<float>("borderthickness"),
							border,
							FlarialGUI::TextSizes[name] + 10.0F,
							40.0F,
							getOps<float>("bgrounding"),
							getOps<float>("bgrounding")
						);
					}
					//draw actual text
					FlarialGUI::FlarialTextWithFont(screen.x, screen.y, widename.c_str(), fontSize, 0, DWRITE_TEXT_ALIGNMENT_LEADING, fontSize, DWRITE_FONT_WEIGHT_NORMAL, text, true);
				}
			}
		}
    }
}


void Waypoints::onKey(KeyEvent& event) {

	if (this->isEnabled() && (
		SDK::clientInstance->getScreenName() == "hud_screen" ||
		SDK::clientInstance->getScreenName() == "f3_screen" ||
		SDK::clientInstance->getScreenName() == "zoom_screen"
	)) {
		for (int i = 0; i <= totalKeybinds - 1; ++i) {
			if (isKeybind(event.keys) && isKeyPartOfKeybind(event.key))
				keybindActions[i]({ std::any(event) });
		}
	}
}


void Waypoints::onRender3D(Render3DEvent& event) {
	if (ModuleManager::getModule("ClickGUI")->active) return;

	for (auto pair : WaypointList) {
		std::string end = "-" + FlarialGUI::cached_to_string(pair.second.index);

		if (!this->settings.getSettingByName<bool>("state" + end)) continue; //check if its enabled
		if (getOps<std::string>("world" + end) != SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName()) continue;
		if (getOps<std::string>("dimension" + end) != SDK::clientInstance->getBlockSource()->getDimension()->getName()) continue;
		if (getOps<bool>("state" + end))
		{

			Waypoint waypoint = pair.second;

			Vec3<float> waypointPos = getPos(waypoint.index);
			D2D1_COLOR_F waypointCol = FlarialGUI::HexToColorF(getOps<std::string>("color" + end));

			int numSides = getOps<int>("sides");

			if (settings.getSettingByName<bool>("showInnerBeam" + end) == nullptr) setDef("showInnerBeam" + end, true);
			if (settings.getSettingByName<bool>("showOuterBeam" + end) == nullptr) setDef("showOuterBeam" + end, true);

			if (getOps<bool>("showInnerBeam" + end)) {
				float innerRadius = getOps<float>("innerBeamRadius");

				mce::MaterialPtr *material1 = MaterialUtils::getSelectionOverlay();
				MCDrawUtil3D dc1(event.LevelRenderer, event.screenContext, material1);

				for (int i = 0; i < numSides; ++i) {
					int j = (i + 1) % numSides;

					float angle_i = 2.0f * std::numbers::pi * i / numSides;
					float angle_j = 2.0f * std::numbers::pi * j / numSides;

					dc1.fillQuad(
						Vec3<float>{waypointPos.x + innerRadius * cos(angle_i), -64.f, waypointPos.z + innerRadius * sin(angle_i)},
						Vec3<float>{waypointPos.x + innerRadius * cos(angle_i), 320.f, waypointPos.z + innerRadius * sin(angle_i)},
						Vec3<float>{waypointPos.x + innerRadius * cos(angle_j), 320.f, waypointPos.z + innerRadius * sin(angle_j)},
						Vec3<float>{waypointPos.x + innerRadius * cos(angle_j), -64.f, waypointPos.z + innerRadius * sin(angle_j)},
						waypointCol
					);
				}

				dc1.flush();
			}

			if (getOps<bool>("showOuterBeam" + end)) {
				float outerRadius = getOps<float>("outerBeamRadius");

				mce::MaterialPtr *material2 = MaterialUtils::getNametag();
				MCDrawUtil3D dc2(event.LevelRenderer, event.screenContext, material2);

				for (int i = 0; i < numSides; ++i) {
					int j = (i + 1) % numSides;

					float angle_i = 2.0f * std::numbers::pi * i / numSides;
					float angle_j = 2.0f * std::numbers::pi * j / numSides;

					dc2.fillQuad(
						Vec3<float>{waypointPos.x + outerRadius * cos(angle_i), -64.f, waypointPos.z + outerRadius * sin(angle_i)},
						Vec3<float>{waypointPos.x + outerRadius * cos(angle_i), 320.f, waypointPos.z + outerRadius * sin(angle_i)},
						Vec3<float>{waypointPos.x + outerRadius * cos(angle_j), 320.f, waypointPos.z + outerRadius * sin(angle_j)},
						Vec3<float>{waypointPos.x + outerRadius * cos(angle_j), -64.f, waypointPos.z + outerRadius * sin(angle_j)},
						D2D1_COLOR_F(1.f, 1.f, 1.f, 0.2f)
					);

				}

				dc2.flush();
			}

		}
	}
}
