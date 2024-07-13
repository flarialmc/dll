#include "ItemStack.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

bool ItemStack::isValid() const {
    return item.counter != nullptr;
}

Item* ItemStack::getItem() const {
    return item.get();
}

short ItemStack::getDamageValue() {
    if (item.counter == nullptr)
        return 0;

    using getDamageValueFunc = short(__fastcall*)(Item*, void*);
    static auto getDamageValue = reinterpret_cast<getDamageValueFunc>(Memory::findSig(GET_SIG("ItemStack::getDamageValue")));
    return getDamageValue(this->item.get(), this->tag);
}

short ItemStack::getMaxDamage() {
    using getMaxDamageFunc = short(__fastcall*)(ItemStack*);
    static auto getMaxDamage = reinterpret_cast<getMaxDamageFunc>(Memory::findSig(GET_SIG("ItemStack::getMaxDamage")));
    return getMaxDamage(this);
}