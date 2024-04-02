#include "VersionUtils.hpp"

// Made by marioCST, ported by FreezeEngine

#include <thread>

#include "../Memory/Game/Offset/OffsetInit.hpp"
#include "../Memory/Game/Sig/SigInit.hpp"
#include "../../Client/Client.hpp"

std::unordered_map<std::string, std::pair<std::function<void()>, std::function<void()>>> VersionUtils::versions;

void VersionUtils::init() {
    std::pair<std::function<void()>, std::function<void()>> p2070 = {SigInit::init2070, OffsetInit::init2070};
    versions.emplace("1.20.7", p2070);

    std::pair<std::function<void()>, std::function<void()>> p2060 = {SigInit::init2060, OffsetInit::init2060};
    versions.emplace("1.20.6", p2060);

    std::pair<std::function<void()>, std::function<void()>> p2050 = {SigInit::init2050, OffsetInit::init2050};
    versions.emplace("1.20.5", p2050);

    std::pair<std::function<void()>, std::function<void()>> p2030 = {SigInit::init2030, OffsetInit::init2030};
    versions.emplace("1.20.3", p2030);
}

bool VersionUtils::isSupported(const std::string& version) {
    return versions.contains(version);
}

void VersionUtils::addData() {
    const auto& [v, o] = versions.at(Client::version);

    std::thread t1(v);
    std::thread t2(o);

    t1.join();
    t2.join();
}
