#pragma once

#include "../Actor/Actor.hpp"
#include "../../../Utils/Utils.hpp"

struct ActorRenderData {
    Actor* actor;
    void* extraData;
    Vec3<float> position;
    Vec2<float> rotation;
    Vec2<float> headRot;
    bool glint;
    bool ignoreLighting;
    bool isInUI;
    float deltaTime;
    int modelObjId;
    float modelSize;
    //AnimationComponent *animationComponent;
    //MolangVariableMap *variables;

private:
    char pad[0x10];
};
