#pragma once

#include <winrt/impl/windows.storage.2.h>
#include "../../../../Client.hpp"

class ScriptMarketplace : public Listener {
public:
    ScriptMarketplace() {
        Listen(this, ProtocolEvent, &ScriptMarketplace::onProtocol);
    }

    ~ScriptMarketplace() {
        Deafen(this, ProtocolEvent, &ScriptMarketplace::onProtocol);
    }

    std::string scriptPath() {
        using namespace winrt::Windows::Storage;

        auto roamingFolder = Utils::getRoamingPath();
        std::filesystem::path scriptsDir = std::filesystem::path(roamingFolder) / "Flarial" / "scripts";

        return scriptsDir.string();
    }

    void onProtocol(ProtocolEvent event) {
        if (event.getPath() == std::wstring(L"flarial-scripting")) {
            for (const auto &pair: event.getProtocolArgs()) {
                if (pair.first == std::wstring(L"scriptId")) {
                    std::string id = String::WStrToStr(pair.second);
                    Logger::info("script id {}", id);
                    std::string url = "http://node2.sear.host:5019/api/scripts/" + id + "/download";
                    std::string data = APIUtils::get(url);
                    std::ofstream file(Utils::getRoamingPath() + "\\Flarial\\tmpd.tmp", std::ios::binary);
                    Logger::info("data: {}", data.c_str());
                    file.write(data.c_str(), data.size());
                    file.close();
                    Utils::extractFromFile(Utils::getRoamingPath() + "\\Flarial\\tmpd.tmp", scriptPath() + "\\" + id);
                    std::filesystem::remove(Utils::getRoamingPath() + "\\Flarial\\tmpd.tmp");
                }
            }
        }
    }
};