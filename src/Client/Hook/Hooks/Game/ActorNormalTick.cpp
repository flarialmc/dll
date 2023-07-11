#include "ActorNormalTick.hpp"
#include "../../../Events/EventHandler.hpp"
#include "../../../../SDK/SDK.hpp"

void ActorNormalTick::enableHook() {


    auto vft = Memory::findSig("48 8D 05 ?? ?? ?? ?? 48 89 01 49 8B 00 48 89 41 08");
    int xd = *reinterpret_cast<int*>(vft + 3);
    uintptr_t** vftREAL = reinterpret_cast<uintptr_t**>(vft + xd + 7);

    this->manualHook(vftREAL[44], callback, (void**)&xed);


}

ActorNormalTick::ActorNormalTick() : Hook("ActorNormalTickHook", "48 8D 05 ???? ?? ?? 48 89 01 49 88 00 48 89 41 08"){}

void ActorNormalTick::callback(Actor *xd) {

    if(xd != nullptr) {
        xed(xd);
        TickEvent event(xd);
        EventHandler::onTick(event);
    }

    

}


