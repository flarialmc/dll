#pragma once

#include "MobEffect.h"
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

std::unique_ptr<MobEffect, std::default_delete<MobEffect>> *MobEffect::getEffects() {
    static auto ref = Memory::findSig(
            GET_SIG("MobEffect::getEffects"));
    int off = *reinterpret_cast<int *>(ref + 3);
    return reinterpret_cast<std::unique_ptr<MobEffect, std::default_delete<MobEffect>> *>(ref + off + 7);
}