#include "ItemStack.hpp"

#include <Utils/VersionUtils.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>

bool ItemStack::isValid() const {
    return item.counter != nullptr;
}

Item* ItemStack::getItem() const {
    return item.get();
}

bool ItemStack::isEnchanted() {
    using isEnchantedFunc = bool(__fastcall*)(ItemStack*);
    static auto getIsEnchanted = reinterpret_cast<isEnchantedFunc>(GET_SIG_ADDRESS("ItemStack::isEnchanted"));
    return getIsEnchanted(this);
}

short ItemStack::getDamageValue() {
    if (item.counter == nullptr)
        return 0;

    if(VersionUtils::checkAboveOrEqual(21, 40)) {
        using getDamageValueFunc = short (__fastcall *)(ItemStack *);
        static auto getDamageValue = reinterpret_cast<getDamageValueFunc>(GET_SIG_ADDRESS("ItemStack::getDamageValue"));
        return getDamageValue(this);
    } else {
        using getDamageValueFunc = short (__fastcall *)(Item *, void *);
        static auto getDamageValue = reinterpret_cast<getDamageValueFunc>(GET_SIG_ADDRESS("Item::getDamageValue"));
        return getDamageValue(this->item.get(), this->tag);
    }
}

short ItemStack::getMaxDamage() {
    using getMaxDamageFunc = short(__fastcall*)(ItemStack*);
    static auto getMaxDamage = reinterpret_cast<getMaxDamageFunc>(GET_SIG_ADDRESS("ItemStack::getMaxDamage"));
    return getMaxDamage(this);
}