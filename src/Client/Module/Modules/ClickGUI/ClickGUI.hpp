#pragma once

#include "../Module.hpp"
#include "../../../Client.hpp"
#include "Elements/ClickGUIElements.hpp"
#include "SDK/Client/Network/Packet/TextPacket.hpp"
#include "Utils/APIUtils.hpp"
#include "Utils/WinrtUtils.hpp"

#define clickgui ModuleManager::getModule("ClickGUI")

class TextPacket;

struct PageType {
	std::string type = "normal";
	std::string module;
};

struct ScrollInfo {
	float scrollpos = 0;
	float barscrollpos = 0;
};

class ClickGUI : public Module {
private:
	float baseHeightReal = 0.f;
	float baseHeightActual = 0.00001f;
	float realBlurAmount = 0.00001f;
	float radioPushAmount1 = 0.0000001f;
	float radioPushAmount2 = 0.0000001f;
	float width1 = 0.000001f;
	float width2 = 0.000001f;
	float width3 = 0.000001f;
	std::string searchBarString;
	Module* ghostMainModule;
public:
	static inline float modcardOpacity = 1.f;
	static inline float settingsOpacity = 0.f;
	static inline float scriptingOpacity = 0.f;
	static inline float modcardOffset = 0.f;
	static inline float settingsOffset = Constraints::RelativeConstraint(2.f, "height", true);
	static inline float scriptingOffset = Constraints::RelativeConstraint(4.f, "height", true);

	static inline PageType page;
	static inline std::string curr = "modules";
	static inline std::map<std::string, ScrollInfo> scrollInfo;
	static float inline accumilatedPos = 1;
	static float inline accumilatedBarPos = 1;
	static bool inline isAnimatingModSet = false;
	static std::chrono::time_point<std::chrono::high_resolution_clock> favoriteStart;

	D2D_COLOR_F getColor(std::string text) {
		D2D_COLOR_F col = this->settings.getSettingByName<bool>(text + "RGB")->value ? FlarialGUI::rgbColor : FlarialGUI::HexToColorF(this->settings.getSettingByName<std::string>(text + "Col")->value);
		col.a = this->settings.getSettingByName<float>(text + "Opacity")->value;
		return col;
	}

	static bool containsAny(const std::string& str) {
		return std::any_of(APIUtils::onlineUsers.begin(), APIUtils::onlineUsers.end(),
			[&](const std::string& user) {
				return !user.empty() && str.find(user) != std::string::npos;
			});
	}

	std::pair<std::string, size_t> findFirstOf(std::string text, std::vector<std::string> words) {
		size_t first_pos = std::string::npos;
		std::string first;

		for (const auto& word : words) {
			size_t pos = text.find(word);
			if (pos != std::string::npos && pos < first_pos) {
				first_pos = pos;
				first = word;
			}
		}
		std::pair<std::string, size_t> pair{ first, first_pos };
		return pair;
	}

	void onPacketReceive(PacketEvent& event) {
		if (event.getPacket()->getId() != MinecraftPacketIds::Text) return;
		auto* pkt = reinterpret_cast<TextPacket*>(event.getPacket());
		std::string message = pkt->message;
		if (message == " ") event.cancel(); //remove onix promotion on zeqa
		if (Client::settings.getSettingByName<bool>("nochaticon")->value) return;
		if (!message.empty() && !containsAny(String::removeNonAlphanumeric(String::removeColorCodes(message)))) return;

		std::pair<std::string, size_t> name = findFirstOf(message, APIUtils::onlineUsers);

		static std::vector<std::pair<std::string, std::string>> roleColors = {
			{ "Dev", "§b" },
			{ "Staff", "§f" },
			{ "Gamer", "§u" },
			{ "Booster", "§d" },
			{ "Supporter", "§5" },
			{ "Regular", "§4" }
		};

		std::string prefix = "§f[§4FLARIAL§f]§r ";

		for (const auto& [role, color] : roleColors) {
			if (APIUtils::hasRole(role, name.first)) {
				prefix = "§f[" + color + "FLARIAL§f]§r ";
				break;
			}
		}

		if (name.second < message.size()) {
			message.insert(name.second, prefix);
			pkt->message = message;
		}
	}

