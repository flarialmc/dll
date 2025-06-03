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
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &Waypoints::onRender)
			Listen(this, KeyEvent, &Waypoints::onKey);
		Module::onEnable();
	}

	void addWaypoint(int index, std::string name, std::string color, Vec3<float> position, bool state, bool config, bool rgb, float opacity) {
		if (config)
		{
			std::string end = "-" + FlarialGUI::cached_to_string(index);
			this->settings.addSetting("waypoint" + end, (std::string)name);
			this->settings.addSetting("color" + end, (std::string)color);
			this->settings.addSetting("x" + end, position.x);
			this->settings.addSetting("y" + end, position.y);
			this->settings.addSetting("z" + end, position.z);
			this->settings.addSetting("state" + end, (bool)state);
			this->settings.addSetting("rgb" + end, (bool)rgb);
			this->settings.addSetting("opacity" + end, (float)opacity);
			this->settings.addSetting("world" + end, (std::string)SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName());
			this->settings.addSetting("dimension" + end, (std::string)SDK::clientInstance->getBlockSource()->getDimension()->getName());
			this->settings.setValue("total", getOps<float>("total") + 1);
			this->saveSettings();

			Waypoint wp(position, false, 100.0f, index, state);
			WaypointList[name] = wp;
		}
		else {
			Waypoint wp(position, false, 100.0f, index, state);
			WaypointList[name] = wp;
		}
	}

	Vec3<float> getPos(int index) {
		return Vec3{ this->settings.getSettingByName<float>("x-" + FlarialGUI::cached_to_string(index))->value, this->settings.getSettingByName<float>("y-" + FlarialGUI::cached_to_string(index))->value, this->settings.getSettingByName<float>("z-" + FlarialGUI::cached_to_string(index))->value };
	}

	void onSetup() override {
		keybindActions.clear();
		keybindActions.push_back([this](std::vector<std::any> args) -> std::any {
			std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_used;
			if (duration.count() >= 0.1) {
				KeyEvent event = std::any_cast<KeyEvent>(args[0]);
				int index = WaypointList.size();
				addWaypoint(
					index,
					"waypoint-" + FlarialGUI::cached_to_string(index),
					"FFFFFF",
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

		if (getOps<float>("total") > 0)
			for (int i = 0; i < getOps<float>("total"); i++) {
				if (!this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(i))) continue;
				addWaypoint(
					i,
					this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(i))->value,
					this->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(i))->value,
					Vec3 {
					this->settings.getSettingByName<float>("x-" + FlarialGUI::cached_to_string(i))->value, this->settings.getSettingByName<float>("y-" + FlarialGUI::cached_to_string(i))->value, this->settings.getSettingByName<float>("z-" + FlarialGUI::cached_to_string(i))->value
				},
					this->settings.getSettingByName<bool>("state-" + FlarialGUI::cached_to_string(i))->value,
					false,
					this->settings.getSettingByName<bool>("rgb-" + FlarialGUI::cached_to_string(i))->value,
					this->settings.getSettingByName<float>("opacity-" + FlarialGUI::cached_to_string(i))->value
				);
			}
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &Waypoints::onRender)
			Deafen(this, KeyEvent, &Waypoints::onKey);
		Module::onDisable();
	}

	void defaultConfig() override {
		getKeybind();
		Module::defaultConfig("core");
		setDef("distance", 1000.0f);
		setDef("total", 0.0f);
		setDef("bg", (std::string)"000000", 0.25f, false);
		setDef("border", (std::string)"000000", 1.f, false);
		setDef("bgrounding", 10.0f);
		setDef("borderthickness", 2.5f);
		setDef("text", (std::string)"000000", 1.f, false);
		setDef("textuse", true);
		setDef("bguse", true);
		setDef("borderuse", true);
		setDef("border", true);
		setDef("showmeters", true);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, 5000, 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Waypoints");
		addButton("Add another Waypoint", "", "Add", [this] {

			int index = WaypointList.size();
			addWaypoint(
				index,
				"waypoint-" + FlarialGUI::cached_to_string(index),
				"FFFFFF",
				Vec3{ SDK::clientInstance->getLocalPlayer()->getPosition()->x, SDK::clientInstance->getLocalPlayer()->getPosition()->y - 1, SDK::clientInstance->getLocalPlayer()->getPosition()->z },
				true,
				true,
				false,
				100.0f
			);
			FlarialGUI::Notify("Added! Scroll down for options.");
			});
		addKeybind("Add waypoint keybind", "Hold for 2 seconds to set bind.", getKeybind());
		addSlider("Distance", "Change until which distance waypoints will be drawn.", getOps<float>("distance"), 10000.f, 0.f, true);
		extraPadding();

		addHeader("Background");
		addSlider("Rounding", "", getOps<float>("bgrounding"));
		addToggle("Waypoint color", "Whether the Background should be the waypoint's color.", getOps<bool>("bguse"));
		addConditionalColorPicker(!getOps<bool>("bguse"), "Color", "", "bg");
		addToggle("Border", "Draw a Border?", getOps<bool>("border"));
		extraPadding();

		if (getOps<bool>("border")) {
			addHeader("Border");
			addSlider("Thickness", "Change the border thickness", getOps<float>("borderthickness"), 15.0f);
			addToggle("Waypoint color", "Whether the Border should be the waypoint's color.", getOps<bool>("borderuse"));
			addConditionalColorPicker(!getOps<bool>("borderuse"), "Color", "", "border");
			extraPadding();
		}

		addHeader("Text");
		addToggle("Waypoint color", "Whether the Text should be the waypoint's color.", getOps<bool>("textuse"));
		addConditionalColorPicker(!getOps<bool>("textuse"), "Color", "", "text");

		for (auto pair : WaypointList) {
			if (!this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(pair.second.index))) continue;
			if (this->settings.getSettingByName<std::string>("world-" + FlarialGUI::cached_to_string(pair.second.index))->value != SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName()) continue;
			this->addHeader(this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(pair.second.index))->value);
			this->addToggle("Enabled", "Change if the waypoint should be shown or not.", this->settings.getSettingByName<bool>("state-" + FlarialGUI::cached_to_string(pair.second.index))->value);
			this->addColorPicker("Color", "Change the color of the waypoint.", "waypoint" + FlarialGUI::cached_to_string(pair.second.index));
			this->addTextBox("Name", "Change the name of the waypoint.", this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(pair.second.index))->value);
			this->addButton("Delete Waypoint", "", "Delete", [this, index = pair.second.index]() {
				std::string end = "-" + FlarialGUI::cached_to_string(index);
				this->settings.deleteSetting("waypoint" + end);
				this->settings.deleteSetting("color" + end);
				this->settings.deleteSetting("x" + end);
				this->settings.deleteSetting("y" + end);
				this->settings.deleteSetting("z" + end);
				this->settings.deleteSetting("state" + end);
				this->settings.deleteSetting("rgb" + end);
				this->settings.deleteSetting("opacity" + end);
				this->saveSettings();
				});
		}

		FlarialGUI::UnsetScrollView();

		this->resetPadding();
	}

	void onRender(RenderEvent& event) {
		if (!SDK::clientInstance || !SDK::clientInstance->getLocalPlayer() || SDK::getCurrentScreen() != "hud_screen" ||
			!SDK::clientInstance->getLocalPlayer()->getLevel() || !SDK::clientInstance->getBlockSource()->getDimension())
			return;

		if (FlarialGUI::inMenu || SDK::getCurrentScreen() != "hud_screen") return;

		Vec2<float> screen;
		D2D1_COLOR_F invis = FlarialGUI::HexToColorF("000000");
		invis.a = 0.0F;
		for (auto pair : WaypointList) {
			if (!this->settings.getSettingByName<bool>("state-" + FlarialGUI::cached_to_string(pair.second.index))) continue; //check if its enabled
			if (this->settings.getSettingByName<std::string>("world-" + FlarialGUI::cached_to_string(pair.second.index))->value != SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName()) continue;
			if (this->settings.getSettingByName<std::string>("dimension-" + FlarialGUI::cached_to_string(pair.second.index))->value != SDK::clientInstance->getBlockSource()->getDimension()->getName()) continue;
			if (this->settings.getSettingByName<bool>("state-" + FlarialGUI::cached_to_string(pair.second.index))->value)
			{
				//get name
				std::string name = this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(pair.second.index))->value;
				Waypoint waypoint = pair.second;

				//get pos
				Vec3<float> waypointPos = getPos(waypoint.index);

				//check if its on screen
				if (Matrix::WorldToScreen(waypointPos, screen)) {
					//get distance
					Vec3<float> origin = MC::Transform.origin;
					float distance = waypointPos.dist(origin);

					//create name/check if meters
					if (this->settings.getSettingByName<bool>("showmeters"))
					{
						name += " (" + FlarialGUI::cached_to_string(int(round(distance))) + "m)";
					}
					//wide
					std::wstring widename = std::wstring(name.begin(), name.end());

					//calculate font size
					float minFontSize = Constraints::RelativeConstraint(0.09f, "height", true);
					float maxFontSize = Constraints::RelativeConstraint(0.14f, "height", true);
					float fontSize = maxFontSize - (maxFontSize - minFontSize) * (distance / getOps<float>("distance"));
					fontSize = std::max(minFontSize, std::min(fontSize, maxFontSize));

					//check distance
					if (distance < getOps<float>("distance"))
					{
						//################ COLORS

						//get the bg color
						D2D1_COLOR_F rect;
						if (getOps<bool>("bguse")) rect = getColor("waypoint" + FlarialGUI::cached_to_string(pair.second.index));
						else rect = getColor("bg");

						//get the border color
						D2D1_COLOR_F border;
						if (getOps<bool>("borderuse")) border = getColor("waypoint" + FlarialGUI::cached_to_string(pair.second.index));
						else border = getColor("border");

						//get the text color
						D2D1_COLOR_F text;
						if (getOps<bool>("textuse")) text = getColor("waypoint" + FlarialGUI::cached_to_string(pair.second.index));
						else text = getColor("text");
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
	//get waypoint color D2D1_COLOR_F color = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index))->value);

	void onKey(KeyEvent& event) {

		if (this->isEnabled()) {
			for (int i = 0; i <= totalKeybinds - 1; ++i) {
				if (isKeybind(event.keys) && isKeyPartOfKeybind(event.key))
					keybindActions[i]({ std::any(event) });
			}
		}
	}

};