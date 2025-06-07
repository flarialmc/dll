#include "Waypoints.hpp"

void Waypoints::onEnable() {
	Listen(this, RenderEvent, &Waypoints::onRender)
		Listen(this, KeyEvent, &Waypoints::onKey);
	Module::onEnable();
}

void Waypoints::addWaypoint(int index, std::string name, std::string color, Vec3<float> position, bool state, bool config, bool rgb, float opacity) {
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
		this->settings.setValue("total", this->settings.getSettingByName<float>("total")->value + 1);
		this->saveSettings();

		Waypoint wp(position, false, 100.0f, index, state);
		WaypointList[name] = wp;
	}
	else {
		Waypoint wp(position, false, 100.0f, index, state);
		WaypointList[name] = wp;
	}
}

Vec3<float> Waypoints::getPos(int index) {
	return Vec3{ this->settings.getSettingByName<float>("x-" + FlarialGUI::cached_to_string(index))->value, this->settings.getSettingByName<float>("y-" + FlarialGUI::cached_to_string(index))->value, this->settings.getSettingByName<float>("z-" + FlarialGUI::cached_to_string(index))->value };
}

void Waypoints::onSetup() {
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

	if (this->settings.getSettingByName<float>("total")->value > 0)
		for (int i = 0; i < this->settings.getSettingByName<float>("total")->value; i++) {
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

void Waypoints::onDisable() {
	Deafen(this, RenderEvent, &Waypoints::onRender)
		Deafen(this, KeyEvent, &Waypoints::onKey);
	Module::onDisable();
}

void Waypoints::defaultConfig() {
	getKeybind();
	Module::defaultConfig("core");
	if (settings.getSettingByName<float>("distance") == nullptr) settings.addSetting("distance", 1000.0f);
	if (settings.getSettingByName<float>("total") == nullptr) settings.addSetting("total", 0.0f);

	if (settings.getSettingByName<std::string>("bgcolor") == nullptr) settings.addSetting("bgcolor", (std::string)"000000");
	if (settings.getSettingByName<float>("bgopacity") == nullptr) settings.addSetting("bgopacity", 0.25f);
	if (settings.getSettingByName<float>("bgrounding") == nullptr) settings.addSetting("bgrounding", 10.0f);
	if (settings.getSettingByName<bool>("bgrgb") == nullptr) settings.addSetting("bgrgb", false);

	if (settings.getSettingByName<std::string>("bordercolor") == nullptr) settings.addSetting("bordercolor", (std::string)"000000");
	if (settings.getSettingByName<float>("borderthickness") == nullptr) settings.addSetting("borderthickness", 2.5f);
	if (settings.getSettingByName<bool>("borderrgb") == nullptr) settings.addSetting("borderrgb", false);
	if (settings.getSettingByName<float>("borderopacity") == nullptr) settings.addSetting("borderopacity", 1.0f);

	if (settings.getSettingByName<std::string>("textcolor") == nullptr) settings.addSetting("textcolor", (std::string)"000000");
	if (settings.getSettingByName<float>("textopacity") == nullptr) settings.addSetting("textopacity", 1.0f);
	if (settings.getSettingByName<bool>("textrgb") == nullptr) settings.addSetting("textrgb", false);

	if (settings.getSettingByName<bool>("textuse") == nullptr) settings.addSetting("textuse", true);
	if (settings.getSettingByName<bool>("bguse") == nullptr) settings.addSetting("bguse", true);
	if (settings.getSettingByName<bool>("borderuse") == nullptr) settings.addSetting("borderuse", true);

	if (settings.getSettingByName<bool>("border") == nullptr) settings.addSetting("border", true);

	if (settings.getSettingByName<bool>("showmeters") == nullptr) settings.addSetting("showmeters", true);
}

void Waypoints::settingsRender(float settingsOffset) {

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
		while (true) {
			SettingType<std::string>* s = this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(index));
			if (s == nullptr) break;
			index++;
		}
		Logger::debug(std::to_string(index));
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
	addSlider("Distance", "Change until which distance waypoints will be drawn.", settings.getSettingByName<float>("distance")->value, 10000.f, 0.f, true);
	extraPadding();

	addHeader("Background");
	addSlider("Rounding", "", settings.getSettingByName<float>("bgrounding")->value);
	addToggle("Waypoint color", "Whether the Background should be the waypoint's color.", settings.getSettingByName<bool>("bguse")->value);
	addConditionalSlider(settings.getSettingByName<bool>("bguse")->value, "Opacity", "", settings.getSettingByName<float>("bgopacity")->value, 1.0f, 0.0f);
	addConditionalColorPicker(!settings.getSettingByName<bool>("bguse")->value, "Color", "", settings.getSettingByName<std::string>("bgcolor")->value, settings.getSettingByName<float>("bgopacity")->value, settings.getSettingByName<bool>("bgrgb")->value);
	addToggle("Border", "Draw a Border?", settings.getSettingByName<bool>("border")->value);
	extraPadding();

	if (settings.getSettingByName<bool>("border")->value) {
		addHeader("Border");
		addSlider("Thickness", "Change the border thickness", settings.getSettingByName<float>("borderthickness")->value, 15.0f);
		addToggle("Waypoint color", "Whether the Border should be the waypoint's color.", settings.getSettingByName<bool>("borderuse")->value);
		addConditionalSlider(settings.getSettingByName<bool>("borderuse")->value, "Opacity", "", settings.getSettingByName<float>("borderopacity")->value, 1.0f, 0.0f);
		addConditionalColorPicker(!settings.getSettingByName<bool>("borderuse")->value, "Color", "", settings.getSettingByName<std::string>("bordercolor")->value, settings.getSettingByName<float>("borderopacity")->value, settings.getSettingByName<bool>("borderrgb")->value);
		extraPadding();
	}

	addHeader("Text");
	addToggle("Waypoint color", "Whether the Text should be the waypoint's color.", settings.getSettingByName<bool>("textuse")->value);
	addConditionalSlider(settings.getSettingByName<bool>("textuse")->value, "Opacity", "", settings.getSettingByName<float>("textopacity")->value, 1.0f, 0.0f);
	addConditionalColorPicker(!settings.getSettingByName<bool>("textuse")->value, "Color", "", settings.getSettingByName<std::string>("textcolor")->value, settings.getSettingByName<float>("textopacity")->value, settings.getSettingByName<bool>("textrgb")->value);

	for (auto pair : WaypointList) {
		if (!this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(pair.second.index))) continue;
		if (this->settings.getSettingByName<std::string>("world-" + FlarialGUI::cached_to_string(pair.second.index))->value != SDK::clientInstance->getLocalPlayer()->getLevel()->getWorldFolderName()) continue;
		this->addHeader(this->settings.getSettingByName<std::string>("waypoint-" + FlarialGUI::cached_to_string(pair.second.index))->value);
		this->addToggle("Enabled", "Change if the waypoint should be shown or not.", this->settings.getSettingByName<bool>("state-" + FlarialGUI::cached_to_string(pair.second.index))->value);
		this->addColorPicker("Color", "Change the color of the waypoint.", this->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index))->value, this->settings.getSettingByName<float>("opacity-" + FlarialGUI::cached_to_string(pair.second.index))->value, this->settings.getSettingByName<bool>("rgb-" + FlarialGUI::cached_to_string(pair.second.index))->value);
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
				float fontSize = maxFontSize - (maxFontSize - minFontSize) * (distance / this->settings.getSettingByName<float>("distance")->value);
				fontSize = std::max(minFontSize, std::min(fontSize, maxFontSize));

				//check distance
				if (distance < this->settings.getSettingByName<float>("distance")->value)
				{
					//################ COLORS

					//get the bg color
					D2D1_COLOR_F rect;
					if (this->settings.getSettingByName<bool>("bguse")->value)
					{
						if (this->settings.getSettingByName<bool>("rgb-" + FlarialGUI::cached_to_string(pair.second.index))->value)
						{
							rect = FlarialGUI::rgbColor;
						}
						else {
							rect = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index))->value);
						}

					}
					else {
						if (this->settings.getSettingByName<bool>("bgrgb")->value)
						{
							rect = FlarialGUI::rgbColor;
						}
						else {
							rect = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("bgcolor")->value);
						}
					}
					rect.a = this->settings.getSettingByName<float>("bgopacity")->value;

					//get the border color
					D2D1_COLOR_F border;
					if (this->settings.getSettingByName<bool>("borderuse")->value)
					{
						if (this->settings.getSettingByName<bool>("rgb-" + FlarialGUI::cached_to_string(pair.second.index))->value)
						{
							border = FlarialGUI::rgbColor;
						}
						else {
							border = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index))->value);
						}
					}
					else {
						if (this->settings.getSettingByName<bool>("borderrgb")->value)
						{
							border = FlarialGUI::rgbColor;
						}
						else {
							border = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("bordercolor")->value);
						}
					}
					border.a = this->settings.getSettingByName<float>("borderopacity")->value;

					//get the text color
					D2D1_COLOR_F text;
					if (this->settings.getSettingByName<bool>("textuse")->value)
					{
						if (this->settings.getSettingByName<bool>("rgb-" + FlarialGUI::cached_to_string(pair.second.index))->value)
						{
							text = FlarialGUI::rgbColor;
						}
						else {
							text = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("color-" + FlarialGUI::cached_to_string(pair.second.index))->value);
						}
					}
					else {
						if (this->settings.getSettingByName<bool>("textrgb")->value)
						{
							text = FlarialGUI::rgbColor;
						}
						else {
							text = FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>("textcolor")->value);
						}
					}
					text.a = this->settings.getSettingByName<float>("textopacity")->value;


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
						this->settings.getSettingByName<float>("bgrounding")->value,
						this->settings.getSettingByName<float>("bgrounding")->value
					);

					//draw border
					if (this->settings.getSettingByName<bool>("border")->value)
					{
						FlarialGUI::RoundedHollowRect(
							screen.x - 5.0F,
							screen.y - 20.0F,
							this->settings.getSettingByName<float>("borderthickness")->value,
							border,
							FlarialGUI::TextSizes[name] + 10.0F,
							40.0F,
							this->settings.getSettingByName<float>("bgrounding")->value,
							this->settings.getSettingByName<float>("bgrounding")->value
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

	if (this->isEnabled()) {
		for (int i = 0; i <= totalKeybinds - 1; ++i) {
			if (isKeybind(event.keys) && isKeyPartOfKeybind(event.key))
				keybindActions[i]({ std::any(event) });
		}
	}
}
