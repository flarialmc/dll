#include "ArmorHUD.hpp"

#include "Manager.hpp"
#include "Events/EventManager.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"

void ArmorHUD::onEnable() {
	if (FlarialGUI::inMenu) {
		std::string s = "To change the position of ArmorHUD, Please click " + ModuleManager::getModule("ClickGUI")->getOps<std::string>("editmenubind") + " in the settings tab.";
		std::cout << s << std::endl;
		FlarialGUI::Notify(s);
	}
	Listen(this, RenderEvent, &ArmorHUD::onRender)
		Listen(this, SetupAndRenderEvent, &ArmorHUD::onSetupAndRender)
		Module::onEnable();
}

void ArmorHUD::onDisable() {
	Deafen(this, RenderEvent, &ArmorHUD::onRender)
		Deafen(this, SetupAndRenderEvent, &ArmorHUD::onSetupAndRender)
		Module::onDisable();
}

void ArmorHUD::defaultConfig() {
	settings.renameSetting("colorMain", "colorMain_opacity", "colorMain_rgb", "main");
	settings.renameSetting("staticDurBarColor", "staticDurBarColor_opacity", "staticDurBarColor_rgb", "staticDurBar");
	settings.renameSetting("specialMaxDurBarColor", "specialMaxDurBarColor_opacity", "specialMaxDurBarColor_rgb", "specialMaxDurBar");
	Module::defaultConfig("core");
	Module::defaultConfig("pos");
	setDef("textSize", 0.05f);
	setDef("uiscale", 1.0f);
	setDef("show_offhand", true);
	setDef("show_mainhand", true);
	setDef("vertical", false);
	setDef("durability_left", false);
	setDef("percent", false);
	setDef("color", false);
	setDef("showdurability", false);
	setDef("main", (std::string)"FFFFFF", 1.f, false);
	setDef("staticDurBar", (std::string)"FFFFFF", 1.f, false);
	setDef("specialMaxDurBar", (std::string)"FFFFFF", 1.f, false);
	setDef("textShadow", true);
	setDef("textShadowOffset", 0.003f);
	setDef("textShadow", (std::string)"00000", 0.55f, false);
	setDef("fillGaps", true);
	setDef("textOffsetX", 17.55f);
	setDef("textOffsetY", 7.697f);
	setDef("showDurBar", true);
	setDef("durBarOffsetX", 1.92f);
	setDef("durBarOffsetY", 12.45f);
	setDef("durBarOpacity", 1.f);
	setDef("spacing", 1.f);
	setDef("hideMaxDurText", false);
	setDef("showSpecialMaxDurBarCol", false);
	setDef("showStaticDurBarCol", false);
	setDef("overrideSpecialMaxDurBarCol", false);
	setDef("showDurBarMax", false);
	setDef("100color", 120.f);
	setDef("0color", 0.f);
	
}

