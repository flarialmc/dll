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

#include "../Assets/Assets.hpp"

namespace winrt
{
	using namespace Windows::UI::Core;
	using namespace Windows::Foundation;
	using namespace Windows::ApplicationModel::Activation;
	using namespace Windows::ApplicationModel::Core;
}

Settings Client::settings = Settings();
Settings Client::legacySettings = Settings();
#include "Hook/Manager.hpp"
#include "Module/Manager.hpp"
#include "Config/Settings.hpp"
#include "Utils/Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


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
bool Client::init = false;

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
	if (name.empty()) return;
	std::ofstream file(Utils::getConfigsPath() + "\\" + name, std::ios::app);
	if (!file) LOG_ERROR("Failed to create new config file '{}'", name);
	else Client::switchConfig(name);
}

void Client::switchConfig(std::string name, bool deleting) {
	// name is with .json extension
	if (name.empty()) name = "default.json";
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
			if (entry.path().extension() == ".json" && !entry.path().filename().empty()) {
				availableConfigs.push_back(entry.path().filename().string());
			}
		}
	}
	if (Client::hasLegacySettings) {
		if (!std::filesystem::exists(Client::legacyDir) || !std::filesystem::is_directory(Client::legacyDir)) return;

		for (const auto& entry : std::filesystem::directory_iterator(Client::legacyDir)) {
			if (is_directory(entry.path())) {
				std::string configName = entry.path().filename().string() + ".json";
				if (configName[0] != '!' && std::find(availableConfigs.begin(), availableConfigs.end(), configName) == availableConfigs.end()) availableConfigs.push_back(configName);
			}
		}
	}
}


