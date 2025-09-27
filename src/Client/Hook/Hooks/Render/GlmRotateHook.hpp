#pragma once

#include "../Hook.hpp"
#include <glm/glm/glm.hpp>

class GlmRotateHook : public Hook {
private:
    static void GlmRotateCallback(glm::mat4x4& mat, float angle, float x, float y, float z);
    static glm::mat4x4 GlmRotateCallback2(glm::mat4x4& mat, float angle, const glm::vec3& axis);

public:
    typedef void(__fastcall* GlmRotateOriginal)(glm::mat4x4&, float, float, float, float);
    typedef glm::mat4x4(__fastcall* GlmRotateOriginal2)(glm::mat4x4&, float, const glm::vec3&);

    static inline GlmRotateOriginal funcOriginal = nullptr;
    static inline GlmRotateOriginal2 funcOriginal2 = nullptr;

    GlmRotateHook();

    void enableHook() override;
};