#include "Utils.hpp"

#include <Client/GUI/Engine/Engine.hpp>
#include <Utils/WinrtUtils.hpp>

#include <sstream>
#include <algorithm>
#include <codecvt>
#include <Psapi.h>
#include <regex>
#include <shared_mutex>
#include <wininet.h>
#include <miniz/miniz.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>

std::string Utils::getRoamingPath() {
	using namespace winrt::Windows::Storage;

	std::filesystem::path path(ApplicationData::Current().RoamingFolder().Path().c_str());

	return path.string();
}

std::string Utils::getClientPath() {
	std::filesystem::path path(Utils::getRoamingPath() + "\\Flarial");
	return path.string();
}

std::string Utils::getConfigsPath() {
	std::filesystem::path path(Utils::getRoamingPath() + "\\Flarial\\Config");
	return path.string();
}

void Utils::MessageDialogW(PCWSTR pText, PCWSTR pTitle)
{
	auto dispatcher = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView()
		.CoreWindow()
		.Dispatcher();

	dispatcher.TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,
		[pText, pTitle]() {
			winrt::Windows::UI::Popups::MessageDialog dialog(pText, pTitle ? pTitle : L"");
			dialog.ShowAsync();
		});
}

std::string Utils::getAssetsPath() {
	std::filesystem::path path(Utils::getRoamingPath() + "\\Flarial\\assets");
	return path.string();
}

std::string Utils::getLogsPath() {
	std::filesystem::path path(Utils::getRoamingPath() + "\\Flarial\\logs");
	return path.string();
}

std::string Utils::getKeyAsString(int key, bool isCapital, bool isKeybind) {
	if (key == 32) { // Spacebar key
		return " ";
	}

	// extra
	switch (key) {
	case 37: return "LEFT";
	case 38: return "UP";
	case 39: return "RIGHT";
	case 40: return "DOWN";
	case 97: return "NUM 1";
	case 98: return "NUM 2";
	case 99: return "NUM 3";
	case 100: return "NUM 4";
	case 101: return "NUM 5";
	case 102: return "NUM 6";
	case 103: return "NUM 7";
	case 104: return "NUM 8";
	case 105: return "NUM 9";
	case 106: return "NUM *";
	case 107: return "NUM +";
	case 109: return "NUM -";
	case 110: return "NUM .";
	case 111: return "NUM /";
	}

	if (isKeybind) {
		switch (key) {
		case 18:
			return "ALT";
		case 17:
			return "CTRL";
		case 16:
			return "SHIFT";
		case 20:
			return "CAPS LOCK";
		case 9:
			return "TAB";
		case 45:
			return "INS";
		case 46:
			return "DEL";
		case 36:
			return "HOME";
		case 35:
			return "END";
		case 33:
			return "PG UP";
		case 34:
			return "PG DOWN";
		case 112:
			return "F1";
		case 113:
			return "F2";
		case 114:
			return "F3";
		case 115:
			return "F4";
		case 116:
			return "F5";
		case 117:
			return "F6";
		case 118:
			return "F7";
		case 119:
			return "F8";
		case 120:
			return "F9";
		case 121:
			return "F10";
		case 122:
			return "F11";
		case 123:
			return "F12";
		case 144:
			return "NUM LOCK";
		default:
			break;
		}
	}

	if (isCapital) {
		switch (key) {
			// A - Z
		case 48:
			return "0";
		case 49:
			return "1";
		case 50:
			return "2";
		case 51:
			return "3";
		case 52:
			return "4";
		case 53:
			return "5";
		case 54:
			return "6";
		case 55:
			return "7";
		case 56:
			return "8";
		case 57:
			return "9";
		case 65:
			return "A";
		case 66:
			return "B";
		case 67:
			return "C";
		case 68:
			return "D";
		case 69:
			return "E";
		case 70:
			return "F";
		case 71:
			return "G";
		case 72:
			return "H";
		case 73:
			return "I";
		case 74:
			return "J";
		case 75:
			return "K";
		case 76:
			return "L";
		case 77:
			return "M";
		case 78:
			return "N";
		case 79:
			return "O";
		case 80:
			return "P";
		case 81:
			return "Q";
		case 82:
			return "R";
		case 83:
			return "S";
		case 84:
			return "T";
		case 85:
			return "U";
		case 86:
			return "V";
		case 87:
			return "W";
		case 88:
			return "X";
		case 89:
			return "Y";
		case 90:
			return "Z";
		case 188:
			return "<";
		case 190:
			return ">";
		case 192:
			return "~";
		case 40:
			return "(";
		case 41:
			return ")";
		case 37:
			return "%";
		case 91:
			return "{";
		case 93:
			return "}";
		case 220:
			return "|";
		case 191:
			return "?";
		case 186:
			return ":";
		case 187:
			return "+";
		case 219:
			return "{";
		case 221:
			return "}";
		default:
			return "";
		}
	}
	else {
		switch (key) {
			// a - z
		case 48:
			return "0";
		case 49:
			return "1";
		case 50:
			return "2";
		case 51:
			return "3";
		case 52:
			return "4";
		case 53:
			return "5";
		case 54:
			return "6";
		case 55:
			return "7";
		case 56:
			return "8";
		case 57:
			return "9";
		case 65:
			return "a";
		case 66:
			return "b";
		case 67:
			return "c";
		case 68:
			return "d";
		case 69:
			return "e";
		case 70:
			return "f";
		case 71:
			return "g";
		case 72:
			return "h";
		case 73:
			return "i";
		case 74:
			return "j";
		case 75:
			return "k";
		case 76:
			return "l";
		case 77:
			return "m";
		case 78:
			return "n";
		case 79:
			return "o";
		case 80:
			return "p";
		case 81:
			return "q";
		case 82:
			return "r";
		case 83:
			return "s";
		case 84:
			return "t";
		case 85:
			return "u";
		case 86:
			return "v";
		case 87:
			return "w";
		case 88:
			return "x";
		case 89:
			return "y";
		case 90:
			return "z";
		case 188:
			return ",";
		case 190:
			return ".";
		case 192:
			return "`";
		case 219:
			return "[";
		case 221:
			return "]";
		case 220:
			return "\\";
		case 191:
			return "/";
		case 187:
			return "=";
		case 189:
			return "-";
		case 186:
			return (std::string)";";
		default:
			return "";
		}
	}
}

