#include "ItemStack.hpp"
#include "SDK/Client/NBT/CompoundTag.hpp"
#include "SDK/Client/Block/BlockLegacy.hpp"

#include <Utils/VersionUtils.hpp>
#include <Utils/Memory/Game/SignatureAndOffsetManager.hpp>
#include <Utils/Logger/Logger.hpp>

bool ItemStack::isValid() const {
    return mItem.counter != nullptr;
}

Item* ItemStack::getItem() const {
    return mItem.get();
}

bool ItemStack::isEnchanted() {
    using isEnchantedFunc = bool(__fastcall*)(ItemStack*);
    static auto getIsEnchanted = reinterpret_cast<isEnchantedFunc>(GET_SIG_ADDRESS("ItemStack::isEnchanted"));
    return getIsEnchanted(this);
}

short ItemStack::getDamageValue() {
    if (mItem.counter == nullptr)
        return 0;

    if(VersionUtils::checkAboveOrEqual(21, 40)) {
        using getDamageValueFunc = short (__fastcall *)(ItemStack *);
        static auto getDamageValue = reinterpret_cast<getDamageValueFunc>(GET_SIG_ADDRESS("ItemStack::getDamageValue"));
        return getDamageValue(this);
    } else {
        using getDamageValueFunc = short (__fastcall *)(Item *, void *);
        static auto getDamageValue = reinterpret_cast<getDamageValueFunc>(GET_SIG_ADDRESS("Item::getDamageValue"));
        return getDamageValue(this->mItem.get(), this->mUserData);
    }
}

short ItemStack::getMaxDamage() {
    using getMaxDamageFunc = short(__fastcall*)(ItemStack*);
    static auto getMaxDamage = reinterpret_cast<getMaxDamageFunc>(GET_SIG_ADDRESS("ItemStack::getMaxDamage"));
    return getMaxDamage(this);
}

void ItemStack::loadItem(const CompoundTag& tag) {
    // Load item data from NBT compound tag
    // This calls the game's internal _loadItem function
    static auto loadItemSig = GET_SIG_ADDRESS("ItemStack::_loadItem");

    if (loadItemSig == 0) {
        return;
    }

    using loadItemFunc = void(__fastcall*)(ItemStack*, const CompoundTag&);
    static auto _loadItem = reinterpret_cast<loadItemFunc>(loadItemSig);
    _loadItem(this, tag);
}

bool ItemStack::loadComponents(const CompoundTag& tag) {
    static auto loadComponentsSig = GET_SIG_ADDRESS("ItemStackBase::_loadComponents");
    if (loadComponentsSig == 0) {
        return false;
    }

    using loadComponentsFunc = void(__fastcall*)(ItemStack*, const CompoundTag&);
    static auto _loadComponents = reinterpret_cast<loadComponentsFunc>(loadComponentsSig);
    _loadComponents(this, tag);
    return true;
}

bool ItemStack::initFromBlock(BlockLegacy* legacy) {
    if (!legacy) {
        return false;
    }

    static auto initFromBlockSig = GET_SIG_ADDRESS("ItemStack::_init_fromBlock");
    if (initFromBlockSig == 0) {
        return false;
    }

    using initFromBlockFunc = void(__fastcall*)(ItemStack*, BlockLegacy*);
    static auto _initFromBlock = reinterpret_cast<initFromBlockFunc>(initFromBlockSig);
    _initFromBlock(this, legacy);
    return true;
}

float* ItemStack::getTextureAtlasSprite(int frame, int aux) {
    // Returns a pointer to a 5-element UV float array for the item's atlas sprite.
    // The game stores the result as a TextureUVCoordinateSet on the stack but returns
    // a pointer that is valid for the lifetime of the call chain.
    using func_t = float* (__fastcall *)(ItemStack *, int, int);
    static func_t efunc = reinterpret_cast<func_t>(GET_SIG_ADDRESS("ItemStack::getTextureAtlasSprite"));
    if (efunc)
        return efunc(this, frame, aux);
    return nullptr;
}
