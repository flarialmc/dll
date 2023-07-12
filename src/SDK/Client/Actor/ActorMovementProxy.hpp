#pragma once

#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Utils.hpp"

class ActorMovementProxy {
public:
	//D60                 dq offset ?setRotation@?$DirectActorProxyImpl@UIMobMovementProxy@@@@UEAAXAEBVVec2@@@Z ; DirectActorProxyImpl<IMobMovementProxy>::setRotation(Vec2 const &)
	void SetRotation(Vec2<float>* rot);
};