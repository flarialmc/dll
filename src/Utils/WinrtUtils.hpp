#pragma once

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>

struct Version {
    int major = 0;
    int minor = 0;
    int build = 0;
    std::string error;
};

class WinrtUtils {
  public:
    class impl {
        public:
        static Version getGameVersion();
        static std::string toRawString(const Version& version);
    };
    static void setCursorType(winrt::Windows::UI::Core::CoreCursorType cursor);
    static void setWindowTitle(const std::string& title);
    static std::string getFormattedVersion();

    static winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Storage::StorageFile>> pickFiles(std::wstring_view fileType = L"*");
    static winrt::Windows::Foundation::IAsyncAction pickAndCopyFiles(std::wstring_view fileType, std::string path);
    static void launchURI(const std::string& uri);
    static void openSubFolder(const std::string& subFolder);

    static void setClipboard(const std::string& text);
    static std::string getClipboard();

    static void showMessageBox(const std::string& title, const std::string& message);
    static void showNotification(const std::string& title, const std::string& message);
};