#include "RenderActor.hpp"


void RenderActorHook::callback(void* a1, void* a2 , Actor* actor, Vec3<float> a4, Vec3<float> distanceFromCamera, Vec2<float>* a6, void* a7) {

	
	//a//6.x = 3.0f;
	//a6.y = 10.0f;
	//if (actor->getEntityTypeId() == 64) {

		//actor->actorRotationComponent->Rotation.x = 0.0f;
		//Vec2<float> rot = Vec2<float>(1.0f, 1.0f);
		//a4.y = 30.0f;
		//renderActorOriginal(a1, a2, actor, a4, distanceFromCamera, a6, a7);
	//}
	//else {
		renderActorOriginal(a1, a2, actor, a4, distanceFromCamera, a6, a7);
	//}
}

void RenderActorHook::enableHook() {
	this->autoHook(callback, (void**)&renderActorOriginal);
}
