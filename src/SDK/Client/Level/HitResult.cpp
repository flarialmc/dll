#include "HitResult.hpp"
#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

Actor *HitResult::getEntity() {
    static auto sig = GET_SIG("HitResult::getEntity");
    static auto getEntity = *(decltype(&HitResult::getEntity)*)&sig;
    return (this->*getEntity)();
}

float HitResult::distance() const {
    return startPos.dist(pos);
}
