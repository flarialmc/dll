#include "HitResult.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"
#include "../../../Utils/Versions/WinrtUtils.hpp"
#include "../../../Utils/Memory/Memory.hpp"

Actor *HitResult::getEntity() {
    static uintptr_t sig = NULL;
    if (sig == NULL) {
        if(WinrtUtils::check(20, 50)) {
            sig = GET_SIG_ADDRESS("HitResult::getEntity");
        } else {
            sig = Memory::offsetFromSig(GET_SIG_ADDRESS("HitResult::getEntity"), 1);
        }
    }
    static auto getEntity = *(decltype(&HitResult::getEntity)*)&sig;
    return (this->*getEntity)();
}

float HitResult::distance() const {
    return startPos.dist(pos);
}
