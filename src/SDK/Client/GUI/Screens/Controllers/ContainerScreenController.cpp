#include "ContainerScreenController.hpp"

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <SDK/Client/Item/ItemStack.hpp>
#include <Utils/Memory/Memory.hpp>
#include <SDK/SDK.hpp>

ItemStack *ContainerScreenController::getContainerItem(ContainerEnum type, int slot) {
    auto lp = SDK::clientInstance->getLocalPlayer();
    if (!lp) return nullptr;

    auto inventory = lp->getSupplies()->getInventory();
    if (inventory == nullptr) return nullptr;

    // src is where the item is
    bool srcAsDest = false;

    if (type == ContainerEnum::OTHER) return nullptr;

    auto startSlot = type == ContainerEnum::HOTBAR ? 0 : 9;

    auto itemStack = inventory->getItem(startSlot + slot);

    return itemStack;
}

ContainerEnum ContainerScreenController::getContainerType(std::string name) {
    if (name == "hotbar_items") return ContainerEnum::HOTBAR;
    if (name == "inventory_items") return ContainerEnum::INVENTORY;
    if (name.find("_output") != std::string::npos) return ContainerEnum::CONTAINER_OUTPUT;
    return ContainerEnum::OTHER;
}

void ContainerScreenController::_handlePlaceAll(std::string collectionName, int32_t slot) {
    static int off = GET_OFFSET("ContainerScreenController::_handlePlaceAll");
    return Memory::CallVFuncI<void, std::string, int32_t>(off, this, collectionName, slot);
}

void ContainerScreenController::_handleTakeAll(std::string collectionName, int32_t slot) {
    using func = void(__fastcall*)(ContainerScreenController *, std::string, int32_t);
    static auto handlePlaceAll = reinterpret_cast<func>(Memory::offsetFromSig(GET_SIG_ADDRESS("ContainerScreenController::_handleTakeAll"), 1));
    return handlePlaceAll(this, collectionName, slot);
}

void ContainerScreenController::swap(std::string srcCollectionName, int32_t srcSlot, std::string dstCollectionName, int32_t dstSlot) {
    auto srcContainerType = getContainerType(srcCollectionName);
    auto dstContainerType = getContainerType(dstCollectionName);

    auto srcContainerItemPtr = getContainerItem(srcContainerType, srcSlot);
    if (!srcContainerItemPtr) return;
    auto srcItem = srcContainerItemPtr->getItem();

    auto dstContainerItemPtr = getContainerItem(dstContainerType, dstSlot);
    if (!dstContainerItemPtr) return;
    auto dstItem = dstContainerItemPtr->getItem();

    if (srcContainerType == ContainerEnum::CONTAINER_OUTPUT) {
        _handleTakeAll(srcCollectionName, srcSlot);
        _handlePlaceAll(dstCollectionName, dstSlot);
        return;
    }

    if (!srcItem && dstItem) {
        _handleTakeAll(dstCollectionName, dstSlot);
        _handlePlaceAll(srcCollectionName, srcSlot);
        _handlePlaceAll(dstCollectionName, dstSlot);
        return;
    }

    _handleTakeAll(srcCollectionName, srcSlot);
    _handlePlaceAll(dstCollectionName, dstSlot);
    _handlePlaceAll(srcCollectionName, srcSlot);
}
