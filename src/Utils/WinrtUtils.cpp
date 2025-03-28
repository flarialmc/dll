#include "WinrtUtils.hpp"

#include <lib/json/json.hpp>

#include <Utils/Logger/Logger.hpp>
#include <Utils/Utils.hpp>
#include "../Client/GUI/Engine/Engine.hpp"

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.Data.Xml.Dom.h>

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

void WinrtUtils::setCursorType(winrt::Windows::UI::Core::CoreCursorType cursor) {
    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([cursor]() {
        auto window = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow();
        window.PointerCursor(winrt::Windows::UI::Core::CoreCursor(cursor, 0));
    });
}

void WinrtUtils::setWindowTitle(const std::string& title) {
    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([title]() {
        winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().Title(String::StrToWStr(title));
    });
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

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Storage::StorageFile>> WinrtUtils::pickFiles(std::wstring_view fileType) {
    using namespace winrt::Windows::Storage::Pickers;
    using namespace winrt::Windows::Storage;
    using namespace winrt::Windows::Foundation::Collections;

    FileOpenPicker picker;
    picker.SuggestedStartLocation(PickerLocationId::Downloads);
    picker.FileTypeFilter().Append(fileType);

    auto files = co_await picker.PickMultipleFilesAsync();

    auto result = winrt::single_threaded_vector<StorageFile>();
    for (auto const& file : files) {
        result.Append(file);
    }

    co_return result;
}

winrt::Windows::Foundation::IAsyncAction WinrtUtils::pickAndCopyFiles(std::wstring_view type = L"*", std::string path = "") {
    using namespace winrt::Windows::Storage;
    
    StorageFolder targetFolder = co_await StorageFolder::GetFolderFromPathAsync(FlarialGUI::to_wide(Utils::getClientPath() + path));
    auto pick = co_await WinrtUtils::pickFiles();

    winrt::Windows::Storage::StorageFile file = pick.Size() > 0 ? pick.GetAt(0) : nullptr;

    try {
        co_await file.CopyAsync(targetFolder, file.Name(), NameCollisionOption::ReplaceExisting);
    }
    catch (winrt::hresult_error const& ex) {
        Logger::error("Failed to copy file {}: {}", winrt::to_string(file.Name()), winrt::to_string(ex.message()));
    }

    co_return;
}


void WinrtUtils::launchURI(const std::string &uri) {
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::System;

    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([uri]() {
        Launcher::LaunchUriAsync(Uri(winrt::to_hstring(uri))).get();
    });
}

void WinrtUtils::openSubFolder(const std::string& subFolder) {
    using namespace winrt;
    using namespace winrt::Windows::Storage;
    using namespace winrt::Windows::System;

    try {
        StorageFolder roamingFolder = ApplicationData::Current().RoamingFolder();

        // Get the specified subfolder inside RoamingState
        auto folder = roamingFolder.GetFolderAsync(winrt::hstring(String::StrToWStr(subFolder))).get();

        // Launch the subfolder in File Explorer
        Launcher::LaunchFolderAsync(folder).get();
    } catch (const winrt::hresult_error& e) {
        Logger::error("An error occurred while trying to open {}: {} ({})", subFolder, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}

void WinrtUtils::setClipboard(const std::string& text) {
    using namespace winrt::Windows::ApplicationModel::DataTransfer;

    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([text]() {
        DataPackage dataPackage;
        dataPackage.SetText(winrt::to_hstring(text));
        Clipboard::SetContent(dataPackage);
    });
}

std::string WinrtUtils::getClipboard() {
    using namespace winrt::Windows::ApplicationModel::DataTransfer;

    try {
        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::Text())) {
            auto text = dataPackageView.GetTextAsync().get();
            return winrt::to_string(text);
        }
    } catch (const winrt::hresult_error& e) {
        Logger::error("Failed to get text from clipboard: {} ({})", winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
    return "";
}

void WinrtUtils::showMessageBox(const std::string& title, const std::string& message) {
    try {
        winrt::Windows::UI::Popups::MessageDialog dialog(winrt::to_hstring(message), winrt::to_hstring(title));
    dialog.ShowAsync();
    } catch (const winrt::hresult_error& e) {
        Logger::error("Failed to show message box {}: {} ({})", title, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}

void WinrtUtils::showNotification(const std::string& title, const std::string& message) {
    using namespace winrt::Windows::UI::Notifications;
    try {
        const auto notification = ToastNotification(ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastImageAndText02));

        winrt::Windows::Data::Xml::Dom::IXmlNodeList element = notification.Content().GetElementsByTagName(L"text");
        element.Item(0).InnerText(winrt::to_hstring(title));
        element.Item(1).InnerText(winrt::to_hstring(message));

        ToastNotificationManager::CreateToastNotifier().Show(notification);
    } catch (const winrt::hresult_error& e) {
        Logger::error("Failed to show notification {}: {} ({})", title, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}

