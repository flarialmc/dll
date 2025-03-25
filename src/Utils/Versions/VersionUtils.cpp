#include "VersionUtils.hpp"

// Made by marioCST, ported by FreezeEngine

#include <thread>

#include "../Memory/Game/Offset/OffsetInit.hpp"
#include "../Memory/Game/Sig/SigInit.hpp"
#include "../../Client/Client.hpp"

std::vector<std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>>> VersionUtils::versions;

void VersionUtils::init() {
    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2170 = {"1.21.7", {SigInit::init2170, OffsetInit::init2170}};
    versions.push_back(p2170);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2160 = {"1.21.6", {SigInit::init2160, OffsetInit::init2160}};
    versions.push_back(p2160);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2150 = {"1.21.5", {SigInit::init2150, OffsetInit::init2150}};
    versions.push_back(p2150);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2140 = {"1.21.4", {SigInit::init2140, OffsetInit::init2140}};
    versions.push_back(p2140);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2130 = {"1.21.3", {SigInit::init2130, OffsetInit::init2130}};
    versions.push_back(p2130);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2120 = {"1.21.2", {SigInit::init2120, OffsetInit::init2120}};
    versions.push_back(p2120);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2102 = {"1.21.02", {SigInit::init2102, OffsetInit::init2100}};
    versions.push_back(p2102);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2110 = {"1.21.1", {SigInit::init2100, OffsetInit::init2100}};
    versions.push_back(p2110);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2100 = {"1.21.03", {SigInit::init2100, OffsetInit::init2100}};
    versions.push_back(p2100);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2080 = {"1.20.8", {SigInit::init2080, OffsetInit::init2080}};
    versions.push_back(p2080);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2070 = {"1.20.7", {SigInit::init2070, OffsetInit::init2070}};
    versions.push_back(p2070);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2060 = {"1.20.6", {SigInit::init2060, OffsetInit::init2060}};
    versions.push_back(p2060);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2050 = {"1.20.5", {SigInit::init2050, OffsetInit::init2050}};
    versions.push_back(p2050);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2040 = {"1.20.4", {SigInit::init2040, OffsetInit::init2040}};
    versions.push_back(p2040);

    std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>> p2030 = {"1.20.3", {SigInit::init2030, OffsetInit::init2030}};
    versions.push_back(p2030);

    std::reverse(versions.begin(), versions.end());
}

bool VersionUtils::isSupported(const std::string& version) {
    return std::find_if(versions.begin(), versions.end(),
                        [version](const std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>>& p) {
                            return p.first == version;
                        }) != versions.end();
}

void VersionUtils::addData() {
    // load all offsets and sigs for each version
    auto sigs = [&](){
        for (const auto &item: versions) {
            item.second.first();
            if(item.first == Client::version) {
                return;
            }
        }
    };
    auto offsets = [&](){
        for (const auto &item: versions) {
            item.second.second();
            if(item.first == Client::version) {
                return;
            }
        }
    };

    std::thread t1(sigs);
    std::thread t2(offsets);

    t1.join();
    t2.join();

    Mgr.scanAllSignatures();
}