	ClickGUI() : Module("ClickGUI", "What do you think it is?", IDR_CLICKGUI_PNG, "K") {
		
		this->ghostMainModule = new Module("main", "troll", IDR_COMBO_PNG, "");
		scrollInfo["modules"] = { 0, 0 };
		scrollInfo["scripting"] = { 0, 0 };
		scrollInfo["settings"] = { 0, 0 };

		Listen(this, MouseEvent, &ClickGUI::onMouse)
		//Listen(this, FOVEvent, &ClickGUI::fov)
			Listen(this, KeyEvent, &ClickGUI::onKey)
			ListenOrdered(this, PacketEvent, &ClickGUI::onPacketReceive, EventOrder::IMMEDIATE)
			ListenOrdered(this, RenderEvent, &ClickGUI::onRender, EventOrder::IMMEDIATE)
			Module::onEnable();
	};

	void onSetup() override {}

	void onEnable() override {}

	void onDisable() override {}

	void terminate() override {
		Deafen(this, MouseEvent, &ClickGUI::onMouse)
			Deafen(this, KeyEvent, &ClickGUI::onKey)
			Deafen(this, RenderEvent, &ClickGUI::onRender)
			Deafen(this, PacketEvent, &ClickGUI::onPacketReceive)
			Module::terminate();
	}

	void defaultConfig() override {
		getKeybind();
		Module::defaultConfig("core");
		setDef("editmenubind", (std::string)"L");
		setDef("custom_logo", false);
		setDef("globalText", (std::string)"ffffff", 1.f, false);
		setDef("modCardEnabled", (std::string)"188830", 1.f, false);
		setDef("modCardDisabled", (std::string)"7d1820", 1.f, false);
		setDef("primary1", (std::string)"ff233a", 1.f, false);
		setDef("primary2", (std::string)"ffffff", 1.f, false);
		setDef("primary3", (std::string)"9a6b72", 1.f, false);
		setDef("primary4", (std::string)"704b52", 1.f, false);
		setDef("secondary1", (std::string)"3f2a2d", 1.f, false);
		setDef("secondary2", (std::string)"201a1b", 1.f, false);
		setDef("secondary3", (std::string)"120e0f", 1.f, false);
		setDef("secondary4", (std::string)"1c1616", 1.f, false);
		setDef("secondary5", (std::string)"8b1b25", 1.f, false);
		setDef("secondary6", (std::string)"ff2438", 1.f, false);
		setDef("secondary7", (std::string)"943c3c", 1.f, false);
		setDef("secondary8", (std::string)"302728", 1.0f, false);
		setDef("modcard1", (std::string)"201a1b", 1.0f, false);
		setDef("modcard2", (std::string)"2f2022", 1.0f, false);
		setDef("modcard3", (std::string)"3f2a2d", 1.0f, false);
		setDef("modcard4", (std::string)"705d60", 1.0f, false);
		setDef("modicon", (std::string)"1A1313", 1.0f, false);
		setDef("flariallogo", (std::string)"FE4443", 1.0f, false);
		setDef("modsettings", (std::string)"FFFFFF", 1.0f, false);
		setDef("enabledRadioButton", (std::string)"D0A0A8", 1.0f, false);
		setDef("disabledRadioButton", (std::string)"FFFFFF", 1.0f, false);
		setDef("_overrideAlphaValues_", 1.f);
		saveSettings();
	}

	void settingsRender(float settingsOffset) override {
		if (getOps<std::string>("editmenubind").empty())
			getOps<std::string>("editmenubind") = "L";

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);

		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		this->addHeader("Keybinds");
		this->addKeybind("Open Menu Bind", "", "keybind", true);
		this->addKeybind("Edit Menu Bind", "", "editmenubind", true);

