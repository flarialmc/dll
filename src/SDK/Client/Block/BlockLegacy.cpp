#include "BlockLegacy.hpp"

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <Utils/WinrtUtils.hpp>

std::string BlockLegacy::getName() {
    static auto [major, minor, build, error] = WinrtUtils::impl::getGameVersion();

    if (minor < 26) return hat::member_at<std::string>(this, GET_OFFSET("BlockLegacy::name"));

    else {
        std::string s = hat::member_at<std::string>(this, GET_OFFSET("BlockLegacy::namespace"));

        size_t pos = s.find(':');
        if (pos != std::string::npos) {
            s.erase(0, pos + 1);
        }

        return s;
    }
}

std::string BlockLegacy::getNamespace() {
    static auto [major, minor, build, error] = WinrtUtils::impl::getGameVersion();

    std::string s = hat::member_at<std::string>(this, GET_OFFSET("BlockLegacy::namespace"));

    if (minor < 26) return s;

    else {

        size_t pos = s.find(':');
        if (pos != std::string::npos) {
            s.erase(pos);
        }

        return s;
    }
}
