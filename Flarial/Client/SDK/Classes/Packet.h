#pragma once
#include "../../Utils/Utils.h"
#include "../../Mem/Mem.h"
#include "../../Utils/Signatures.h"
#include "Packets/MinecraftPacketIds.h"

enum InputMode { INPUT_MODE_UNKNOWN = 0, MOUSE = 1, TOUCH = 2, GAME_PAD = 3, MOTION_CONTROLLER = 4 };
enum PlayMode { PLAY_MODE_NORMAL = 0, TEASER = 1, SCREEN = 2, VIEWER = 3, REALITY = 4, PLACEMENT = 5, LIVING_ROOM = 6, EXIT_LEVEL = 7, EXIT_LEVEL_LIVING_ROOM = 8, NUM_MODES = 9 };

enum MovePacketMode { MOVE_PACKET_MODE_NORMAL = 0, RESET = 1, TELEPORT = 2, ROTATION = 3 };
enum MovePacketCause { MOVE_PACKET_CAUSE_UNKNOWN = 0, PROJECTILE = 1, CHORUS_FRUIT = 2, COMMAND = 3, BEHAVIOR = 4 };

/* namespace PacketVTables {
    uintptr_t TextPacketSig = NULL;
    uintptr_t CommandRequestPacketSig = NULL;
} */

class PacketHandlerDispatcherInstance {
public:
    uintptr_t** vTable;
};

class Packet {
public:
    uintptr_t** vTable;
private:
    char pad_0000[0x18];
public:
    PacketHandlerDispatcherInstance* packetHandler;
private:
    char pad_0028[0x8];
public:
    template<typename P>
    void setVTable(uintptr_t sig) {
        if (sig == NULL)
            return;

        int offset = *(int*)(sig + 3);
        this->vTable = (uintptr_t**)(sig + offset + 7);
    }
public:
    std::string getName();
    MinecraftPacketIds getId();
};

static_assert(sizeof(Packet) == 0x30);

//static_assert(offsetof(TextPacket, xboxUserId) == 0x98);