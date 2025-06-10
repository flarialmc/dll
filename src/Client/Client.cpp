#include "Client.hpp"

#include <filesystem>
#include <thread>

#include <Utils/VersionUtils.hpp>
#include <Utils/WinrtUtils.hpp>

#include "Command/CommandManager.hpp"

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <Scripting/ScriptManager.hpp>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#include "Utils/APIUtils.hpp"

namespace winrt
{
	using namespace Windows::UI::Core;
	using namespace Windows::Foundation;
	using namespace Windows::ApplicationModel::Activation;
	using namespace Windows::ApplicationModel::Core;
}

Settings Client::settings = Settings();
Settings Client::legacySettings = Settings();

bool notifiedOfConnectionIssue = false;

std::string Client::current_commit = COMMIT_HASH;
std::vector<std::string> Client::availableConfigs;

std::vector<std::string> Client::getPlayersVector(const nlohmann::json& data) {
	std::vector<std::string> allPlayers;

	try {
		// Check if the JSON is an array of players
		if (data.is_array()) {
			for (const auto& player : data) {
				if (player.is_string()) {
					allPlayers.push_back(player.get<std::string>());
				}
			}
		}
		else {
			LOG_ERROR("Invalid JSON format: expected an array of players.");
		}
	}
	catch (const nlohmann::json::exception& e) {
		LOG_ERROR("Error parsing players: {}", e.what());
	}

	if (SDK::clientInstance && SDK::clientInstance->getLocalPlayer()) {
		try {
			std::string name = SDK::clientInstance->getLocalPlayer()->getPlayerName();
			std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(name));

			if (clearedName.empty()) {
				clearedName = String::removeColorCodes(name);
			}
			allPlayers.push_back(clearedName);
		}
		catch (const std::exception& e) {
			LOG_ERROR("Error processing local player name: {}", e.what());
		}
	}

	return allPlayers;
}


bool Client::disable = false;

winrt::event_token activationToken;

void RegisterActivationHandler()
{
	activationToken = winrt::CoreApplication::MainView().Activated(
		[](const auto&, const winrt::IActivatedEventArgs& context) {
			if (context.Kind() != winrt::ActivationKind::Protocol)
				return;

			auto uri = winrt::unbox_value<winrt::ProtocolActivatedEventArgs>(context).Uri();

			std::vector<std::pair<std::wstring, std::wstring>> dataList;

			for (const auto& dataContext : uri.QueryParsed()) {
				std::wstring name = dataContext.Name().c_str();
				std::wstring value = dataContext.Value().c_str();

				dataList.emplace_back(name, value);
			}

			auto event = nes::make_holder<ProtocolEvent>(uri.Host().c_str(), dataList);
			eventMgr.trigger(event);
		});
}

void Client::UnregisterActivationHandler()
{
	if (activationToken) // Check if the token is valid
	{
		winrt::CoreApplication::MainView().Activated(activationToken); // Unregister using the token
	}
}

void Client::createConfig(std::string name) {
	// name is with .json extension
	std::ofstream file(Utils::getConfigsPath() + "\\" + name, std::ios::app);
	if (!file) LOG_ERROR("Failed to create new config file '{}'", name);
	else Client::switchConfig(name);
}

void Client::switchConfig(std::string name, bool deleting) {
	// name is with .json extension
	if (!std::filesystem::exists(Utils::getConfigsPath() + "\\" + name)) return Client::createConfig(name);

	Client::settings.getSettingByName<std::string>("currentConfig")->value = name;
	Client::activeConfig = name;

	if (Client::hasLegacySettings) {
		name.resize(name.length() - 5); // remove .json extension for legacy settings
		Client::legacySettings.getSettingByName<std::string>("currentConfig")->value = name;
	}

	if (!deleting) Client::SaveSettings();
	Client::SavePrivate();
	Client::LoadPrivate();
}

void Client::deleteConfig(std::string name) {
	Client::switchConfig("default.json", true);
	std::string to = Utils::getConfigsPath() + "\\" + name;
	if (std::filesystem::exists(to)) {
		std::filesystem::remove_all(to);
	}
}

