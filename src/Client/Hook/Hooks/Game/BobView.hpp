#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

class BobView : public Hook {
private:
    static void BobViewhook(uint64_t _this, glm::mat4* matrix) {
        auto event = nes::make_holder<BobViewEvent>();
        event->matrix = matrix;
        eventMgr.trigger(event);

        if (GetAsyncKeyState(VK_RBUTTON)) {
            *matrix = glm::translate<float>(*matrix, glm::vec3(0.42222223281, 0.0, -0.16666666269302368));
            *matrix = glm::translate<float>(*matrix, glm::vec3(-0.1f, 0.15f, -0.2f));
            *matrix = glm::translate<float>(*matrix, glm::vec3(-0.24F, 0.25f, -0.20F));
            *matrix = glm::rotate<float>(*matrix, -1.98F, glm::vec3(0.0F, 1.0F, 0.0F));
            *matrix = glm::rotate<float>(*matrix, 1.30F, glm::vec3(4.0F, 0.0F, 0.0F));
            *matrix = glm::rotate<float>(*matrix, 60.0F, glm::vec3(0.0F, 1.0F, 0.0F));
        }

        return funcOriginal(_this, matrix);
    }

public:
    typedef void(__thiscall* original)(uint64_t _this, glm::mat4* matrix);

    static inline original funcOriginal = nullptr;

    BobView() : Hook("BobViewHook", GET_SIG_ADDRESS("BobView")) {}

    void enableHook() override {
        this->autoHook((void*)BobViewhook, (void**)&funcOriginal);
    }
};