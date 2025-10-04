#include "VersionUtils.hpp"

#include <Client/Client.hpp>
#include <Utils/Memory/Game/Offset/OffsetInit.hpp>
#include <Utils/Memory/Game/Sig/SigInit.hpp>
#include <Utils/WinrtUtils.hpp>
#include <Utils/Logger/Logger.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

std::vector<std::pair<std::string, std::pair<std::function<void()>, std::function<void()>>>> VersionUtils::versions;

std::string VersionUtils::getFormattedVersion() {
    Version currentVersion = WinrtUtils::impl::getGameVersion();
    std::string version = WinrtUtils::impl::toRawString(currentVersion);

    if (version == "1.21.202") { // i cba for now
        // 1.21.202.0 - 1.21.2
        // 1.21.2003.0 - 1.21.20
        return "1.21.02";
    }

    if (version == "1.21.3") { // i cba for now #2
        // 1.21.3.0 - 1.21.0 ".3"
        // 1.21.3003.0 - 1.21.30
        return "1.21.03";
    }

    // Split version string by dots
    std::vector<std::string> parts;
    std::istringstream iss(version);
    std::string part;

    while (std::getline(iss, part, '.')) {
        parts.push_back(part);
    }

    // Ensure at least major and minor parts exist
    if (parts.size() < 2) {
        return version; // Invalid version format
    }

    // Construct formatted version string
    std::string formattedVersion = parts[0] + "." + parts[1];
    if (parts.size() > 2) {
        formattedVersion += "." + parts[2].substr(0, parts[2].size() - 3); // Take only the first character of the patch version
    }

    return formattedVersion;
}

void VersionUtils::initialize() {
    versions = {
        {"1.21.11", {SigInit::init21110, OffsetInit::init21110}},
        {"1.21.10", {SigInit::init21100, OffsetInit::init21100}},
        {"1.21.9", {SigInit::init2190, OffsetInit::init2190}},
        {"1.21.8", {SigInit::init2180, OffsetInit::init2180}},
        {"1.21.7", {SigInit::init2170, OffsetInit::init2170}},
        {"1.21.6", {SigInit::init2160, OffsetInit::init2160}},
        {"1.21.5", {SigInit::init2150, OffsetInit::init2150}},
        {"1.21.4", {SigInit::init2140, OffsetInit::init2140}},
        {"1.21.3", {SigInit::init2130, OffsetInit::init2130}},
        {"1.21.2", {SigInit::init2120, OffsetInit::init2120}},
        {"1.21.02", {SigInit::init2102, OffsetInit::init2100}},
        {"1.21.1",  {SigInit::init2100, OffsetInit::init2100}},
        {"1.21.03",  {SigInit::init2100, OffsetInit::init2100}},
        {"1.20.8", {SigInit::init2080, OffsetInit::init2080}},
        {"1.20.7", {SigInit::init2070, OffsetInit::init2070}},
        {"1.20.6", {SigInit::init2060, OffsetInit::init2060}},
        {"1.20.5", {SigInit::init2050, OffsetInit::init2050}},
        {"1.20.4", {SigInit::init2040, OffsetInit::init2040}},
        {"1.20.3", {SigInit::init2030, OffsetInit::init2030}}
    };

    std::ranges::reverse(versions);
}

bool VersionUtils::isSupported(const std::string& version) {
    if (std::ranges::any_of(versions, [&version](const auto& p) {
        return p.first == version;
    })) {
        return true;
    }

    std::string supported;
    for (const auto& [v, _] : versions) {
        if (!supported.empty()) supported += ", ";
        supported += v;
    }
    Logger::debug("unsupported {} vs supported {}", version, supported);
    return false;

}


void VersionUtils::addData() {
    std::thread t1([&](){
        for (const auto&[fst, snd] : versions) {
            snd.first(); // Load signatures
            if (fst == Client::version) {
                break;
            }
        }
    });

    std::thread t2([&](){
        for (const auto&[fst, snd] : versions) {
            snd.second(); // Load offsets
            if (fst == Client::version) {
                break;
            }
        }
    });


    t1.join();
    t2.join();

    Mgr.scanAllSignatures();

}

bool VersionUtils::checkAboveOrEqual(const int m, const int b) {
    static auto [major, minor, build, error] = WinrtUtils::impl::getGameVersion();
    if (minor > m)
        return true;
    if (minor == m)
        return (build / 100) >= b;
    return false;
}

bool VersionUtils::checkBelowOrEqual(const int m, const int b) {
    static auto [major, minor, build, error] = WinrtUtils::impl::getGameVersion();
    if (minor < m)
        return true;
    if (minor == m)
        return (build / 100) <= b;
    return false;
}

bool VersionUtils::checkBetween(const int m1, const int b1, const int m2, const int b2) {
    return checkAboveOrEqual(m1, b1) && checkBelowOrEqual(m2, b2);
}