void ArmorHUD::settingsRender(float settingsOffset) {
	initSettingsPage();

	addHeader("Armor HUD");
	addSlider("Size", "", "uiscale", 5.f, 0.f, true);
	addSlider("Spacing", "", "spacing", 10.f, 0.f, true);
	addToggle("Vertical ArmorHUD", "To switch between a vertical or horizontal layout", "vertical");
	addConditionalToggle(getOps<bool>("vertical"), "Durability to the left", "", "durability_left");
	addToggle("Show offhand item", "", "show_offhand");
	addToggle("Show mainhand item", "", "show_mainhand");
	addToggle("Fill Empty Slots", "Fill gaps when a piece of armor isn't equipped", "fillGaps");
	addToggle("Change Color", "", "color");

	extraPadding();

	addHeader("Durability");
	addToggle("Durability Text", "", "showdurability");
	addConditionalSlider(getOps<bool>("showdurability") && getOps<bool>("vertical"), "Text Offset X", "", "textOffsetX", 50.f, 0.0f, false);
	addConditionalSlider(getOps<bool>("showdurability") && !getOps<bool>("vertical"), "Text Offset Y", "", "textOffsetY", 50.f, 0.0f, false);
	addConditionalSlider(getOps<bool>("showdurability"), "Text Size", "", "textSize", 0.25f, 0.0f, true);
	addConditionalToggle(getOps<bool>("showdurability"), "Show Durability in %", "", "percent");
	addConditionalToggle(getOps<bool>("showdurability") && !getOps<bool>("percent"), "Hide Max Durability Text", "", "hideMaxDurText");
	addConditionalToggle(getOps<bool>("showdurability"), "Text Shadow", "Displays a shadow under the text", "textShadow");
	addConditionalSlider(getOps<bool>("showdurability") && getOps<bool>("textShadow"), "Shadow Offset", "How far the shadow will be.", "textShadowOffset", 0.02f, 0.001f);

	addToggle("Durability Bar", "", "showDurBar");
	addConditionalSlider(getOps<bool>("showDurBar"), "Durability Bar Offset X", "", "durBarOffsetX", 50.f, 0.0f, false);
	addConditionalSlider(getOps<bool>("showDurBar"), "Durability Bar Offset Y", "", "durBarOffsetY", 50.f, 0.0f, false);
	addConditionalSlider(getOps<bool>("showDurBar"), "Durability Bar Opacity", "", "durBarOpacity", 1.f, 0.0f, false);
	addConditionalToggle(getOps<bool>("showDurBar"), "Show Max Durability Bar", "", "showDurBarMax");

	extraPadding();
	addHeader("Colors");
	addToggle("Show Special Max Durability Bar Color", "", "showSpecialMaxDurBarCol");
	addConditionalColorPicker(getOps<bool>("showSpecialMaxDurBarCol"), "Special Max Durability Bar Color", "", "specialMaxDurBar");
	addColorPicker("Normal Color", "", "main");
	addConditionalColorPicker(getOps<bool>("textShadow"), "Shadow Color", "", "textShadow");
	addToggle("Enable Static Durability Bar Color", "", "showStaticDurBarCol");
	addConditionalColorPicker(getOps<bool>("showStaticDurBarCol"), "Static Durability Bar Color", "", "staticDurBar");
	addConditionalToggle(getOps<bool>("showStaticDurBarCol"), "Override Special Max Durability Bar Color", "", "overrideSpecialMaxDurBarCol");
	addConditionalSlider(!getOps<bool>("showStaticDurBarCol"), "100% Durability Bar Color", "Hue in degrees", "100color", 360.f, 0.0f, false);
	addConditionalSlider(!getOps<bool>("showStaticDurBarCol"), "0% Durability Bar Color", "Hue in degrees", "0color", 360.f, 0.0f, false);

	FlarialGUI::UnsetScrollView();
	resetPadding();
}

