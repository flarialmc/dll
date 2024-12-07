#include "WinrtUtils.hpp"

// Made by marioCST, ported by FreezeEngine

#include <winrt/Windows.ApplicationModel.h>

#include "../Utils.hpp"

Version WinrtUtils::getMCVersion() {
    static Version version;
        try {
            const winrt::Windows::ApplicationModel::Package package = winrt::Windows::ApplicationModel::Package::Current();
            auto [major, minor, build, revision] = package.Id().Version();

            version.major = major;
            version.minor = minor;
            version.build = build;
        }
        catch (const winrt::hresult_error& ex) {
            version.error = "Error: " + to_string(ex.message());
        }
    return version;
}

bool WinrtUtils::checkAboveOrEqual(const int m, const int b) {
    static auto [major, minor, build, error] = getMCVersion();
    if(m < minor) return true;
    return m <= minor && b <= build / 100;
}

bool WinrtUtils::checkBelowOrEqual(int m, int b) {
    static auto [major, minor, build, error] = getMCVersion();
    if(m < minor) return true;
    return m >= minor && b >= build / 100;
}

bool WinrtUtils::checkBetween(int m1, int b1, int m2, int b2) {
    return checkBelowOrEqual(m1, b1) && checkAboveOrEqual(m2, b2);
}

std::string WinrtUtils::getVersion() {
    static auto [major, minor, build, error] = getMCVersion();
    return error.empty() ? std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build) : error;
}

std::string WinrtUtils::getFormattedVersion() {
    const std::string &version = getVersion();

    if (version == "1.21.202") { // i cba for now
        // 1.21.202.0 - 1.21.2
        // 1.21.2003.0 - 1.21.20
        return "1.21.02";
    }

    if (version == "1.21.3") { // i cba for now #2
        // 1.21.3.0 - 1.21.0 ".3"
        // 1.21.3003.0 - 1.21.30
        return "1.21.03";
    }

    // Split version string by dots
    std::vector<std::string> parts;
    std::istringstream iss(version);
    std::string part;

    while (std::getline(iss, part, '.')) {
        parts.push_back(part);
    }

    // Ensure at least major and minor parts exist
    if (parts.size() < 2) {
        return version; // Invalid version format
    }

    // Construct formatted version string
    std::string formattedVersion = parts[0] + "." + parts[1];
    if (parts.size() > 2) {
        formattedVersion += "." + parts[2].substr(0, 1); // Take only the first character of the patch version
    }

    return formattedVersion;
}