void Client::loadAvailableConfigs() {
	const std::string directoryPath = Utils::getConfigsPath();
	if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
			if (entry.path().extension() == ".json") {
				availableConfigs.push_back(entry.path().filename().string());
			}
		}
	}
	if (Client::hasLegacySettings) {
		const std::string directoryPath = Client::legacyDir;
		if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) return;

		for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
			if (is_directory(entry.path())) {
				std::string configName = entry.path().filename().string() + ".json";
				if (std::find(availableConfigs.begin(), availableConfigs.end(), configName) == availableConfigs.end()) availableConfigs.push_back(configName);
			}
		}
	}
}

void Client::initialize() {
	winrt::init_apartment();


#if defined(__TEST__)
	WinrtUtils::setWindowTitle(fmt::format("Flarial v{} {} {}", FLARIAL_VERSION, FLARIAL_BUILD_TYPE, FLARIAL_BUILD_DATE));
#else
	WinrtUtils::setWindowTitle(fmt::format("Flarial v{} {}", FLARIAL_VERSION, FLARIAL_BUILD_DATE));
#endif

	VersionUtils::initialize();
	version = VersionUtils::getFormattedVersion();
	if (!VersionUtils::isSupported(Client::version)) {
		LOG_FATAL("{}", "Unsupported Minecraft version");
		Utils::MessageDialogW(L"Flarial: this version is not supported!", FlarialGUI::to_wide("VERSION " + version + " NOT SUPPORTED!").c_str());
		ModuleManager::terminate();
		Client::disable = true;
		return;
	}


	VersionUtils::addData();


    std::vector<std::filesystem::path> directories = {
        Utils::getRoamingPath() + "\\Flarial",
        Utils::getRoamingPath() + "\\Flarial\\assets",
        Utils::getRoamingPath() + "\\Flarial\\logs",
        Utils::getRoamingPath() + "\\Flarial\\Config",
        Utils::getRoamingPath() + "\\Flarial\\Scripts",
        Utils::getRoamingPath() + "\\Flarial\\Crosshairs"
    };


	std::thread updateThread([]() {
		std::string playersList;
		std::string filePath = Utils::getRoamingPath() + "\\Flarial\\playerscache.txt";
		std::ifstream file(filePath);
		if (!file.is_open()) {
			std::ofstream createFile(filePath);
			if (!createFile.is_open()) {
				LOG_ERROR("Could not create file: ");
			}
			else {
				createFile.close();
				file.open(filePath);
				if (!file.is_open()) {
					LOG_ERROR("Could not open file for reading after creation: ");
				}
			}
		}


		std::stringstream buffer;
		buffer << file.rdbuf();
		playersList = buffer.str();
		file.close();

		APIUtils::onlineUsers = APIUtils::ListToVector(playersList);
		});

	updateThread.detach();



	for (const auto& path : directories) {
		if (!std::filesystem::exists(path)) {
			std::filesystem::create_directory(path);
		}
	}

	Client::LoadLegacySettings();
	Client::CheckSettingsFile();
	if (Client::privateInit) {
		Client::LoadPrivate();
		Client::LoadSettings();
	}

	Logger::success("4");

	ADD_SETTING("fontname", std::string("Space Grotesk"));
	ADD_SETTING("mod_fontname", std::string("Space Grotesk"));
	ADD_SETTING("blurintensity", 2.0f);
	ADD_SETTING("killdx", false);
	ADD_SETTING("disable_alias", false);
	ADD_SETTING("vsync", false);
	ADD_SETTING("recreateAtStart", false);
	ADD_SETTING("forceIntel", true);
	ADD_SETTING("promotions", true);
	ADD_SETTING("snappinglines", true);
	ADD_SETTING("apiusage", true);
	ADD_SETTING("donotwait", true);
	ADD_SETTING("bufferingmode", std::string("Double Buffering"));
	ADD_SETTING("swapeffect", std::string("FLIP_SEQUENTIAL"));
	ADD_SETTING("disableanims", false);
	ADD_SETTING("anonymousApi", false);
	ADD_SETTING("dlassets", true);
	ADD_SETTING("noicons", false);
	ADD_SETTING("noshadows", false);
	ADD_SETTING("watermark", true);
	ADD_SETTING("centreCursor", false);
	ADD_SETTING("aliasingMode", std::string("Default"));
	ADD_SETTING("ejectKeybind", std::string(""));
	ADD_SETTING("enabledModulesOnTop", false);
	ADD_SETTING("rgb_speed", 1.0f);
	ADD_SETTING("rgb_saturation", 1.0f);
	ADD_SETTING("rgb_value", 1.0f);
	ADD_SETTING("pixelateFonts", false);
	ADD_SETTING("modules_font_scale", 1.0f);
	ADD_SETTING("gui_font_scale", 1.0f);
	ADD_SETTING("overrideFontWeight", false);
	ADD_SETTING("fontWeight", std::string("Normal"));
	ADD_SETTING("nologoicon", false);
	ADD_SETTING("nochaticon", false);
	ADD_SETTING("currentConfig", std::string("default.json"));
	ADD_SETTING("resettableSettings", true);
	ADD_SETTING("clearTextBoxWhenClicked", true);

	if (Client::hasLegacySettings && !Client::softLoadLegacy) Client::settings.getSettingByName<std::string>("currentConfig")->value = Client::legacySettings.getSettingByName<std::string>("currentConfig")->value + ".json";

	Client::loadAvailableConfigs();
	Client::SavePrivate();
	if (Client::hasLegacySettings) Client::LoadPrivate();

	Logger::success("5");


	FlarialGUI::ExtractImageResource(IDR_RED_LOGO_PNG, "red-logo.png", "PNG");
	FlarialGUI::ExtractImageResource(IDR_CYAN_LOGO_PNG, "dev-logo.png", "PNG");
	FlarialGUI::ExtractImageResource(IDR_GAMER_LOGO_PNG, "gamer-logo.png", "PNG");
	FlarialGUI::ExtractImageResource(IDR_BOOSTER_LOGO_PNG, "booster-logo.png", "PNG");
	FlarialGUI::ExtractImageResource(IDR_WHITE_LOGO_PNG, "white-logo.png", "PNG");
	FlarialGUI::ExtractImageResource(IDR_SUPPORTER_LOGO_PNG, "supporter-logo.png", "PNG");

	FlarialGUI::LoadFont(IDR_FONT_TTF);
	FlarialGUI::LoadFont(IDR_FONT_BOLD_TTF);
	FlarialGUI::LoadFont(IDR_MINECRAFTIA_TTF);

	Logger::success("6");

	RegisterActivationHandler();
	HookManager::initialize();
	MH_ApplyQueued();
	ModuleManager::initialize();
	CommandManager::initialize();
	ScriptManager::initialize();
}

