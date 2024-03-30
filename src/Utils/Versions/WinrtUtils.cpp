#include "WinrtUtils.hpp"
#include "../Utils.hpp"
#include <winrt/Windows.ApplicationModel.h>

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
    return m <= minor && b <= build / 100;
}

std::string WinrtUtils::getVersion() {
    static auto [major, minor, build, error] = getMCVersion();
    return error.empty() ? std::to_string(major)+"."+std::to_string(minor)+"."+std::to_string(build) : error;
}

std::string WinrtUtils::getFormattedVersion() {
    static const std::string& version = getVersion();
    return version.substr(0, version.size() - 3);
}
