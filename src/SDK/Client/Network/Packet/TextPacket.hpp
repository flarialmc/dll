#pragma once

#include "Packet.hpp"
#include <Utils/Memory/Memory.hpp>
#include <Utils/VersionUtils.hpp>
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <concepts>
#include <utility>

enum class TextPacketType : int8_t {
    RAW = 0,
    CHAT = 1,
    TRANSLATION = 2,
    POPUP = 3,
    JUKEBOX_POPUP = 4,
    TIP = 5,
    SYSTEM = 6,
    WHISPER = 7,
    ANNOUNCEMENT = 8,
    JSON_WHISPER = 9,
    JSON = 10,
    JSON_ANNOUNCEMENT = 11 // 1.19.30+
};

class TextPacketBase : public Packet {
protected:
    TextPacketBase() = default;

public:
    //code has peaked
    //AKA this is the simplest  way to do it
    static std::string typeToString(TextPacketType type) {
        switch (type) {
        case TextPacketType::RAW: return "RAW";
        case TextPacketType::CHAT: return "CHAT";
        case TextPacketType::TRANSLATION: return "TRANSLATION";
        case TextPacketType::POPUP: return "POPUP";
        case TextPacketType::JUKEBOX_POPUP: return "JUKEBOX_POPUP";
        case TextPacketType::TIP: return "TIP";
        case TextPacketType::SYSTEM: return "SYSTEM";
        case TextPacketType::WHISPER: return "WHISPER";
        case TextPacketType::JSON_WHISPER: return "JSON_WHISPER";
        case TextPacketType::JSON: return "JSON";
        case TextPacketType::JSON_ANNOUNCEMENT: return "JSON_ANNOUNCEMENT";
        default:
            return "UNKNOWN";

        }
    }
};

// not sure the earliest version this structure is ABI compatible with, but marking it as the minimum version Flarial supports
class __declspec(empty_bases) TextPacket_1_20_0 : public TextPacketBase {
public:
    TextPacketType type = TextPacketType::CHAT; // 0x30
    std::string name{}; // 0x38
    std::string message{}; // 0x58

    std::vector<std::string> mParams{}; // 0x78
    bool translationNeeded = false; // 0x90
    std::string xuid{}; // 0x98
    std::string platformId{}; // 0xB8

    TextPacket_1_20_0() = default;
};

class __declspec(empty_bases) TextPacket_1_21_0 : public TextPacketBase {
public:
    TextPacketType type = TextPacketType::CHAT; // 0x30
    std::string name{}; // 0x38
    std::string message{}; // 0x58
    std::optional<std::string> filteredMessage{}; // 0x78
    std::vector<std::string> mParams{}; // 0xA0
    bool translationNeeded = false; // 0xB8
    std::string xuid{}; // 0xC0
    std::string platformId{}; // 0xE0

    TextPacket_1_21_0() = default;
};

static_assert(sizeof(TextPacket_1_20_0) == 0xD8);
static_assert(sizeof(TextPacket_1_21_0) == 0x100);

class TextPacketProxy : public TextPacketBase {
    template<std::derived_from<TextPacketBase> ABI>
    const auto& as() const {
        return *reinterpret_cast<const ABI*>(this); // copy_cvref type trait would be nice
    }

    template<std::derived_from<TextPacketBase> ABI>
    auto& as() {
        return const_cast<ABI&>(std::as_const(*this).as<ABI>()); // copy_cvref type trait would be nice
    }

    template<typename T>
    static constexpr auto&& selectMember(auto&& self, T TextPacket_1_20_0::* m20, T TextPacket_1_21_0::* m21) {
        if (VersionUtils::checkAboveOrEqual(21, 0)) {
            return (std::forward<decltype(self)>(self).template as<TextPacket_1_21_0>().*m21);
        }
        else {
            return (std::forward<decltype(self)>(self).template as<TextPacket_1_20_0>().*m20);
        }
    }

public:
    // TODO: generate these with a macro
    constexpr auto&& getType(this auto&& self) { return selectMember(std::forward<decltype(self)>(self), &TextPacket_1_20_0::type, &TextPacket_1_21_0::type); }
    constexpr auto&& getName(this auto&& self) { return selectMember(std::forward<decltype(self)>(self), &TextPacket_1_20_0::name, &TextPacket_1_21_0::name); }
    constexpr auto&& getMessage(this auto&& self) { return selectMember(std::forward<decltype(self)>(self), &TextPacket_1_20_0::message, &TextPacket_1_21_0::message); }
    constexpr auto&& getTranslationNeeded(this auto&& self) { return selectMember(std::forward<decltype(self)>(self), &TextPacket_1_20_0::translationNeeded, &TextPacket_1_21_0::translationNeeded); }
    constexpr auto&& getXuid(this auto&& self) { return selectMember(std::forward<decltype(self)>(self), &TextPacket_1_20_0::xuid, &TextPacket_1_21_0::xuid); }
    constexpr auto&& getPlatformId(this auto&& self) { return selectMember(std::forward<decltype(self)>(self), &TextPacket_1_20_0::platformId, &TextPacket_1_21_0::platformId); }
};