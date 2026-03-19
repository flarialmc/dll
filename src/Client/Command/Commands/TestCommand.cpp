#include "TestCommand.hpp"

#include <Utils/WinrtUtils.hpp>
#include <Utils/PlatformUtils.hpp>

#include <winrt/Windows.Media.Control.h>

using namespace winrt;
using namespace Windows::Media::Control;


void TestCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        addCommandMessage("§cUsage: .test <uri/pick/notification/box/clipboard>");
        return;
    }

    const std::string action = String::toLower(args[0]);

    if (PlatformUtils::isUWP()) {
        if (action == "uri") {
            WinrtUtils::launchURI("https://flarial.xyz");
        }
        if (action == "pick") {
            WinrtUtils::pickFiles();
        }
        if (action == "copy") {
            WinrtUtils::pickAndCopyFiles(L"*", "\\assets");
        }
        if (action == "notification") {
            WinrtUtils::showNotification("Flarial", "Hi");
        }
        if (action == "box") {
            WinrtUtils::showMessageBox("Flarial", "Your Minecraft version is not supported.");
        }
        if (action == "clipboard") {
            if (args[1].empty()) return;

            if (args[1] == "get") {
                addCommandMessage(WinrtUtils::getClipboard());
            }
            if (args[1] == "set") {
                if (args[2].empty()) return;
                WinrtUtils::setClipboard(args[2]);
            }
        }
    }
    else if (PlatformUtils::isGDK()) {
        if (action == "trackinfo") {

            GlobalSystemMediaTransportControlsSessionManager sessionMgr =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();

            GlobalSystemMediaTransportControlsSession currentSession = sessionMgr.GetCurrentSession();
            if (currentSession == nullptr) {
                addCommandMessage("No track");
                return;
            }

            GlobalSystemMediaTransportControlsSessionMediaProperties mediaInfo = currentSession.TryGetMediaPropertiesAsync().get();

            auto appId = currentSession.SourceAppUserModelId();
            std::wstring appName = std::wstring(appId);

            std::wstring title = mediaInfo.Title().c_str();
            std::wstring artist = mediaInfo.Artist().c_str();
            std::wstring album = mediaInfo.AlbumTitle().c_str();

            addCommandMessage(std::format("App: {}\nTitle: {}\nArtist: {}\nAlbum: {}",
                std::string(appName.begin(), appName.end()),
                std::string(title.begin(), title.end()),
                std::string(artist.begin(), artist.end()),
                std::string(artist.begin(), artist.end()))
            );
        }
    }



}