int Utils::getStringAsKey(const std::string& str) {
	if (str == "CAPS LOCK" || str == "caps lock") return 20;
	if (str == "NUM LOCK" || str == "num lock") return 144;

	if (str == "LEFT" || str == "left") return 37;
	if (str == "UP" || str == "up") return 38;
	if (str == "RIGHT" || str == "right") return 39;
	if (str == "DOWN" || str == "down") return 40;
	if (str == "NUM 1" || str == "num 1") return 97;
	if (str == "NUM 2" || str == "num 2") return 98;
	if (str == "NUM 3" || str == "num 3") return 99;
	if (str == "NUM 4" || str == "num 4") return 100;
	if (str == "NUM 5" || str == "num 5") return 101;
	if (str == "NUM 6" || str == "num 6") return 102;
	if (str == "NUM 7" || str == "num 7") return 103;
	if (str == "NUM 8" || str == "num 8") return 104;
	if (str == "NUM 9" || str == "num 9") return 105;
	if (str == "NUM *" || str == "num *") return 106;
	if (str == "NUM +" || str == "num +") return 107;
	if (str == "NUM -" || str == "num -") return 109;
	if (str == "NUM ." || str == "num .") return 110;
	if (str == "NUM /" || str == "num /") return 111;
	if (str == "ALT" || str == "alt") return 18;
	if (str == "CTRL" || str == "ctrl") return 17;
	if (str == "SHIFT" || str == "shift") return 16;
	if (str == "INS" || str == "ins") return 45;
	if (str == "DEL" || str == "del") return 46;
	if (str == "HOME" || str == "home") return 36;
	if (str == "PG UP" || str == "pg up") return 33;
	if (str == "PG DOWN" || str == "pg down") return 34;
	if (str == "TAB") return 9;
	if (str == "-") return 189;
	if (str == "[" || str == "{") return 219;  // [ and { share the same key
	if (str == "]" || str == "}") return 221;  // ] and } share the same key
	if (str == "+" || str == "=") return 187;  // + and = share the same key
	if (str == ";" || str == ":") return 186;  // ; and : share the same key
	if (str == "/" || str == "?") return 191;  // / and ? share the same key
	if (str == "\\" || str == "|") return 220; // \ and | share the same key
	if (str == "`" || str == "~") return 192;  // ` and ~ share the same key
	if (str == "'") return 222;                // ' and " share the same key
	if (str == "\"") return 222;               // " detected as 222 too
	if (str == " ") return 32;
	if (str == "F1" || str == "f1") return 112;
	if (str == "F2" || str == "f2") return 113;
	if (str == "F3" || str == "f3") return 114;
	if (str == "F4" || str == "f4") return 115;
	if (str == "F5" || str == "f5") return 116;
	if (str == "F6" || str == "f6") return 117;
	if (str == "F7" || str == "f7") return 118;
	if (str == "F8" || str == "f8") return 119;
	if (str == "F9" || str == "f9") return 120;
	if (str == "F10" || str == "f10") return 121;
	if (str == "F11" || str == "f11") return 122;
	if (str == "F12" || str == "f12") return 123;

	if (str.length() == 1) {
		char c = str[0];
		return static_cast<int>(std::toupper(c));
	}

	return -1;
}


