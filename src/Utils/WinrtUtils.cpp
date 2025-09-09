#include "WinrtUtils.hpp"

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

#include "Modules/ClickGUI/ClickGUI.hpp"

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

winrt::Windows::UI::Core::CoreCursorType WinrtUtils::getCursorType() {
    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([]() {
        auto window = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow();
        return window.PointerCursor().Type();
    });
    return winrt::Windows::UI::Core::CoreCursorType::Arrow;
}

void WinrtUtils::setCursor(winrt::Windows::UI::Core::CoreCursor cursor) {
    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([cursor]() {
        auto window = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow();
        window.PointerCursor(cursor);
        WinrtUtils::currentCursorType = cursor.Type();
    });
}

void WinrtUtils::setCursorTypeThreaded(winrt::Windows::UI::Core::CoreCursorType cursor, int resId) {
    if (ModuleManager::getModule("ClickGUI")->active || ClickGUI::editmenu) {
        std::thread troll([cursor, resId]() { WinrtUtils::setCursor(winrt::Windows::UI::Core::CoreCursor(cursor, resId)); });
        troll.detach();
    }
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
        lastPart = lastPart.substr(0, 3); // Keep the first two digits
    }

    return parts[0] + "." + parts[1] + "." + lastPart;
}
namespace winrt
{
    using namespace Windows::Storage::Pickers;
    using namespace Windows::Storage;
    using namespace Windows::Foundation::Collections;
}

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Storage::StorageFile>> WinrtUtils::pickFiles(std::wstring_view fileType) {
    winrt::FileOpenPicker picker;
    picker.SuggestedStartLocation(winrt::PickerLocationId::Downloads);
    picker.FileTypeFilter().Append(fileType);

    auto files = co_await picker.PickMultipleFilesAsync();

    auto result = winrt::single_threaded_vector<winrt::StorageFile>();
    for (auto const& file : files) {
        result.Append(file);
    }

    co_return result;
}
namespace winrt
{
    using namespace Windows::Storage;
}

winrt::Windows::Foundation::IAsyncAction WinrtUtils::pickAndCopyFiles(std::wstring_view type = L"*", std::string path = "") {
    winrt::StorageFolder targetFolder = co_await winrt::StorageFolder::GetFolderFromPathAsync(FlarialGUI::to_wide(Utils::getClientPath() + path));
    auto pick = co_await WinrtUtils::pickFiles();

    winrt::Windows::Storage::StorageFile file = pick.Size() > 0 ? pick.GetAt(0) : nullptr;

    try {
        co_await file.CopyAsync(targetFolder, file.Name(), winrt::NameCollisionOption::ReplaceExisting);
    }
    catch (winrt::hresult_error const& ex) {
        LOG_ERROR("Failed to copy file {}: {}", winrt::to_string(file.Name()), winrt::to_string(ex.message()));
    }

    co_return;
}

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::System;
}

void WinrtUtils::launchURI(const std::string &uri) {


    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([uri]() {
        winrt::Launcher::LaunchUriAsync(winrt::Uri(winrt::to_hstring(uri))).get();
    });
}
namespace winrt
{
    using namespace Windows::Storage;
    using namespace Windows::System;
}

void WinrtUtils::openSubFolder(const std::string& subFolder) {


    try {
        winrt::StorageFolder roamingFolder = winrt::ApplicationData::Current().RoamingFolder();

        // Get the specified subfolder inside RoamingState
        auto folder = roamingFolder.GetFolderAsync(winrt::hstring(String::StrToWStr(subFolder))).get();

        // Launch the subfolder in File Explorer
        winrt::Launcher::LaunchFolderAsync(folder).get();
    } catch (const winrt::hresult_error& e) {
        LOG_ERROR("An error occurred while trying to open {}: {} ({})", subFolder, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}

namespace winrt
{
    using namespace Windows::ApplicationModel::DataTransfer;
}

void WinrtUtils::setClipboard(const std::string& text) {

    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([text]() {
        winrt::DataPackage dataPackage;
        dataPackage.SetText(winrt::to_hstring(text));
        winrt::Clipboard::SetContent(dataPackage);
    });
}

namespace winrt
{
    using namespace Windows::ApplicationModel::DataTransfer;
}

std::string WinrtUtils::getClipboard() {

    try {
        auto dataPackageView = winrt::Clipboard::GetContent();
        if (dataPackageView.Contains(winrt::StandardDataFormats::Text())) {
            auto text = dataPackageView.GetTextAsync().get();
            return winrt::to_string(text);
        }
    } catch (const winrt::hresult_error& e) {
        LOG_ERROR("Failed to get text from clipboard: {} ({})", winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
    return "";
}

void WinrtUtils::showMessageBox(const std::string& title, const std::string& message) {
    try {
        winrt::Windows::UI::Popups::MessageDialog dialog(winrt::to_hstring(message), winrt::to_hstring(title));
    dialog.ShowAsync();
    } catch (const winrt::hresult_error& e) {
        LOG_ERROR("Failed to show message box {}: {} ({})", title, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}
namespace winrt
{
    using namespace Windows::UI::Notifications;
}

void WinrtUtils::showNotification(const std::string& title, const std::string& message) {
    try {
        const auto notification = winrt::ToastNotification(winrt::ToastNotificationManager::GetTemplateContent(winrt::ToastTemplateType::ToastImageAndText02));

        winrt::Windows::Data::Xml::Dom::IXmlNodeList element = notification.Content().GetElementsByTagName(L"text");
        element.Item(0).InnerText(winrt::to_hstring(title));
        element.Item(1).InnerText(winrt::to_hstring(message));

        winrt::ToastNotificationManager::CreateToastNotifier().Show(notification);
    } catch (const winrt::hresult_error& e) {
        LOG_ERROR("Failed to show notification {}: {} ({})", title, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
    }
}

