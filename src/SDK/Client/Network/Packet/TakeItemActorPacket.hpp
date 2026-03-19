#pragma once
#include <cstdint>
#include "Packet.hpp"
#include "../../../../Utils/Utils.hpp"

enum class SerializationMode : int {
    ManualOnly                         = 0,
    SideBySideLogOnMismatch            = 1,
    SideBySideAssertOnMismatch         = 2,
    SemanticSideBySideLogOnMismatch    = 3,
    SemanticSideBySideAssertOnMismatch = 4,
    CerealOnly                         = 5,
};

struct TakeItemActorPacketPayload {
public:
    // member variables
    // NOLINTBEGIN
    int64_t mItemId;
    int64_t mActorId;
    // NOLINTEND
};

class TakeItemActorPacket : public PayloadPacket<TakeItemActorPacketPayload> {
public:
    // member variables
    // NOLINTBEGIN
    SerializationMode mSerializationMode;
    // NOLINTEND
};