std::vector<int> Utils::getStringAsKeys(const std::string& str) {
	std::vector<int> keys;

	if (str.empty()) {
		// Empty string
		return keys;
	}

	std::stringstream iss(str);
	std::string token;
	while (std::getline(iss, token, '+')) {
		int key = Utils::getStringAsKey(token);
		if (key != -1) {
			keys.push_back(key);
		}
	}

	return keys;
}

bool Utils::CursorInEllipse(float ellipseX, float ellipseY, float radiusX, float radiusY) {
	float mouseX = MC::mousePos.x;
	float mouseY = MC::mousePos.y;

	float normalizedX = (mouseX - ellipseX) / radiusX;
	float normalizedY = (mouseY - ellipseY) / radiusY;

	return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0f;
}

int Utils::CountBytes(const std::string& data) {
	std::istringstream iss(data);
	int count = 0;
	std::string byte;

	while (iss >> byte) {
		count++;
	}

	return count;
}

uint64_t Utils::getCurrentMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch())
		.count();
}

void Utils::extractFromFile(const std::string& zipFilePath, const std::string& destinationFolder) {
	mz_zip_archive zip_archive = {};

	if (!mz_zip_reader_init_file(&zip_archive, zipFilePath.c_str(), 0)) {
		Logger::error("Failed to open zip file: {}", zipFilePath);
		return;
	}

	std::string baseDir = destinationFolder;
	if (!baseDir.empty() && baseDir.back() != '\\') {
		baseDir.push_back('\\');
	}

	int file_count = static_cast<int>(mz_zip_reader_get_num_files(&zip_archive));
	for (int i = 0; i < file_count; i++) {
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
			// Could not get file info for this entry, skip it
			continue;
		}

		std::string filename(file_stat.m_filename);

		if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
			std::filesystem::create_directories(baseDir + filename);
		}
		else {
			// Not a directory aka a file
			std::filesystem::path fullPath = baseDir + filename;
			std::filesystem::create_directories(fullPath.parent_path());

			mz_zip_reader_extract_to_file(&zip_archive, i, fullPath.string().c_str(), 0);
		}
	}
	mz_zip_reader_end(&zip_archive);
}

std::string Utils::downloadFile(const std::string& url) {
	auto internetCloser = [](HINTERNET handle) { if (handle) InternetCloseHandle(handle); };

	std::unique_ptr<void, decltype(internetCloser)> interwebs(
		InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL),
		internetCloser
	);

	if (!interwebs) {
		return "";
	}

	DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI | INTERNET_FLAG_SECURE;
	std::unique_ptr<void, decltype(internetCloser)> urlFile(
		InternetOpenUrlA(interwebs.get(), url.c_str(), NULL, 0, flags, 0),
		internetCloser
	);

	if (!urlFile) {
		return "";
	}

	DWORD statusCode = 0;
	DWORD statusCodeSize = sizeof(statusCode);
	if (!HttpQueryInfo(urlFile.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL) || statusCode != 200) {
		return "";
	}

	std::string rtn;
	rtn.reserve(8192);
	char buffer[8192];
	DWORD bytesRead;

	while (InternetReadFile(urlFile.get(), buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
		rtn.append(buffer, bytesRead);
	}

	return rtn;
}

std::string Utils::sanitizeName(const std::string& name) {
	std::string cleanedName = String::removeNonAlphanumeric(String::removeColorCodes(name));
	if (cleanedName.empty()) {
		cleanedName = name;
	}
	return cleanedName;
}