		this->extraPadding();

		this->addHeader("Logo");
		// this->addToggle("Custom logo", "", "custom_logo");
		// if (!getOps<bool>("custom_logo")) {
		this->addColorPicker("Logo Color", "Color of the client's logo", "flariallogo");
		// } else {
		//     this->addButton("", "To customize the logo, change the 'custom-logo.png' file in Flarial's Assets folder.", "Open Subfolder", [this]() {
		//         WinrtUtils::openSubFolder("Flarial\\assets");
		//     });
		// }

		addHeader("Colors");
		addColorPicker("Radio Button Icon Disabled", "", "enabledRadioButton");
		addColorPicker("Radio Button Icon Enabled", "", "disabledRadioButton");
		addColorPicker("Text Color", "", "globalText");
		addColorPicker("Enabled", "", "modCardEnabled");
		addColorPicker("Disabled", "", "modCardDisabled");
		addColorPicker("Primary 1", "Active elements, main color of sliders, bg color of enabled toggles", "primary1");
		addColorPicker("Primary 2", "Minor color of toggles and sliders, text Indicator color", "primary2");
		addColorPicker("Primary 3", "Color of inactive elements, unfilled slider bar color, inner color of color pickers, background color of disabled toggles","primary3");
		addColorPicker("Primary 4", "Base color of color pickers, hover color of dropdown childrens", "primary4");
		addColorPicker("Secondary 1", "Background color of settings", "secondary1");
		addColorPicker("Secondary 2", "Background color of navigation bar, tooltips, and buttons", "secondary2");
		addColorPicker("Secondary 3", "Background color of base Click GUI rectangle", "secondary3");
		addColorPicker("Secondary 4", "Background color of active Modules search bar", "secondary4");
		addColorPicker("Secondary 5", "Background color of navigation bar icons", "secondary5");
		addColorPicker("Secondary 6", "Background color of active navigation bar buttons", "secondary6");
		addColorPicker("Secondary 7", "Outline color of tooltips", "secondary7");
		addColorPicker("Secondary 8", "Navigation bar background deactivated Color", "secondary8");
		addColorPicker("Modcard 1", "Color of top rounded rectangle", "modcard1");
		addColorPicker("Modcard 2", "Color of bottom rounded rectangle", "modcard2");
		addColorPicker("Modcard 3", "Background color of module icons, outer color of module settings icon", "modcard3");
		addColorPicker("Modcard 4", "Inner color of module settings icon", "modcard4");
		addColorPicker("Modcard Icon", "Color of the mod's icon", "modicon");
		addColorPicker("Setting Icon Color", "Color of the mod's settings icon", "modsettings");
		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	static inline bool editmenu = false;

	static std::pair<float, float>
		centerChildRectangle(float parentWidth, float parentHeight, float childWidth, float childHeight) {
		return std::make_pair((parentWidth - childWidth) / 2, (parentHeight - childHeight) / 2);
	}

	void onRender(RenderEvent& event);

	//void fov(FOVEvent& event);

