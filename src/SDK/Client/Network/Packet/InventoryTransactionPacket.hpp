#pragma once

#include "Packet.hpp"
#include "PlaySoundPacket.hpp"  // For NetworkBlockPosition
#include "../../../../Utils/Utils.hpp"
#include <memory>

// ComplexInventoryTransaction::Type enum
enum class ComplexInventoryTransactionType : int {
    NormalTransaction = 0,
    InventoryMismatch = 1,
    ItemUseTransaction = 2,
    ItemUseOnEntityTransaction = 3,
    ItemReleaseTransaction = 4
};

// Action types for item use transactions
enum class ItemUseActionType : int {
    Place = 0,           // Place/use item on block (right-click)
    Use = 1,             // Use item (eat, drink, etc.)
    Destroy = 2          // Destroy block
};

// Forward declaration
class ComplexInventoryTransaction;
class LocalPlayer;

// ItemUseInventoryTransaction structure based on IDA analysis
// Total size: 0x198 (408 bytes)
// Inherits from ComplexInventoryTransaction -> InventoryTransaction
struct ItemUseInventoryTransaction {
    // Base: ComplexInventoryTransaction
    void* vftable;                          // 0x00 - Virtual function table
    ComplexInventoryTransactionType mType;  // 0x08 - Transaction type
    int pad_0x0C;                           // 0x0C - Padding

    // InventoryTransaction member (at offset 0x10)
    // This contains mActions (unordered_map) and mContents (vector)
    // We'll leave this as padding since we don't need to manipulate it
    char mTransaction[0x58];                // 0x10-0x67 - InventoryTransaction base

    // ItemUseInventoryTransaction specific fields
    ItemUseActionType mActionType;          // 0x68 - Action type (Place=0, Use=1, Destroy=2)
    int mPosX;                              // 0x6C - Block X position
    int mPosY;                              // 0x70 - Block Y position
    int mPosZ;                              // 0x74 - Block Z position
    int mTargetBlockId;                     // 0x78 - Target block runtime ID
    uint8_t mFace;                          // 0x7C - Block face (0=down, 1=up, etc.)
    char pad_0x7D[3];                       // 0x7D - Padding for alignment
    int mSlot;                              // 0x80 - Hotbar slot
    char pad_0x84[4];                       // 0x84 - Padding

    // ItemStack structure (very complex - includes ItemStackBase, NetworkItemStackDescriptor)
    // Size: 0xF8 bytes (from 0x88 to 0x180)
    char mItem[0xF8];                       // 0x88-0x17F - ItemStack

    // Position data
    float mFromPosX;                        // 0x180 - Player X position
    float mFromPosY;                        // 0x184 - Player Y position
    float mFromPosZ;                        // 0x188 - Player Z position
    float mClickPosX;                       // 0x18C - Click X position (relative to block)
    float mClickPosY;                       // 0x190 - Click Y position
    float mClickPosZ;                       // 0x194 - Click Z position

    ItemUseInventoryTransaction() {
        memset(this, 0, sizeof(*this));
        mType = ComplexInventoryTransactionType::ItemUseTransaction;
        mActionType = ItemUseActionType::Place;
        mFace = 1;  // Up face by default
    }
};

static_assert(sizeof(ItemUseInventoryTransaction) == 0x198, "ItemUseInventoryTransaction size mismatch");

// InventoryTransactionPacket - uses unique_ptr to ComplexInventoryTransaction
// Based on LeviLamina analysis
class InventoryTransactionPacket : public Packet {
public:
    // After Packet base (0x30 bytes):
    // ItemStackLegacyRequestId mLegacyRequestId (16 bytes)
    // vector<pair<ContainerEnumName, vector<uchar>>> mLegacySetItemSlots (24 bytes)
    // unique_ptr<ComplexInventoryTransaction> mTransaction (8 bytes)
    // bool mIsClientSide (1 byte)

    char mPayload[0x40];  // Placeholder for the complex payload

    InventoryTransactionPacket() {
        memset(mPayload, 0, sizeof(mPayload));
    }
};

// Function pointer type for sendComplexInventoryTransaction
// void LocalPlayer::sendComplexInventoryTransaction(std::unique_ptr<ComplexInventoryTransaction>)
using SendComplexInventoryTransactionFn = void(__fastcall*)(
    LocalPlayer* player,
    std::unique_ptr<ComplexInventoryTransaction> transaction
);
