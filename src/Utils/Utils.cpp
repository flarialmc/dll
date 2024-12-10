#include "Utils.hpp"

#include "../Client/GUI/Engine/Engine.hpp"
#include <sstream>
#include <algorithm>
#include <codecvt>
#include <Psapi.h>
#include <regex>
#include <wininet.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>

std::string Utils::getRoamingPath() {
    char *path = nullptr;
    size_t length;

    _dupenv_s(&path, &length, "appdata");

    if (path == nullptr)
        return "";

    return std::string(path) + R"(\..\Local\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\RoamingState)"; // Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe Microsoft.MinecraftUWP_8wekyb3d8bbwe
}

std::string Utils::getClientPath() {
    return Utils::getRoamingPath() + "\\Flarial";
}

std::string Utils::getConfigsPath() {
    return Utils::getRoamingPath() + "\\Flarial\\Config";
}

std::string Utils::getAssetsPath() {
    return Utils::getRoamingPath() + "\\Flarial\\Assets";
}

std::string Utils::getLogsPath() {
    return Utils::getRoamingPath() + "\\Flarial\\Logs";
}

std::string Utils::getKeyAsString(int key, bool isCapital, bool isKeybind) {
    if (key == 32) { // Spacebar key
        return " ";
    }

    if (isKeybind) {
        switch (key) {
            case 18:
                return "ALT";
            case 17:
                return "CTRL";
            case 16:
                return "SHIFT";
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
    } else {
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
                return (std::string) ";";
            default:
                return "";
        }
    }
}

int Utils::getStringAsKey(const std::string &str) {
    if (str == "ALT" || str == "alt") return 18;
    if (str == "CTRL" || str == "ctrl") return 17;
    if (str == "SHIFT" || str == "shift") return 16;
    if (str == "INS" || str == "ins") return 45;
    if (str == "DEL" || str == "del") return 46;
    if (str == "HOME" || str == "home") return 36;
    if (str == "PG UP" || str == "pg up") return 33;
    if (str == "PG DOWN" || str == "pg down") return 34;
    if (str == "TAB") return 9;
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

bool Utils::isMinecraftLoaded(HANDLE process) {
    int modulesNeeded = 176;
    HMODULE modules[1024];
    DWORD cbNeeded;

    if (EnumProcessModules(process, modules, sizeof(modules), &cbNeeded)) {
        int moduleCount = cbNeeded / sizeof(HMODULE);
        if (moduleCount >= modulesNeeded) {
            return true;
        }
    }
    return false;
}

uint64_t Utils::getCurrentMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}
std::string Utils::DownloadString(const std::string& url) {
    HINTERNET interwebs = InternetOpenA("Samsung Smart Fridge", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!interwebs) {
        return "";
    }

    std::string rtn;
    HINTERNET urlFile = InternetOpenUrlA(interwebs, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (urlFile) {
        char buffer[2000];
        DWORD bytesRead;
        while (InternetReadFile(urlFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            rtn.append(buffer, bytesRead);
        }
        InternetCloseHandle(urlFile);
    }

    InternetCloseHandle(interwebs);
    return String::replaceAll(rtn, "|n", "\r\n");
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

void Utils::openFolder(const std::string& path) {
    using namespace winrt;
    using namespace Windows::Foundation;
    using namespace Windows::Storage;
    using namespace Windows::System;

    try {
        hstring hFolderPath = to_hstring(path);

        StorageFolder folder = StorageFolder::GetFolderFromPathAsync(hFolderPath).get();

        // Launch the folder in File Explorer
        Launcher::LaunchFolderAsync(folder).get();
    } catch (const winrt::hresult_error& e) {
        Logger::error("An error occurred while trying to open {}: {} ({})", path, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}

void Utils::openSubFolder(const std::string& path) {
    using namespace winrt;
    using namespace Windows::Storage;
    using namespace Windows::System;

    try {
        StorageFolder roamingFolder = ApplicationData::Current().RoamingFolder();

        // Get the specified subfolder inside RoamingState
        auto folder = roamingFolder.GetFolderAsync(winrt::hstring(String::StrToWStr(path))).get();

        // Launch the subfolder in File Explorer
        Launcher::LaunchFolderAsync(folder).get();
    } catch (const winrt::hresult_error& e) {
        Logger::error("An error occurred while trying to open {}: {} ({})", path, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
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

std::string String::removeColorCodes(const std::string &input) {
    std::string result;

    bool skipNext = false;
    for (size_t i = 0; i < input.size();) {
        if (skipNext) {
            skipNext = false;
            ++i;
        } else if (input[i] == '\xC2' && i + 1 < input.size() && input[i + 1] == '\xA7') {
            skipNext = true;
            i += 2;
        } else {
            if ((input[i] & 0xC0) == 0xC0) { // UTF-8 continuation byte
                size_t bytesLeft = 0;
                while ((input[i + bytesLeft] & 0xC0) == 0x80) {
                    ++bytesLeft;
                }
                result.append(input, i, bytesLeft + 1);
                i += bytesLeft + 1;
            } else {
                result += input[i];
                ++i;
            }
        }
    }

    return result;
}

std::string String::removeNonAlphanumeric(const std::string &input) {
    std::regex pattern("[A-Za-z][A-Za-z0-9 ]{2,16}");
    std::smatch match;
    if (std::regex_search(input, match, pattern)) {
        std::string nickname = match.str();
        // Remove trailing spaces
        nickname.erase(nickname.find_last_not_of(" ") + 1);
        return nickname;
    } else {
        return "";
    }
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
