#pragma once

#include <Utils/Memory/Memory.hpp>
#include <Utils/Utils.hpp>

struct __declspec(align(8)) NameTagRenderObject {
    std::string nameTag;
    std::shared_ptr<uintptr_t> mesh;
    void* tagMat;
    void* mTextMatOverride;
    MCCColor tagColor;
    MCCColor textColor;
    Vec3<float> pos;
};