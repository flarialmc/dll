#pragma once
#include "../Hook.hpp"
#include <Utils/Utils.hpp>
class SoundEnginePlayHook : public Hook {

private:
    static uint64_t callback(void* SoundEngine, const std::string& name, Vec3<float> a3, float a4, float a5);

public:
    typedef uint64_t(__thiscall *original)(void* SoundEngine, const std::string& name, Vec3<float> a3, float a4, float a5);

    static inline original funcOriginal = nullptr;

    SoundEnginePlayHook();

    void enableHook() override;
};
