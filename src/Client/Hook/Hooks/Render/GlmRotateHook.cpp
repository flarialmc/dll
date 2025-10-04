#include "GlmRotateHook.hpp"
#include "../../../Events/EventManager.hpp"
#include "../../../Events/Render/GlmRotateEvent.hpp"
#include <glm/glm/ext/matrix_transform.hpp>

void GlmRotateHook::GlmRotateCallback(glm::mat4x4& mat, float angle, float x, float y, float z) {
    auto event = nes::make_holder<GlmRotateEvent>(mat, angle, glm::vec3(x, y, z));
    eventMgr.trigger(event);

    if (!event->isCancelled()) {
        funcOriginal(mat, angle, x, y, z);
    }
}

glm::mat4x4 GlmRotateHook::GlmRotateCallback2(glm::mat4x4& mat, float angle, const glm::vec3& axis) {
    auto event = nes::make_holder<GlmRotateEvent>(mat, angle, axis);
    eventMgr.trigger(event);

    if (!event->isCancelled()) {
        return funcOriginal2(mat, angle, axis);
    }
    return mat;
}

GlmRotateHook::GlmRotateHook() : Hook("Glm Rotate Hook", GET_SIG_ADDRESS("glm_rotateRef")) {}

void GlmRotateHook::enableHook() {
    if (VersionUtils::checkAboveOrEqual(21, 40)) {
        this->autoHook((void*)GlmRotateCallback2, (void**)&funcOriginal2);
    } else {
        this->autoHook((void*)GlmRotateCallback, (void**)&funcOriginal);
    }
}