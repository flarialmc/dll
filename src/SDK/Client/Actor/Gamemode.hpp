#pragma once

#include <Utils/Memory/Memory.hpp>

FK(Player)

struct BlockPos;

class Gamemode {
public:
    Player* getPlayer() {
        return hat::member_at<Player*>(this, GET_OFFSET("Gamemode::player"));
    };
    float getLastBreakProgress() {
        return hat::member_at<float>(this, GET_OFFSET("Gamemode::lastBreakProgress"));
    };

    // Virtual function at vindex 1
    // Starts destroying a block (left-click) - for note blocks this triggers attack/play
    // ?startDestroyBlock@GameMode@@UEAA_NAEBVBlockPos@@EAEA_N@Z
    bool startDestroyBlock(const BlockPos& pos, uint8_t face, bool& hasDestroyedBlock) {
        return Memory::CallVFuncI<bool>(1, this, pos, face, hasDestroyedBlock);
    }

    // Virtual function at vindex 4
    // Stops destroying a block - resets mDestroyProgress
    // ?stopDestroyBlock@GameMode@@UEAAXAEBVBlockPos@@@Z
    void stopDestroyBlock(const BlockPos& pos) {
        return Memory::CallVFuncI<void>(4, this, pos);
    }

    // Virtual function at vindex 2
    // Destroys a block at the given position
    // ?destroyBlock@GameMode@@UEAA_NAEBVBlockPos@@E@Z
    bool destroyBlock(const BlockPos& pos, uint8_t face) {
        return Memory::CallVFuncI<bool>(2, this, pos, face);
    }

    // Virtual function at vindex 6
    // Creates ItemUseInventoryTransaction and sends it to server
    // This is the proper way to interact with blocks (right-click)
    bool buildBlock(const BlockPos& pos, uint8_t face, bool isSimTick = false) {
        return Memory::CallVFuncI<bool>(6, this, pos, face, isSimTick);
    }


};