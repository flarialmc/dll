#include "ActorMovementProxy.hpp"

void ActorMovementProxy::SetRotation(Vec2<float>& rot) {
	return Memory::CallVFunc<74, void>(this, rot);
}
void ActorMovementProxy::SetYHeadRotation(float rotY) {
	return Memory::CallVFunc<150, void>(this, rotY);
}
