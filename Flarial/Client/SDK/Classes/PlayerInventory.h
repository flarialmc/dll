#pragma once
#include "ItemStack.h"

class Container {
private:
	virtual ~Container(void);
public:
	virtual auto init(void) -> void;
	virtual auto serverInitItemStackIds(int, int, class std::function<void(int, class ItemStack const&)>) -> void;
public:
	virtual auto addContentChangeListener(class ContainerContentChangeListener*) -> void;
	virtual auto removeContentChangeListener(class ContainerContentChangeListener*) -> void;
public:
	virtual auto getItem(int) -> class ItemStack*;
	virtual auto hasRoomForItem(class ItemStack*) -> bool;
public:
	virtual auto addItem(class ItemStack*) -> bool;
	virtual auto addItemToFirstEmptySlot(class ItemStack*) -> bool;
public:
	virtual auto setItem(int, class ItemStack*) -> void;
	virtual auto setItemWithForceBalance(int, class ItemStack*, bool) -> void;
public:
	virtual auto removeItem(int, int) -> void;
	virtual auto removeAllItems(void) -> void;
	virtual auto dropContents(class BlockSource*, Vec3<float>*, bool) -> void;
public:
	virtual auto getContainerSize(void) -> __int64;
	virtual auto getMaxStackSize(void) -> __int64;
public:
	virtual auto startOpen(class Player*) -> void;
	virtual auto stopOpen(class Player*) -> void;
public:
	virtual auto getSlotCopies(void) -> std::vector<class ItemStack, class std::allocator<class ItemStack>>;
	virtual auto getSlots(void) -> std::vector<class ItemStack const*, class std::allocator<class ItemStack const*>>;
public:
	virtual auto getItemCount(class ItemStack*) -> int;
	virtual auto findFirstSlotForItem(class ItemStack*) -> int;
	virtual auto canPushInItem(int, int, class ItemStack const&) -> bool;
	virtual auto canPullOutItem(int, int, class ItemStack const&) -> bool;
public:
	virtual auto setContainerChanged(int) -> void;
	virtual auto setContainerMoved(void) -> void;
public:
	virtual auto setCustomName(std::string&) -> void;
	virtual auto isWatchingIfActorEnters(void) -> bool;
public:
	virtual auto readAdditionalSaveData(class CompoundTag*) -> void;
	virtual auto addAdditionalSaveData(class CompoundTag*) -> void;
public:
	virtual auto createTransactionContext(class std::function<void(class Container&, int, class ItemStack const&, class ItemStack const&)>, class std::function<void(void)>) -> void;
	virtual auto initializeContainerContents(class BlockSource*) -> void;
public:
	virtual auto isEmpty(void) -> bool;
public:
	virtual auto add(class ItemStack*) -> bool;
	virtual auto canAdd(class ItemStack*) -> bool;
public:
	virtual auto clearSlot(int) -> void;
	virtual auto clearInventory(int) -> __int64;
public:
	virtual auto load(class ListTag*, class SemVersion*, class Level*) -> void;
public:
	virtual auto getEmptySlotsCount(void) -> __int64;
};

class PlayerInventory {
public:
	auto getContainer(void) -> Container* {
		return *(Container**)((uintptr_t)(this) + 0xC0);
	};
public:
	BUILD_ACCESS(this, int, hotbarSlot, 0x10);
};