void Client::initialize() {
	winrt::init_apartment();


#if defined(__TEST__)
	WinrtUtils::setWindowTitle(fmt::format("Flarial v{} {} {} [{}]", FLARIAL_VERSION, FLARIAL_BUILD_TYPE, FLARIAL_BUILD_DATE, COMMIT_HASH));
#else
	WinrtUtils::setWindowTitle(fmt::format("Flarial v{} {} [{}]", FLARIAL_VERSION, FLARIAL_BUILD_DATE, COMMIT_HASH));
#endif

	VersionUtils::initialize();
	version = VersionUtils::getFormattedVersion();
	if (!VersionUtils::isSupported(Client::version)) {
		LOG_FATAL("{} Unsupported Minecraft version", Client::version);
		MessageBoxA(NULL, "Flarial: this version is not supported!", "", MB_OK);
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
        Utils::getRoamingPath() + "\\Flarial\\Crosshairs",
        Utils::getRoamingPath() + "\\Flarial\\MessageLogger"
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
		APIUtils::onlineUsersSet = APIUtils::onlineUsers | std::ranges::to<decltype(APIUtils::onlineUsersSet)>();
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
	ADD_SETTING("promotions", true);
	ADD_SETTING("saveScrollPos", true);
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
	ADD_SETTING("enabledModulesOnTop", true);
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
	ADD_SETTING("singlewatermark", false);
	ADD_SETTING("watermarkduplicates", true);
	ADD_SETTING("currentConfig", std::string("default.json"));
	ADD_SETTING("resettableSettings", true);
	ADD_SETTING("clearTextBoxWhenClicked", true);
	ADD_SETTING("dotcmdprefix", std::string("."));
	ADD_SETTING("autosearch", false);

	if (Client::hasLegacySettings && !Client::softLoadLegacy) Client::settings.getSettingByName<std::string>("currentConfig")->value = Client::legacySettings.getSettingByName<std::string>("currentConfig")->value + ".json";

	Client::loadAvailableConfigs();
	Client::SavePrivate();
	if (!Client::privateInit) Client::LoadSettings();
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

	Client::PerformPostLegacySetup();
	Client::init = true;
}

std::string window = "Minecraft";

HWND hWnd = FindWindow(nullptr, window.c_str());
RECT currentRect = { 0 };
RECT clientRect = { 0 };
RECT previousRect = { 0 };
bool InHudScreen = false;

bool toes = false;

void Client::PerformPostLegacySetup() {
	if (Client::hasLegacySettings) {
		fs::path pathToCheck;
		if (Client::legacySettings.getSettingByName<std::string>("currentConfig")->value != "default") pathToCheck = fs::path(Client::legacyDir) / fmt::format("{}\\{}", Client::legacyDir, Client::legacySettings.getSettingByName<std::string>("currentConfig")->value);
		else pathToCheck = Client::legacyDir;

		if (fs::exists(pathToCheck) && fs::is_directory(pathToCheck) && pathToCheck.filename().string()[0] != '!') {
			if (Client::legacySettings.getSettingByName<std::string>("currentConfig")->value == "default") {
				if (!fs::exists(Client::legacyDir + "\\!default")) {
					if (fs::create_directory(Client::legacyDir + "\\!default")) {}
					else return LOG_ERROR("Failed to create new default directory for Legacy");
				}

				for (const auto& entry : fs::directory_iterator(Client::legacyDir)) {
					if (entry.is_regular_file() && entry.path().extension() == ".flarial") {
						try {
							fs::rename(entry.path(), (Client::legacyDir + "\\!default") / entry.path().filename());
						}
						catch (fs::filesystem_error& e) {
							return LOG_ERROR("Failed to move file {} to legacy default folder", entry.path().filename().string());
						}
					}
				}
			}
			else {
				std::error_code ec_rename;
				fs::rename(pathToCheck, pathToCheck.parent_path().string() + "\\!" + pathToCheck.filename().string(), ec_rename);

				if (ec_rename) {
					LOG_ERROR("Failed to exclude legacy setting: {}", ec_rename.message());
					return;
				}
			}
		}
	}
}

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

// Static member definitions moved from header
std::string Client::privatePath = Utils::getConfigsPath() + "\\PRIVATE";
std::string Client::activeConfig;
bool Client::hasLegacySettings = false;
bool Client::softLoadLegacy = false;
bool Client::privateInit = false;
bool Client::savingSettings = false;
bool Client::savingPrivate = false;
nlohmann::json Client::globalSettings;
std::string Client::version;
HMODULE Client::currentModule = nullptr;
std::string Client::path = Utils::getConfigsPath() + "\\default.json";
std::string Client::legacyPath = Utils::getConfigsPath() + "\\main.flarial";
std::string Client::legacyDir = Utils::getConfigsPath() + "\\Legacy";

// Function implementations moved from header
void Client::LoadLegacySettings() {
	if (fs::exists(legacyDir) && fs::is_directory(legacyDir)) {
		Client::legacySettings.addSetting("currentConfig", (std::string)"default");
		Client::hasLegacySettings = true;
		Client::softLoadLegacy = true;
		return Logger::custom(fg(fmt::color::dark_magenta), "Config", "Legacy dir already exists... aborting");
	}

	if (!fs::exists(legacyPath)) return Logger::custom(fg(fmt::color::dark_magenta), "Config", "No legacy settings found");

	Client::hasLegacySettings = true;
	Logger::custom(fg(fmt::color::dark_magenta), "Config", "Legacy settings found");

	std::error_code ec_rename;
	fs::rename(Utils::getConfigsPath(), Utils::getClientPath() + "\\Legacy", ec_rename);

	if (ec_rename) {
		LOG_ERROR("Failed to rename Config directory to Legacy: {}", ec_rename.message());
		return;
	}
	else Logger::custom(fg(fmt::color::dark_magenta), "Config", "Renamed Config directory to Legacy");

	if (fs::create_directory(Utils::getConfigsPath())) Logger::custom(fg(fmt::color::dark_magenta), "Config", "Config directory successfully created");
	else {
		LOG_ERROR("Failed to create Config directory");
		return;
	}

	fs::rename(Utils::getClientPath() + "\\Legacy", legacyDir, ec_rename);
	if (ec_rename) {
		LOG_ERROR("Failed to move Legacy folder to Config: {}", ec_rename.message());
		return;
	}
	else Logger::custom(fg(fmt::color::dark_magenta), "Config", "Moved Legacy dir to Config dir");

	legacyPath = legacyDir + "\\main.flarial";

	std::ifstream legacyFile(legacyPath);

	if (!legacyFile) {
		Logger::custom(fg(fmt::color::dark_magenta), "Config", "No legacy settings found");
		return;
	}

	std::stringstream ss;
	ss << legacyFile.rdbuf();
	legacyFile.close();
	legacySettings.FromJson(ss.str(), true);
	settings.AppendFromJson(ss.str(), true);

	if (legacySettings.getSettingByName<std::string>("currentConfig") == nullptr) legacySettings.addSetting("currentConfig", (std::string)"default");
	else if (legacySettings.getSettingByName<std::string>("currentConfig")->value.empty()) legacySettings.setValue("currentConfig", (std::string)"default");

	Logger::custom(fg(fmt::color::dark_magenta), "Config", "Set config to {}", legacySettings.getSettingByName<std::string>("currentConfig")->value);
}

void Client::SavePrivate() {
	if (savingPrivate) return;
	else savingPrivate = true;

	try {
		Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saving PRIVATE");

		if (!settings.getSettingByName<std::string>("currentConfig")) return Logger::warn("No Client Settings to save to PRIVATE");

		DWORD originalAttributes = INVALID_FILE_ATTRIBUTES;
		originalAttributes = GetFileAttributesA(privatePath.c_str());
		if (originalAttributes == INVALID_FILE_ATTRIBUTES) return LOG_ERROR("Failed to get attributes of PRIVATE: {}", GetLastError());
		DWORD attributesToClear = 0;
		if (originalAttributes & FILE_ATTRIBUTE_HIDDEN) attributesToClear |= FILE_ATTRIBUTE_HIDDEN;
		if (attributesToClear != 0)
			if (!SetFileAttributesA(privatePath.c_str(), originalAttributes & ~attributesToClear))
				return LOG_ERROR("Failed to remove temporary attributes from PRIVATE: {}", GetLastError());

		std::ofstream pCls(privatePath, std::ofstream::out | std::ofstream::trunc);
		if (!pCls.is_open()) return LOG_ERROR("Could not clear the contents of PRIVATE: {}", GetLastError());
		pCls.close();

		std::ofstream pFile(privatePath, std::ios::app);
		pFile << settings.ToJson();
		pFile.close();

		Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saved PRIVATE");
		savingPrivate = false;
	}
	catch (const std::exception& e) {
		LOG_ERROR("An error occurred while trying to save settings: {}", e.what());
		savingPrivate = false;
	}
}

void Client::LoadPrivate() {
	Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loading PRIVATE");

	std::ifstream privateFile(privatePath);
	if (!privateFile) return Logger::warn("Client PRIVATE not found");

	std::stringstream pSS;
	pSS << privateFile.rdbuf();
	privateFile.close();

	std::string pStr = pSS.str();

	if (pStr.empty() || pStr == "null") return Logger::warn("Empty or null PRIVATE");

	try { settings.FromJson(pStr); }
	catch (const nlohmann::json::parse_error& e) {
		LOG_ERROR("Failed to parse JSON: {}", e.what());
	}

	path = Utils::getConfigsPath() + "\\" + settings.getSettingByName<std::string>("currentConfig")->value;

	Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loaded PRIVATE");
}

void Client::SaveSettings() {
	if (savingSettings) return;
	else savingSettings = true;

	if (path.empty()) path = Utils::getConfigsPath() + "\\" + settings.getSettingByName<std::string>("currentConfig")->value;
	Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saving {}", Client::settings.getSettingByName<std::string>("currentConfig")->value);

	try {
		nlohmann::json configJson;
		
		for (const auto& [key, module] : ModuleManager::moduleMap) {
			if (module == nullptr) continue;
			
			try {
				std::string moduleSettingsStr = module->settings.ToJson();
				if (!moduleSettingsStr.empty()) {
					nlohmann::json moduleSettings = nlohmann::json::parse(moduleSettingsStr);
					configJson[module->name] = moduleSettings;
				}
			}
			catch (const nlohmann::json::parse_error& e) {
				LOG_ERROR("Failed to parse settings for module '{}': {}", module->name, e.what());
				continue;
			}
		}
		
		std::string tempPath = path + ".tmp";
		std::ofstream tempFile(tempPath, std::ofstream::out | std::ofstream::trunc);
		if (!tempFile.is_open()) {
			LOG_ERROR("Could not open temporary config file for writing");
			savingSettings = false;
			return;
		}
		
		tempFile << configJson.dump(2);
		tempFile.close();
		
		std::error_code ec;
		std::filesystem::rename(tempPath, path, ec);
		if (ec) {
			LOG_ERROR("Failed to rename temporary config file: {}", ec.message());
			std::filesystem::remove(tempPath, ec);
			savingSettings = false;
			return;
		}

		Logger::custom(fg(fmt::color::dark_magenta), "Config", "Saved {}", Client::settings.getSettingByName<std::string>("currentConfig")->value);
		savingSettings = false;
	}
	catch (const std::exception& e) {
		LOG_ERROR("An error occurred while trying to save settings: {}", e.what());
		savingSettings = false;
	}

	ScriptManager::saveSettings();
}

void Client::LoadSettings() {
	if (!settings.getSettingByName<std::string>("currentConfig")) return LOG_ERROR("No PRIVATE settings found");
	Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loading {}", Client::settings.getSettingByName<std::string>("currentConfig")->value);

	if (path.empty()) path = Utils::getConfigsPath() + "\\" + Client::settings.getSettingByName<std::string>("currentConfig")->value;

	std::ifstream inputFile(path);
	if (!inputFile) return LOG_ERROR("Config file could not be loaded {}", GetLastError());

	std::stringstream ss;
	ss << inputFile.rdbuf();
	inputFile.close();
	std::string str = ss.str();

	if (str.empty()) {
		Logger::warn("Settings String is empty");
		globalSettings.clear();
		return;
	}

	try { globalSettings = nlohmann::json::parse(str); }
	catch (const nlohmann::json::parse_error& e) {
		LOG_ERROR("Failed to parse JSON: {}", e.what());
	}

	Logger::custom(fg(fmt::color::dark_magenta), "Config", "Loaded {}", Client::settings.getSettingByName<std::string>("currentConfig")->value);
}

void Client::CheckSettingsFile() {
	if (!fs::exists(Utils::getConfigsPath())) fs::create_directory(Utils::getConfigsPath());

	if (!fs::exists(path)) {
		std::ofstream def(path, std::ios::app);
		if (!def) LOG_ERROR("Failed to create config: {}", GetLastError());
		else def.close();
	}

	if (!fs::exists(privatePath)) {
		std::ofstream priv(privatePath, std::ios::app);
		DWORD attributes = GetFileAttributesA(privatePath.c_str());
		if (!priv) LOG_ERROR("Failed to create PRIVATE: {}", GetLastError());
		else priv.close();
	}
	else Client::privateInit = true;
}
