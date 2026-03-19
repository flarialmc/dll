#include "WinrtUtils.hpp"
#include "PlatformUtils.hpp"
#include "ShellMessageUtil.hpp"

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

#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>

#include "Modules/ClickGUI/ClickGUI.hpp"

static std::mutex s_uiMutex;
static std::atomic_bool s_handlesCached{false};
static winrt::Windows::UI::Core::CoreDispatcher s_coreDispatcher{ nullptr };
static winrt::Windows::UI::Core::CoreWindow    s_coreWindow{ nullptr };
static std::atomic_bool g_uiCached{ false };

static std::atomic<winrt::Windows::UI::Core::CoreCursorType> g_requestedCursor{
    winrt::Windows::UI::Core::CoreCursorType::Arrow
};

static std::optional<winrt::Windows::UI::Core::CoreCursorType> g_lastCursor;
static winrt::event_token g_pointerMovedToken{};

Version WinrtUtils::impl::getGameVersion() {
    static Version version;

    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);

    DWORD filler;
    const DWORD size = GetFileVersionInfoSizeA(path, &filler);
    if (size == 0)
    {
        return version;
    }

    std::vector<char> versionInfo(size);
    if (!GetFileVersionInfoA(path, 0, size, versionInfo.data()))
    {
        return version;
    }

    VS_FIXEDFILEINFO* fileInfo;
    UINT length;
    if (!VerQueryValueA(versionInfo.data(), ("\\"), reinterpret_cast<void**>(&fileInfo), &length))
    {
        return version;
    }

    version.major = HIWORD(fileInfo->dwFileVersionMS);
    version.minor = LOWORD(fileInfo->dwFileVersionMS);
    version.build = HIWORD(fileInfo->dwFileVersionLS);

    Logger::debug(PlatformUtils::getPlatformString());
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
    try {
        auto dispatcher = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue();
        dispatcher.TryEnqueue([cursor]() {
            try {
                auto window = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow();
                window.PointerCursor(cursor);
                WinrtUtils::currentCursorType.store(cursor.Type(), std::memory_order_relaxed);
            } catch (...) {}
        });
    } catch (...) {}
}

// completely broke this. if you know a fix, cool.
void WinrtUtils::setCursorTypeThreaded(
    winrt::Windows::UI::Core::CoreCursorType cursor,
    int resId
) {
    if (PlatformUtils::isUWP()) {
        g_requestedCursor.store(cursor, std::memory_order_relaxed);
        return;
    }

    if (g_lastCursor && *g_lastCursor == cursor) return;
    g_lastCursor = cursor;
    currentCursorType.store(cursor, std::memory_order_relaxed); // Always track requested cursor type for GDK

    if (!g_uiCached.load(std::memory_order_acquire)) return;

    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    winrt::Windows::UI::Core::CoreWindow window{nullptr};
    {
        std::lock_guard lock(s_uiMutex);
        dispatcher = s_coreDispatcher;
        window = s_coreWindow;
    }

    if (!dispatcher) return;

    dispatcher.RunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
        [window, cursor, resId]() {
            try {
                window.PointerCursor(
                    winrt::Windows::UI::Core::CoreCursor(cursor, resId)
                );
                currentCursorType.store(cursor, std::memory_order_relaxed);
            } catch (...)
            {}
        }
    );
}