std::string String::replaceAll(std::string& string, std::string_view c1, std::string_view c2) {
	size_t pos = 0;
	while ((pos = string.find(c1, pos)) != std::string::npos) {
		string.replace(pos, c1.length(), c2);
		pos += c2.length();
	}
	return string;
}

bool String::find(const std::string& string, const std::string& find) {
	auto it = std::search(
		string.begin(), string.end(),
		find.begin(), find.end(),
		[](char ch1, char ch2) {
			return std::tolower(static_cast<unsigned char>(ch1)) == std::tolower(static_cast<unsigned char>(ch2));
		}
	);
	return it != string.end();
}

bool String::hasEnding(const std::string& string, const std::string& ending) {
	size_t stringLength = string.length();
	size_t endingLength = ending.length();

	if (stringLength < endingLength) {
		return false;
	}
	return string.compare(stringLength - endingLength, endingLength, ending) == 0;
}

std::wstring String::StrToWStr(const std::string& s) {
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), static_cast<int>(s.length()), nullptr, 0);
	if (len == 0) {
		return L"";
	}

	std::wstring result(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), static_cast<int>(s.length()), &result[0], len);

	return result;
}

std::string String::WStrToStr(const std::wstring& ws) {
	int ws_len = static_cast<int>(ws.size());
	int len = WideCharToMultiByte(CP_UTF8, 0, ws.data(), ws_len, NULL, 0, NULL, NULL);

	if (len == 0) {
		return "";
	}

	std::string ret(len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, ws.data(), ws_len, &ret[0], len, NULL, NULL);

	return ret;
}

std::string String::removeColorCodes(const std::string& input) {
	// Static cache and mutex for thread-safe caching.
	static std::unordered_map<std::string, std::string> cache;
	static std::shared_mutex cacheMutex;

	{ // First, attempt a shared (read) lock.
		std::shared_lock lock(cacheMutex);
		auto it = cache.find(input);
		if (it != cache.end()) {
			return it->second;
		}
	}

	std::string result;
	result.reserve(input.size());

	bool skipNext = false;
	for (size_t i = 0; i < input.size();) {
		if (skipNext) {
			skipNext = false;
			++i;
		}
		else if (input[i] == '\xC2' && i + 1 < input.size() && input[i + 1] == '\xA7') {
			skipNext = true;
			i += 2;
		}
		else {
			if ((input[i] & 0xC0) == 0xC0) {
				size_t bytesLeft = 0;
				while (i + bytesLeft < input.size() && (input[i + bytesLeft] & 0xC0) == 0x80) {
					++bytesLeft;
				}
				result.append(input, i, bytesLeft + 1);
				i += bytesLeft + 1;
			}
			else {
				result.push_back(input[i]);
				++i;
			}
		}
	}

	{
		std::unique_lock lock(cacheMutex);
		cache[input] = result;
	}
	return result;
}

std::string String::removeNonAlphanumeric(const std::string& input) {
	// Static cache and mutex for thread-safe caching.
	static std::unordered_map<std::string, std::string> cache;
	static std::shared_mutex cacheMutex;

	{
		std::shared_lock lock(cacheMutex);
		auto it = cache.find(input);
		if (it != cache.end()) {
			return it->second;
		}
	}

	static const std::regex pattern("[A-Za-z][A-Za-z0-9 ]{2,16}");
	std::smatch match;
	std::string nickname;
	if (std::regex_search(input, match, pattern)) {
		nickname = match.str();
		// Remove trailing spaces.
		nickname.erase(nickname.find_last_not_of(" ") + 1);
	}

	{
		std::unique_lock lock(cacheMutex);
		cache[input] = nickname;
	}
	return nickname;
}

std::string String::removeNonNumeric(const std::string& string) {
	std::string result;
	result.reserve(string.size());

	for (char c : string) {
		if ((c >= '0' && c <= '9') || c == '.') {
			result += c;
		}
	}
	return result;
}

std::vector<std::string> String::split(std::string_view str, char delimiter) {
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = 0;

	while ((end = str.find(delimiter, start)) != std::string::npos) {
		result.emplace_back(str.substr(start, end - start));
		start = end + 1;
	}

	result.emplace_back(str.substr(start));

	return result;
}

std::string String::toLower(std::string input) {
	std::ranges::transform(input, input.begin(), ::tolower);
	return input;
}

std::string String::toUpper(std::string input) {
	std::ranges::transform(input, input.begin(), ::toupper);
	return input;
}
