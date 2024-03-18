#include "ItemStack.hpp"

bool ItemStack::isValid() {
    return item.counter != nullptr;
}

Item* ItemStack::getItem() {
    return item.get();
}

short ItemStack::getDamageValue() {
    if (item.counter == nullptr)
        return 0;

    using getDamageValueFunc = short(__fastcall*)(Item*, void*);
    static auto getDamageValue = reinterpret_cast<getDamageValueFunc>(Memory::findSig("48 89 5C 24 ? 57 48 83 EC ? 48 8B DA 48 85 D2"));
    return getDamageValue(this->item.get(), this->tag);
}

short ItemStack::getMaxDamage() {
    using getMaxDamageFunc = short(__fastcall*)(ItemStack*);
    static auto getMaxDamage = reinterpret_cast<getMaxDamageFunc>(Memory::findSig("48 8B 41 08 48 85 C0 74 19"));
    return getMaxDamage(this);
}