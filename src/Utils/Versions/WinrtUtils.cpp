#include "WinrtUtils.hpp"

// Made by marioCST, ported by FreezeEngine

#include <winrt/Windows.ApplicationModel.h>

#include "../Utils.hpp"

Version WinrtUtils::getMCVersion() {
    static Version version;

    static bool done = false;

    if (!done) {
        winrt::init_apartment();

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

        done = true;
    }

    return version;
}

bool WinrtUtils::check(const int m, const int b) {
    static auto [major, minor, build, error] = getMCVersion();
    if(m < minor) return true;
    return m <= minor && b <= build / 100;
}

std::string WinrtUtils::getVersion() {
    static auto [major, minor, build, error] = getMCVersion();
    return error.empty() ? std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(build) : error;
}

std::string WinrtUtils::getFormattedVersion() {
    const std::string& version = getVersion();
    size_t lastDotPos = version.find_last_of('.');
    if (lastDotPos != std::string::npos && version.size() - lastDotPos > 2) {
        // Found a dot before the last segment with more than one character after it
        return version.substr(0, lastDotPos + 3);
    } else {
        // No dot found or the last segment has only one or two characters
        return version;
    }
}
