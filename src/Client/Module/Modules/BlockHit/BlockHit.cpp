#include "BlockHit.hpp"

#include "Events/EventManager.hpp"
#include "glm/glm/ext/matrix_transform.hpp"

void BlockHit::onEnable() {
    Module::onEnable();
    Listen(this, RenderItemInHandEvent, &BlockHit::onItemInHandRender);
    Listen(this, PerspectiveEvent, &BlockHit::onPerspectiveChange);
}

void BlockHit::onDisable() {
    Deafen(this, RenderItemInHandEvent, &BlockHit::onItemInHandRender);
    Deafen(this, PerspectiveEvent, &BlockHit::onPerspectiveChange);
    Module::onDisable();
}

void BlockHit::onItemInHandRender(RenderItemInHandEvent &event) {
    if (!this->isEnabled()) return;
    auto& matrix = SDK::clientInstance->getCamera().getWorldMatrixStack().top().matrix;
    auto itemStack = event.itemStack;

    if (itemStack->item.get() != nullptr && CPSCounter::GetRightHeld() && itemStack->getItem()->name.contains("sword")) {

        switch (perspective) {
            case Perspective::FirstPerson:

                matrix = glm::translate<float>(matrix, glm::vec3(-0.5f, 0.2f, 0.0f));
                matrix = glm::rotate<float>(matrix, glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f));
                matrix = glm::rotate<float>(matrix, glm::radians(-80.f), glm::vec3(1.f, 0.f, 0.f));
                matrix = glm::rotate<float>(matrix, glm::radians(60.f), glm::vec3(0.f, 1.f, 0.f));

                break;

            case Perspective::ThirdPersonBack:

                matrix = glm::translate<float>(matrix, glm::vec3(-0.5f, 0.2f, 0.2f));
                matrix = glm::rotate<float>(matrix, glm::radians(105.f), glm::vec3(0.f, 1.f, 0.f));
                matrix = glm::rotate<float>(matrix, glm::radians(-100.f), glm::vec3(1.f, 0.f, 0.f));
                matrix = glm::rotate<float>(matrix, glm::radians(130.f), glm::vec3(0.f, 1.f, 0.f));

                break;

            case Perspective::ThirdPersonFront:

                matrix = glm::translate<float>(matrix, glm::vec3(-0.5f, 0.2f, 0.2f));
                matrix = glm::rotate<float>(matrix, glm::radians(105.f), glm::vec3(0.f, 1.f, 0.f));
                matrix = glm::rotate<float>(matrix, glm::radians(-100.f), glm::vec3(1.f, 0.f, 0.f));
                matrix = glm::rotate<float>(matrix, glm::radians(130.f), glm::vec3(0.f, 1.f, 0.f));

                break;
        }
    }
}

void BlockHit::defaultConfig() {
    Module::defaultConfig("core");
    saveSettings();
}

void BlockHit::onPerspectiveChange(PerspectiveEvent &event)  {
    if (!this->isEnabled()) return;
    this->perspective = event.getPerspective();
}