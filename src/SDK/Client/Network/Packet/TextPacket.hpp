#pragma once

#include "../../../../Utils/Memory/Memory.hpp"
#include "Packet.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <memory>

enum class TextPacketType : int8_t
{
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
    JSON_ANNOUNCEMENT = 11
};

struct LocalizedText
{
    void* vtable = nullptr;
    void* resourcePtr = nullptr;
    uint8_t flags = 0;
    uint8_t padding[7] = {};
    void* localeState = nullptr;
    uint64_t formattingFlags = 0;

    using PtrLT = std::shared_ptr<LocalizedText>;
    using Arg = std::variant<std::string, int64_t, PtrLT>;
    std::vector<Arg> args;

    std::string value;

    bool isTranslationKey() const noexcept { return (flags & 1) != 0; }
};

struct MessageOnly
{
    LocalizedText message;
};

struct AuthorAndMessage
{
    std::string author; // "Player Name"
    LocalizedText message;
};

struct MessageAndParams
{
    std::string translationKey;
    std::vector<LocalizedText::Arg> params;
};

struct TextPacketPayload_v21130
{
    bool translationNeeded = false;
    std::string xuid;
    std::string platformId;
    std::optional<std::string> filteredMessage;
    TextPacketType type;
    std::string str;
    std::variant<std::monostate, std::string, std::vector<std::string>> data;
};

using TextPacket_v21130 = PayloadPacket<TextPacketPayload_v21130>;

class TextPacket : public Packet
{
public:
    TextPacketType type; // 0x30
    std::string name; // 0x38
    std::string message; // 0x58
    std::vector<std::string> mParams; // 0x78
    std::optional<std::string> filteredMessage; // 0xA0
    bool translationNeeded = false; // 0xB8
    // padding may be required by ABI here
    std::string xuid; // 0xC0
    std::string platformId; // 0xE0

    TextPacket() = default;
};

inline void craftChatPacket(void* packetPtr, const std::string& author, const std::string& msg)
{
    if (VersionUtils::checkAboveOrEqual(21, 130))
    {
        auto* p = static_cast<TextPacket_v21130*>(packetPtr);
        // PayloadPacket<T> inherits from T directly
        p->type = TextPacketType::CHAT;
        p->data = msg;
        p->str = author;
        p->platformId.clear();
        p->translationNeeded = false;
        p->xuid.clear();
    }
    else
    {
        auto* p = static_cast<TextPacket*>(packetPtr);
        p->type = TextPacketType::CHAT;
        p->message = msg;
        p->name = author;
        p->platformId.clear();
        p->translationNeeded = false;
        p->xuid.clear();
    }
}

inline void craftChatPacket(TextPacket* p, const std::string& author, const std::string& msg)
{
    p->type = TextPacketType::CHAT;
    p->message = msg;
    p->name = author;
    p->platformId.clear();
    p->translationNeeded = false;
    p->xuid.clear();
}

inline void craftChatPacket(TextPacket_v21130* p, const std::string& author, const std::string& msg)
{
    p->type = TextPacketType::CHAT;
    p->data = msg;
    p->str = author;
    p->platformId.clear();
    p->translationNeeded = false;
    p->xuid.clear();
}

struct TextPacketCommon
{
    TextPacketType type;
    std::string name;
    std::string message;
    std::optional<std::string> filteredMessage;
    bool translationNeeded = false;
    std::string xuid;
    std::string platformId;
};

class TextPacketView : public TextPacketCommon
{
public:
    explicit TextPacketView(void* packetPtr)
    {
        if (VersionUtils::checkAboveOrEqual(21, 130))
        {
            auto* p = static_cast<TextPacket_v21130*>(packetPtr);
            this->type = p->type;
            this->name = std::holds_alternative<std::string>(p->data) ? p->str : "";
            this->message = std::holds_alternative<std::string>(p->data)
                                ? std::get<std::string>(p->data)
                                : p->str;
            this->filteredMessage = p->filteredMessage;
            this->translationNeeded = p->translationNeeded;
            this->xuid = p->xuid;
            this->platformId = p->platformId;
        }
        else
        {
            const auto* p = static_cast<TextPacket*>(packetPtr);
            this->type = p->type;
            this->name = p->name;
            this->message = p->message;
            this->filteredMessage = p->filteredMessage;
            this->translationNeeded = p->translationNeeded;
            this->xuid = p->xuid;
            this->platformId = p->platformId;
        }
    }

    TextPacketType getType() const { return this->type; }
    std::string getName() const { return this->name; }
    std::string getMessage() const { return this->message; }
    std::optional<std::string> getFilteredMessage() const { return this->filteredMessage; }
    bool isTranslationNeeded() const { return this->translationNeeded; }
    std::string getXuid() const { return this->xuid; }
    std::string getPlatformId() const { return this->platformId; }
    bool isChat() const { return this->type == TextPacketType::CHAT; }
};

inline std::optional<TextPacketView> getTextPacket(void* packetPtr)
{
    try
    {
        return TextPacketView(packetPtr);
    }
    catch (...)
    {
        // Invalid packet data (can happen when exiting world)
        return std::nullopt;
    }
}