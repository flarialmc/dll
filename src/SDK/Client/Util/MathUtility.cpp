#include "MathUtility.hpp"

#include "../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

Vec2<float> mce::MathUtility::getRotationFromNegZToVector(const Vec3<float>& vec) {
    static auto sig = GET_SIG_ADDRESS("mce::MathUtility::getRotationFromNegZToVector");
    static auto func = *(decltype(&MathUtility::getRotationFromNegZToVector)*)&sig;
    return func(vec);
}