void WinrtUtils::setWindowTitle(const std::string& title) {
    if (PlatformUtils::isUWP()) {
        // UWP: Use ApplicationView API
        try {
            winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([title]() {
                try {
                    winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().Title(String::StrToWStr(title));
                } catch (const winrt::hresult_error& e) {
                    LOG_ERROR("Failed to set UWP window title: {} (0x{:08X})", winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
                }
            });
        } catch (const winrt::hresult_error& e) {
            LOG_ERROR("Failed to enqueue window title change: {} (0x{:08X})", winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
        } catch (const std::exception& e) {
            LOG_ERROR("Exception setting UWP window title: {}", e.what());
        }
    }
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
    auto result = winrt::single_threaded_vector<winrt::StorageFile>();

    if (PlatformUtils::isUWP()) {
        // UWP: Use native WinRT file picker
        try {
            winrt::FileOpenPicker picker;
            picker.SuggestedStartLocation(winrt::PickerLocationId::Downloads);
            picker.FileTypeFilter().Append(fileType);

            auto files = co_await picker.PickMultipleFilesAsync();

            for (auto const& file : files) {
                result.Append(file);
            }
        } catch (const winrt::hresult_error& e) {
            LOG_ERROR("UWP file picker failed: {} ({})", winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
        }
    } else {
        // GDK: Use Win32 file dialog and convert to StorageFile
        OPENFILENAMEW ofn;
        wchar_t szFile[4096] = { 0 }; // Buffer for multiple files

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);

        // Convert filter format from ".ext" to "All Files\0*.ext\0"
        std::wstring filterStr = L"Selected Files\0*";
        filterStr += fileType;
        filterStr += L"\0\0";
        ofn.lpstrFilter = filterStr.c_str();

        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

        if (GetOpenFileNameW(&ofn) == TRUE) {
            // Parse multiple file selection
            std::wstring directory = szFile;
            wchar_t* pFile = szFile + directory.length() + 1;

            if (*pFile == 0) {
                // Single file selected
                try {
                    auto file = co_await winrt::StorageFile::GetFileFromPathAsync(directory);
                    result.Append(file);
                } catch (const winrt::hresult_error& e) {
                    LOG_ERROR("Failed to get StorageFile from path {}: {}", winrt::to_string(directory), winrt::to_string(e.message()));
                }
            } else {
                // Multiple files selected
                while (*pFile) {
                    std::wstring fullPath = directory + L"\\" + pFile;
                    try {
                        auto file = co_await winrt::StorageFile::GetFileFromPathAsync(fullPath);
                        result.Append(file);
                    } catch (const winrt::hresult_error& e) {
                        LOG_ERROR("Failed to get StorageFile from path {}: {}", winrt::to_string(fullPath), winrt::to_string(e.message()));
                    }
                    pFile += wcslen(pFile) + 1;
                }
            }
        }
    }

    co_return result;
}
namespace winrt
{
    using namespace Windows::Storage;
}

winrt::Windows::Foundation::IAsyncAction WinrtUtils::pickAndCopyFiles(std::wstring_view type = L"*", std::string path = "") {
    try {
        winrt::StorageFolder targetFolder = co_await winrt::StorageFolder::GetFolderFromPathAsync(FlarialGUI::to_wide(Utils::getClientPath() + path));
        auto pick = co_await WinrtUtils::pickFiles(type);

        if (pick.Size() == 0) {
            Logger::warn("No files selected");
            co_return;
        }

        winrt::Windows::Storage::StorageFile file = pick.GetAt(0);

        try {
            co_await file.CopyAsync(targetFolder, file.Name(), winrt::NameCollisionOption::ReplaceExisting);
            Logger::success("Copied file: {}", winrt::to_string(file.Name()));
        }
        catch (winrt::hresult_error const& ex) {
            LOG_ERROR("Failed to copy file {}: {}", winrt::to_string(file.Name()), winrt::to_string(ex.message()));
        }
    } catch (const winrt::hresult_error& ex) {
        LOG_ERROR("Failed to access target folder: {}", winrt::to_string(ex.message()));
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
    if (PlatformUtils::isUWP()) {
        try {
            const auto notification = winrt::ToastNotification(winrt::ToastNotificationManager::GetTemplateContent(winrt::ToastTemplateType::ToastImageAndText02));

            winrt::Windows::Data::Xml::Dom::IXmlNodeList element = notification.Content().GetElementsByTagName(L"text");
            element.Item(0).InnerText(winrt::to_hstring(title));
            element.Item(1).InnerText(winrt::to_hstring(message));

            winrt::ToastNotificationManager::CreateToastNotifier().Show(notification);
        } catch (const winrt::hresult_error& e) {
            LOG_ERROR("Failed to show notification {}: {} ({})", title, winrt::to_string(e.message()), static_cast<uint32_t>(e.code()));
        }
    } else {
        // GDK/Win32: Use Windows notification API or fallback to message box
        Logger::warn("Notifications not implemented for GDK/Win32, showing message box instead");
        ShellMessageUtil::showA(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
    }
}

// ===== Platform-agnostic implementations =====

void WinrtUtils::setCursorVisibility(bool visible) {
    if (PlatformUtils::isUWP()) {
        if (visible) {
            setCursor(winrt::Windows::UI::Core::CoreCursor(winrt::Windows::UI::Core::CoreCursorType::Arrow, 0));
        } else {
            setCursor(winrt::Windows::UI::Core::CoreCursor(winrt::Windows::UI::Core::CoreCursorType::Custom, 0));
        }
    } else {
        // GDK/Win32: Use standard ShowCursor
        ShowCursor(visible);
    }
}

std::vector<std::string> WinrtUtils::pickFilesSync(const std::string& filter) {
    std::vector<std::string> result;

    // Use Win32 file dialog for GDK (also works on UWP but async is preferred)
    OPENFILENAMEA ofn;
    char szFile[4096] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        // Parse multiple file selection
        std::string directory = szFile;
        char* pFile = szFile + directory.length() + 1;

        if (*pFile == 0) {
            // Single file selected
            result.push_back(directory);
        } else {
            // Multiple files selected
            while (*pFile) {
                std::string fullPath = directory + "\\" + pFile;
                result.push_back(fullPath);
                pFile += strlen(pFile) + 1;
            }
        }
    }

    return result;
}

void WinrtUtils::cacheUIHandles() noexcept {
    try {
        Logger::debug("Caching UI handles in WinrtUtils... (thread id {})", GetCurrentThreadId());
        // MUST be called on the UI thread. Safe operations only here.
        auto window = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow();
        auto dispatcher = window.Dispatcher();

        std::lock_guard<std::mutex> lk(s_uiMutex);
        s_coreWindow = window;
        s_coreDispatcher = dispatcher;
        s_handlesCached.store(true, std::memory_order_release);
        Logger::debug("UI handles cached! :3");
    } catch (...) {
        Logger::debug("Failed to cache UI handles in WinrtUtils!");
    }
}

bool WinrtUtils::hasCachedUIHandles() noexcept {
    return s_handlesCached.load(std::memory_order_acquire);
}

void WinrtUtils::setCursorOnUI(winrt::Windows::UI::Core::CoreCursor cursor) {
    // Must be executed on UI thread or via dispatcher
    try {
        if (s_coreWindow) {
            s_coreWindow.PointerCursor(cursor);
            WinrtUtils::currentCursorType.store(cursor.Type(), std::memory_order_relaxed);
        }
    } catch (...) {}
}

void WinrtUtils::installPointerHook() noexcept {
    if (!g_uiCached.load(std::memory_order_acquire))
        return;

    std::lock_guard<std::mutex> lock(s_uiMutex);
    if (!s_coreWindow)
        return;

    try {
        if (g_pointerMovedToken.value != 0) {
            s_coreWindow.PointerMoved(g_pointerMovedToken);
        }
    } catch (...) {}

    g_pointerMovedToken = s_coreWindow.PointerMoved(
        [](auto&&, auto&&) {
            try {
                auto type = g_requestedCursor.load(std::memory_order_relaxed);
                s_coreWindow.PointerCursor(
                    winrt::Windows::UI::Core::CoreCursor(type, 0)
                );
                currentCursorType.store(type, std::memory_order_relaxed);
            } catch (...) {}
        }
    );
}