void ArmorHUD::renderDurability() {
	bool showOffhand = getOps<bool>("show_offhand");

	D2D1_COLOR_F mainColor = getColor("main");
	D2D1_COLOR_F staticDurBarColor = getColor("staticDurBar");
	D2D1_COLOR_F specialMaxDurBarColor = getColor("specialMaxDurBar");

	const float textSize = getOps<float>("textSize") * 1080;

	bool vertical = getOps<bool>("vertical");
	bool durability_left = getOps<bool>("durability_left");
	bool fillGaps = getOps<bool>("fillGaps");
	bool hideMaxDurText = getOps<bool>("hideMaxDurText");

	float maxDurCol = getOps<float>("100color");
	float minDurCol = getOps<float>("0color");

	float textOffsetX = getOps<float>("textOffsetX");
	float textOffsetY = getOps<float>("textOffsetY");
	float durBarOffsetX = getOps<float>("durBarOffsetX");
	float durBarOffsetY = getOps<float>("durBarOffsetY");
	float ospacing = getOps<float>("spacing");
	float uiscale = getOps<float>("uiscale");
	float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();

	bool showSpecialMaxDurBarCol = getOps<bool>("showSpecialMaxDurBarCol");
	bool overrideSpecialMaxDurBarCol = getOps<bool>("overrideSpecialMaxDurBarCol");
	bool showStaticDurBarColor = getOps<bool>("showStaticDurBarCol");
	bool showTextShadow = getOps<bool>("textShadow");

	if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
		if (SDK::clientInstance->getLocalPlayer() != nullptr)
			if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
				if (getOps<bool>("showdurability") || getOps<bool>("showDurBar")) {
					auto scaledPos = PositionUtils::getCustomScreenScaledPos(Vec2<float>{16, 16}, uiscale);
					float spacing = (vertical ? scaledPos.y : scaledPos.x) * ospacing;

					float xmodifier = 0.0f;
					float ymodifier = 0.0f;

					for (int i = 2; i < 6; i++) {
						if (SDK::clientInstance->getLocalPlayer()->getArmor(i - 2)->getItem() != nullptr) {

							std::string text;

							float durPerc = (float)durabilities[i][0] / (float)durabilities[i][1];

							if (durabilities[i][1] != 0) {
								if (getOps<bool>("percent")) text = FlarialGUI::cached_to_string((int)std::round(durPerc * 100)) + "%";
								else text = FlarialGUI::cached_to_string(durabilities[i][0]) + (hideMaxDurText ? "" : ("/" + FlarialGUI::cached_to_string(durabilities[i][1])));
							}
							else text = FlarialGUI::cached_to_string(SDK::clientInstance->getLocalPlayer()->getArmor(i - 2)->getcount());

							std::wstring widestr = std::wstring(text.begin(), text.end());

							const wchar_t* widecstr = widestr.c_str();

							D2D1_COLOR_F finalColor = mainColor;

							if (durabilities[i][0] != 0 || durabilities[i][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
							if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
							if (showStaticDurBarColor) {
								if (durabilities[i][0] == durabilities[i][1] && durabilities[i][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[i][0] == durabilities[i][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[i][0] != durabilities[i][1]) finalColor = staticDurBarColor;
							}

							finalColor.a = getOps<float>("durBarOpacity");

							if (getOps<bool>("showDurBar") && (getOps<bool>("showDurBarMax") || durabilities[i][0] != durabilities[i][1]) && durabilities[i][1] != 0) {
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									D2D1::ColorF(D2D1::ColorF::Black, getOps<float>("durBarOpacity")),
									guiscale * uiscale * 13.f,
									guiscale * uiscale * 2.f,
									0, 0
								);
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									finalColor,
									durPerc * (guiscale * uiscale * 13.f),
									guiscale * uiscale * 1.f,
									0, 0
								);
							}

							float textX = currentPos.x + (xmodifier + (vertical ? (textOffsetX * uiscale * guiscale) : 0)) * (durability_left ? -1 : 1);
							float textY = currentPos.y + ymodifier + (vertical ? 0 : (textOffsetY * uiscale * guiscale));

							// armor
							if (getOps<bool>("showdurability")) {
								if (showTextShadow) FlarialGUI::FlarialTextWithFont(
									textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
									textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
									widecstr,
									vertical ? 0 : 16 * uiscale * guiscale,
									vertical ? 16 * uiscale * guiscale : 0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
									textSize * uiscale * guiscale, DWRITE_FONT_WEIGHT_NORMAL,
									getColor("textShadow"),
									true
								);

								FlarialGUI::FlarialTextWithFont(
									textX,
									textY, widecstr,
									vertical ? 0 : 16 * uiscale * guiscale,
									vertical ? 16 * uiscale * guiscale : 0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
									textSize * uiscale * guiscale,
									DWRITE_FONT_WEIGHT_NORMAL, finalColor, true);
							}
							if (fillGaps) {
								if (vertical) ymodifier += spacing;
								else xmodifier += spacing;
							}

						}
						if (!fillGaps) {
							if (vertical) ymodifier += spacing;
							else xmodifier += spacing;
						}
					}

					if (getOps<bool>("show_mainhand") && SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
						SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot())->getItem() !=
						nullptr) {
						ItemStack* currentItem = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
							SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());

						std::string text;

						float durPerc = (float)durabilities[0][0] / (float)durabilities[0][1];

						if (durabilities[0][1] != 0) {
							if (getOps<bool>("percent")) text = FlarialGUI::cached_to_string((int)std::round(durPerc * 100)) + "%";
							else text = FlarialGUI::cached_to_string(durabilities[0][0]) + (hideMaxDurText ? "" : ("/" + FlarialGUI::cached_to_string(durabilities[0][1])));
						}
						else text = FlarialGUI::cached_to_string(currentItem->getcount());

						std::wstring widestr = std::wstring(text.begin(), text.end());

						const wchar_t* widecstr = widestr.c_str();

						D2D1_COLOR_F finalColor = mainColor;

						if (durabilities[0][0] != 0 || durabilities[0][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
						if (durabilities[0][0] == durabilities[0][1] && durabilities[0][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
						if (showStaticDurBarColor) {
							if (durabilities[0][0] == durabilities[0][1] && durabilities[0][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
							else if (durabilities[0][0] == durabilities[0][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
							else if (durabilities[0][0] != durabilities[0][1]) finalColor = staticDurBarColor;
						}

						finalColor.a = getOps<float>("durBarOpacity");

						if (getOps<bool>("showDurBar") && (getOps<bool>("showDurBarMax") || durabilities[0][0] != durabilities[0][1]) && durabilities[0][1] != 0) {
							FlarialGUI::RoundedRect(
								currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
								currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
								D2D1::ColorF(D2D1::ColorF::Black, getOps<float>("durBarOpacity")),
								guiscale * uiscale * 13.f,
								guiscale * uiscale * 2.f,
								0, 0
							);
							FlarialGUI::RoundedRect(
								currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
								currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
								finalColor,
								durPerc * (guiscale * uiscale * 13.f),
								guiscale * uiscale * 1.f,
								0, 0
							);
						}

						float textX = currentPos.x + (xmodifier + (vertical ? (textOffsetX * uiscale * guiscale) : 0)) * (durability_left ? -1 : 1);
						float textY = currentPos.y + ymodifier + (vertical ? 0 : (textOffsetY * uiscale * guiscale));

						// main hand
						if (getOps<bool>("showdurability")) {
							if (showTextShadow) FlarialGUI::FlarialTextWithFont(
								textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
								textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
								widecstr,
								vertical ? 0 : 16 * uiscale * guiscale,
								vertical ? 16 * uiscale * guiscale : 0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
								textSize * uiscale * guiscale, DWRITE_FONT_WEIGHT_NORMAL,
								getColor("textShadow"),
								true
							);

							FlarialGUI::FlarialTextWithFont(
								textX,
								textY, widecstr,
								vertical ? 0 : 16 * uiscale * guiscale,
								vertical ? 16 * uiscale * guiscale : 0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
								textSize * uiscale * guiscale,
								DWRITE_FONT_WEIGHT_NORMAL, finalColor, true);
						}

						if (fillGaps) {
							if (vertical) ymodifier += spacing;
							else xmodifier += spacing;
						}
					}
					if (!fillGaps) {
						if (vertical) ymodifier += spacing;
						else xmodifier += spacing;
					}
					if (showOffhand) {
						if (SDK::clientInstance->getLocalPlayer()->getOffhandSlot() != nullptr &&
							SDK::clientInstance->getLocalPlayer()->getOffhandSlot()->getItem() != nullptr) {

							ItemStack* offhandItem = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();

							std::string text;

							float durPerc = (float)durabilities[1][0] / (float)durabilities[1][1];

							if (durabilities[1][1] != 0) {
								if (getOps<bool>("percent")) text = FlarialGUI::cached_to_string((int)std::round(durPerc * 100)) + "%";
								else text = FlarialGUI::cached_to_string(durabilities[1][0]) + (hideMaxDurText ? "" : ("/" + FlarialGUI::cached_to_string(durabilities[1][1])));
							}
							else text = FlarialGUI::cached_to_string(offhandItem->getcount());

							std::wstring widestr = std::wstring(text.begin(), text.end());

							const wchar_t* widecstr = widestr.c_str();

							D2D1_COLOR_F finalColor = mainColor;

							if (durabilities[1][0] != 0 || durabilities[1][1] != 0) finalColor = FlarialGUI::HSVtoColorF(minDurCol + durPerc * (maxDurCol - minDurCol), 1.f, 1.f);
							if (durabilities[1][0] == durabilities[1][1] && durabilities[1][1] != 0 && showSpecialMaxDurBarCol) finalColor = specialMaxDurBarColor;
							if (showStaticDurBarColor) {
								if (durabilities[1][0] == durabilities[1][1] && durabilities[1][1] != 0 && overrideSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[1][0] == durabilities[1][1] && !showSpecialMaxDurBarCol) finalColor = staticDurBarColor;
								else if (durabilities[1][0] != durabilities[1][1]) finalColor = staticDurBarColor;
							}

							finalColor.a = getOps<float>("durBarOpacity");

							if (getOps<bool>("showDurBar") && (getOps<bool>("showDurBarMax") || durabilities[1][0] != durabilities[1][1]) && durabilities[1][1] != 0) {
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									D2D1::ColorF(D2D1::ColorF::Black, getOps<float>("durBarOpacity")),
									guiscale * uiscale * 13.f,
									guiscale * uiscale * 2.f,
									0, 0
								);
								FlarialGUI::RoundedRect(
									currentPos.x + xmodifier + (durBarOffsetX * uiscale * guiscale),
									currentPos.y + ymodifier + (durBarOffsetY * uiscale * guiscale),
									finalColor,
									durPerc * (guiscale * uiscale * 13.f),
									guiscale * uiscale * 1.f,
									0, 0
								);
							}

							float textX = currentPos.x + (xmodifier + (vertical ? (textOffsetX * uiscale * guiscale) : 0)) * (durability_left ? -1 : 1);
							float textY = currentPos.y + ymodifier + (vertical ? 0 : (textOffsetY * uiscale * guiscale));

							// offhand
							if (getOps<bool>("showdurability")) {
								if (showTextShadow) FlarialGUI::FlarialTextWithFont(
									textX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
									textY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset")) * uiscale,
									widecstr,
									vertical ? 0 : 16 * uiscale * guiscale,
									vertical ? 16 * uiscale * guiscale : 0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
									textSize * uiscale * guiscale, DWRITE_FONT_WEIGHT_NORMAL,
									getColor("textShadow"),
									true
								);

								FlarialGUI::FlarialTextWithFont(
									textX,
									textY, widecstr,
									vertical ? 0 : 16 * uiscale * guiscale,
									vertical ? 16 * uiscale * guiscale : 0, vertical ? durability_left ? DWRITE_TEXT_ALIGNMENT_TRAILING : DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER,
									textSize * uiscale * guiscale,
									DWRITE_FONT_WEIGHT_NORMAL, finalColor, true);
							}
						}
					}
				}
			}
	}
}


void ArmorHUD::onRender(RenderEvent& event) {
	if (ClientInstance::getTopScreenName() == "hud_screen" &&
		isEnabled()) {

		// guiscale * uiscale = 1px in minecraft
		float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();
		float uiscale = guiscale * getOps<float>("uiscale");

		float s = 16 * uiscale;

		float spacing = getOps<float>("spacing");

		Vec2<float> settingperc = Vec2<float>(getOps<float>("percentageX"), getOps<float>("percentageY"));

		float width;
		float height;

		if (!getOps<bool>("vertical")) {
			width = s * 5 * spacing + s;
			height = s;
		}
		else {
			width = s;
			height = s * 5 * spacing + s;
		}

		if (settingperc.x != 0) currentPos = Vec2<float>(settingperc.x * (MC::windowSize.x - width), settingperc.y * (MC::windowSize.y - height));
		else currentPos = Constraints::CenterConstraint(width, height);

		if (ClickGUI::editmenu) {
			// bounding boxes
			FlarialGUI::SetWindowRect(currentPos.x, currentPos.y, width, height, 18, this->name);
			checkForRightClickAndOpenSettings(currentPos.x, currentPos.y, width, height);
		}

		Vec2<float> vec2;
		// bounding boxes
		vec2 = FlarialGUI::CalculateMovedXY(currentPos.x, currentPos.y, 18, width, height);


		currentPos.x = vec2.x;
		currentPos.y = vec2.y;

		Vec2<float> percentages = Constraints::CalculatePercentage(currentPos.x, currentPos.y, width, height);

		settings.setValue("percentageX", percentages.x);
		settings.setValue("percentageY", percentages.y);

		if (ClickGUI::editmenu) {
			FlarialGUI::UnsetWindowRect();
		}

		renderDurability();
	}

	if (SDK::getCurrentScreen() != "hud_screen") ClickGUI::editmenu = false;
}

void ArmorHUD::onSetupAndRender(SetupAndRenderEvent& event) {
	if (isEnabled())
		if (ClientInstance::getTopScreenName() == "hud_screen") {
			auto muirc = event.getMuirc();
			BaseActorRenderContext barc(muirc->getScreenContext(), muirc->getClientInstance(),
				muirc->getClientInstance()->getMinecraftGame());

			Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);

			float uiscale = getOps<float>("uiscale");
			float guiscale = SDK::clientInstance->getGuiData()->getGuiScale();

			float spacing = 16 * uiscale * getOps<float>("spacing");

			bool vertical = getOps<bool>("vertical");
			bool fillGaps = getOps<bool>("fillGaps");

			float xmodifier = 0.0f;
			float ymodifier = 0.0f;

			if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
				if (SDK::clientInstance->getLocalPlayer() != nullptr)
					if (SDK::clientInstance->getLocalPlayer()->getSupplies() != nullptr) {
						for (int i = 2; i < 6; i++) {
							auto armorSlot = SDK::clientInstance->getLocalPlayer()->getArmor(i - 2);

							if (armorSlot->getItem() != nullptr) {

								auto maxDamage = armorSlot->getMaxDamage();
								auto durabilityLeft = maxDamage - armorSlot->getDamageValue();

								durabilities[i][1] = maxDamage;
								durabilities[i][0] = durabilityLeft;

								barc.itemRenderer->renderGuiItemNew(&barc,
									armorSlot,
									0,
									scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
									settings.getSettingByName<float>(
										"uiscale")->value, false);

								if (armorSlot->isEnchanted()) {
									barc.itemRenderer->renderGuiItemNew(&barc,
										armorSlot,
										0,
										scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
										settings.getSettingByName<float>(
											"uiscale")->value,
										true);
								}

								if (fillGaps) {
									if (vertical) ymodifier += spacing;
									else xmodifier += spacing;
								}
							}
							if (!fillGaps) {
								if (vertical) ymodifier += spacing;
								else xmodifier += spacing;
							}
						}

						if (getOps<bool>("show_mainhand") && SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
							SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot())->getItem() !=
							nullptr) {
							auto item = SDK::clientInstance->getLocalPlayer()->getSupplies()->getInventory()->getItem(
								SDK::clientInstance->getLocalPlayer()->getSupplies()->getSelectedSlot());

							auto maxDamage = item->getMaxDamage();
							auto durabilityLeft = maxDamage - item->getDamageValue();

							durabilities[0][1] = maxDamage;
							durabilities[0][0] = durabilityLeft;

							barc.itemRenderer->renderGuiItemNew(&barc,
								item,
								0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
								settings.getSettingByName<float>(
									"uiscale")->value,
								false);

							if (item->isEnchanted()) {
								barc.itemRenderer->renderGuiItemNew(&barc,
									item,
									0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
									settings.getSettingByName<float>(
										"uiscale")->value,
									true);
							}
							if (fillGaps) {
								if (vertical) ymodifier += spacing;
								else xmodifier += spacing;
							}
						}

						if (!fillGaps) {
							if (vertical) ymodifier += spacing;
							else xmodifier += spacing;
						}

						auto showOffhand = getOps<bool>("show_offhand");
						if (showOffhand) {
							if (SDK::clientInstance->getLocalPlayer()->getOffhandSlot() != nullptr &&
								SDK::clientInstance->getLocalPlayer()->getOffhandSlot()->getItem() != nullptr) {
								auto item = SDK::clientInstance->getLocalPlayer()->getOffhandSlot();

								auto maxDamage = item->getMaxDamage();
								auto durabilityLeft = maxDamage - item->getDamageValue();

								durabilities[1][1] = maxDamage;
								durabilities[1][0] = durabilityLeft;

								barc.itemRenderer->renderGuiItemNew(&barc,
									item,
									0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
									settings.getSettingByName<float>(
										"uiscale")->value,
									false);

								if (item->isEnchanted()) {
									barc.itemRenderer->renderGuiItemNew(&barc,
										item,
										0, scaledPos.x + xmodifier, scaledPos.y + ymodifier, 1.0f,
										settings.getSettingByName<float>(
											"uiscale")->value,
										true);
								}
							}
						}
					}
			}
		}
}
