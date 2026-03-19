#pragma once
#include <cstdint>
#include <string>

struct ActorDefinitionIdentifierComponent : IEntityComponent {
    std::string mNamespace;      // 0x00 — e.g. "minecraft"
    std::string mIdentifier;     // 0x20 — e.g. "ender_crystal"
    std::string mInitEvent;      // 0x40
    std::string mFullName;       // 0x60 — e.g. "minecraft:ender_crystal"
    // HashedString mCanonicalName (48 bytes): hash + string + lastMatch ptr
    uint64_t mCanonicalHash;     // 0x80
    std::string mCanonicalStr;   // 0x88
    const void* mLastMatch;      // 0xA8
};
static_assert(sizeof(ActorDefinitionIdentifierComponent) == 176);