	void onKey(KeyEvent& event) {

		//TODO: MAKE module->setActive() module->isActive() module->isRestricted()

		if (this->isKeybind(event.keys) && this->isKeyPartOfKeybind(event.key) && event.getAction() == ActionType::Pressed) {

			if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen")
				this->active = false;
			else {
				if (!editmenu) {
					if (!Client::settings.getSettingByName<bool>("nochaticon")->value) Listen(this, PacketEvent, &ClickGUI::onPacketReceive)
					else Deafen(this, PacketEvent, &ClickGUI::onPacketReceive);
					ModuleManager::cguiRefresh = true;
					keybindActions[0]({});
				}
			}

			if (this->active) {
				accumilatedPos = 0;
				accumilatedBarPos = 0;

				page.type = "normal";
				curr = "modules";
			}
			else {
				SDK::clientInstance->grabMouse();
				FlarialGUI::ResetShit();
				ModuleManager::SaveModulesConfig();
				Client::SaveSettings();
			}
		}

		// if clicked esc
		if ((event.getKey() == VK_ESCAPE && event.getAction() == ActionType::Released)) {
			if (!editmenu) {
				if (this->active) {
					// exit ClickGUI
					SDK::clientInstance->grabMouse(); // let mouse control the view

					this->active = false;
					page.type = "normal";
					curr = "modules";

					FlarialGUI::ResetShit();
					ModuleManager::SaveModulesConfig();
					Client::SaveSettings();
				}
			}
			else {
				// switch from edit mode back to ClickGUI
				editmenu = false;
				this->active = true;
			}
		}

		if (this->active) {
			SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

			// auto search? TODO: make it optional
			if (page.type == "normal" && curr == "modules" &&
				event.getAction() == ActionType::Pressed) {

				//FlarialGUI::TextBoxes[0].isActive = true;
				// you searching
				if (FlarialGUI::TextBoxes[0].isActive) {
					FlarialGUI::scrollpos = 0;
					FlarialGUI::barscrollpos = 0;
					accumilatedPos = 0;
					accumilatedBarPos = 0;
				}
			}

			if (this->isAdditionalKeybind(event.keys,
				getOps<std::string>("editmenubind")) &&
				Module::isKeyPartOfAdditionalKeybind(event.key, this->settings.getSettingByName<std::string>(
					"editmenubind")->value)) {

				if (!editmenu) {
					this->active = false;
					FlarialGUI::Notify("Right click a module to directly go to their settings page.");
					FlarialGUI::Notify("To disable this menu press ESC or " +
						getOps<std::string>("editmenubind"));
					editmenu = true;
				}

			}
			// switch back to ClickGUI if clicked on Edit Mode bind
		}
		else if (editmenu && this->isAdditionalKeybind(event.keys, this->settings.getSettingByName<std::string>(
			"editmenubind")->value) && Module::isKeyPartOfAdditionalKeybind(event.key,
				this->settings.getSettingByName<std::string>(
					"editmenubind")->value) ||
			editmenu && this->isKeybind(event.keys) && Module::isKeyPartOfAdditionalKeybind(event.key,
				this->settings.getSettingByName<std::string>(
					"editmenubind")->value)) {

			editmenu = false;
			this->active = true;

		}

		if (this->active || editmenu && SDK::getCurrentScreen() == "hud_screen")
			SDK::clientInstance->releaseMouse(); // release mouse lets cursor move

		if (this->active || editmenu)
			event.cancel(); // do not pass key event to the game

		FlarialGUI::inMenu = this->active;
	}

	void onMouse(MouseEvent& event) {
		MC::mousePos.x = event.getMouseX();
		MC::mousePos.y = event.getMouseY();
		MC::mouseButton = event.getButton();
		MC::mouseAction = event.getAction();

		if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) MC::held = true;
		if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) MC::held = false;
		if (event.getButton() != MouseButton::None) MC::lastMouseButton = event.getButton();

		if (event.getButton() == MouseButton::Scroll) {
			accumilatedPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::scrollposmodifier
				: -FlarialGUI::scrollposmodifier;
			accumilatedBarPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::barscrollposmodifier
				: -FlarialGUI::barscrollposmodifier;
		}

		if (this->active) event.cancel();

#if !defined(__DEBUG__)
		if (SDK::getCurrentScreen() != "hud_screen" && SDK::getCurrentScreen() != "pause_screen") {
			if (this->active)
				this->active = false;
		}
#endif

		if ((this->active || editmenu) && SDK::getCurrentScreen() == "hud_screen")
			event.cancel(); // TODO: modules dont listen for canceled state!!!

	}
};