std::string window = "Minecraft";

HWND hWnd = FindWindow(nullptr, window.c_str());
RECT currentRect = { 0 };
RECT clientRect = { 0 };
RECT previousRect = { 0 };
bool InHudScreen = false;

bool toes = false;

void Client::centerCursor() {
	if (Client::disable) return;
	if (hWnd != nullptr && Client::settings.getSettingByName<bool>("centreCursor")->value) {
		if (!toes) {
			toes = true;
			std::thread wow([&]() {
				while (!Client::disable && Client::settings.getSettingByName<bool>("centreCursor")->value) {
					GetWindowRect(hWnd, &currentRect);
					GetClientRect(hWnd, &clientRect);

					if (memcmp(&currentRect, &previousRect, sizeof(RECT)) != 0) {
						previousRect = currentRect;
					}
					Sleep(1000);
				}
				if (Client::disable && !Client::settings.getSettingByName<bool>("centreCursor")->value) toes = false;
				});

			wow.detach();
		}


		if ((SDK::getCurrentScreen() != "hud_screen" && InHudScreen) ||
			(SDK::getCurrentScreen() == "hud_screen" && !InHudScreen)) {
			GetWindowRect(hWnd, &currentRect);
			GetClientRect(hWnd, &clientRect);

			int windowX = currentRect.left;
			int windowY = currentRect.top;

			int centerX = windowX + (clientRect.right) / 2;
			int centerY = windowY + (clientRect.bottom) / 2;

			SetCursorPos(centerX, centerY);

			InHudScreen = !InHudScreen;
		}
	}
}