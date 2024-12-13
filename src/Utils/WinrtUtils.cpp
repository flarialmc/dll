#include "WinrtUtils.hpp"

#include <lib/json/json.hpp>

#include <Utils/Logger/Logger.hpp>
#include <Utils/Utils.hpp>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>

void WinrtUtils::setWindowTitle(const std::string& title) {
    CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([title]() {
        ApplicationView::GetForCurrentView().Title(String::StrToWStr(title));
    });
}

void WinrtUtils::setCursorType(CoreCursorType cursor) {
    CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([cursor]() {
        auto window = CoreApplication::MainView().CoreWindow();
        window.PointerCursor(CoreCursor(cursor, 0));
    });
}

void WinrtUtils::openFolder(const std::string& path) {
    using namespace winrt;
    using namespace Windows::Foundation;
    using namespace Windows::Storage;
    using namespace Windows::System;

    try {
        hstring hFolderPath = to_hstring(path);

        auto folder = StorageFolder::GetFolderFromPathAsync(hFolderPath).get();

        // Launch the folder in File Explorer
        Launcher::LaunchFolderAsync(folder).get();
    } catch (const winrt::hresult_error& e) {
        Logger::error("An error occurred while trying to open {}: {} ({})", path, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}

void WinrtUtils::openSubFolder(const std::string& path) {
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

Version WinrtUtils::impl::getGameVersion() {
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

std::string WinrtUtils::impl::toRawString(const Version &version) {
    if (!version.error.empty()) {
        return version.error;
    }

    std::ostringstream oss;
    oss << version.major << "." << version.minor << "." << version.build;
    return oss.str();
}

std::string WinrtUtils::getFormattedVersion() {
    std::string version = impl::toRawString(impl::getGameVersion());

    std::vector<std::string> parts;
    std::stringstream ss(version);
    std::string part;

    // Split the version string by '.' and store parts
    while (std::getline(ss, part, '.')) {
        parts.push_back(part);
    }

    // Ensure the version has at least three parts
    if (parts.size() < 3) return version;

    std::string lastPart = parts[2];

    // Process the last part based on its length
    if (lastPart.length() == 3) {
        lastPart = lastPart.substr(0, 1); // Keep only the first digit
    } else if (lastPart.length() > 3) {
        lastPart = lastPart.substr(0, 2); // Keep the first two digits
    }

    return parts[0] + "." + parts[1] + "." + lastPart;
}

int WinrtUtils::getRawGameVersion() {
    Version currentVersion = impl::getGameVersion();
    std::string rawString = impl::toRawString(currentVersion);

    rawString.erase(std::ranges::remove(rawString, '.').begin(), rawString.end());

    try {
        return std::stoi(rawString);
    } catch (...) {
        return -1;
    }
}