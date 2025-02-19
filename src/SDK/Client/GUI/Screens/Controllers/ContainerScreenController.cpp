#include "ContainerScreenController.hpp"

#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <Utils/Memory/Memory.hpp>

void ContainerScreenController::_handlePlaceAll(std::string collectionName, int32_t slot) {
    static int off = GET_OFFSET("ContainerScreenController::_handlePlaceAll");
    return Memory::CallVFuncI<void, std::string, int32_t>(off, this, collectionName, slot);
}

void ContainerScreenController::_handleTakeAll(std::string collectionName, int32_t slot) {

    using func = void(__fastcall*)(ContainerScreenController*, std::string, int32_t);
    static auto handlePlaceAll = reinterpret_cast<func>(Memory::offsetFromSig(GET_SIG_ADDRESS("ContainerScreenController::_handleTakeAll"), 1));
    return handlePlaceAll(this, collectionName, slot);
}

void ContainerScreenController::swap(std::string srcCollectionName, int32_t srcSlot, std::string dstCollectionName, int32_t dstSlot) {
    _handleTakeAll(srcCollectionName, srcSlot);
    _handlePlaceAll(dstCollectionName, dstSlot);
    _handlePlaceAll(srcCollectionName, srcSlot);
}
