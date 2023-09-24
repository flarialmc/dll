#include "ActorMovementProxy.hpp"

void ActorMovementProxy::SetRotation(Vec2<float>* rot) {
	return Memory::CallVFunc<57, void>(this, rot);
}
