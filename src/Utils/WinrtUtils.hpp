#pragma once

#include "winrt/windows.applicationmodel.core.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/windows.system.h"

using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::UI::Core;

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
    static void setCursorType(CoreCursorType cursor);
    static void setWindowTitle(const std::string& title);
    static void openFolder(const std::string& path);
    static void openSubFolder(const std::string& path);
    static std::string getFormattedVersion();
    static int getRawGameVersion();
};