#include "SwingAnimations.hpp"

#include "glm/glm/ext/matrix_transform.hpp"

void SwingAnimations::onEnable() {
    Module::onEnable();
    Listen(this, RenderItemInHandEvent, &SwingAnimations::onItemInHandRender);
    Listen(this, PerspectiveEvent, &SwingAnimations::onPerspectiveChange);
    Listen(this, SwingDurationEvent, &SwingAnimations::onSwingDuration);

    previousFluxSwingState = getOps<bool>("fluxSwing");
    if (previousFluxSwingState && !isPatched) {
        patch();
        isPatched = true;
    }

    initializeSwingAngle();
    previousSwingAngle = getOps<float>("swingAngle");
    updateSwingAngle(previousSwingAngle);
}

void SwingAnimations::onDisable() {
    Deafen(this, RenderItemInHandEvent, &SwingAnimations::onItemInHandRender);
    Deafen(this, PerspectiveEvent, &SwingAnimations::onPerspectiveChange);
    Deafen(this, SwingDurationEvent, &SwingAnimations::onSwingDuration);
    Module::onDisable();

    if (isPatched) {
        unpatch();
        isPatched = false;
    }

    restoreSwingAngle();
}

void SwingAnimations::onItemInHandRender(RenderItemInHandEvent &event) {
    if (!this->isEnabled()) return;

    bool currentFluxSwingState = getOps<bool>("fluxSwing");
    float currentSwingAngle = getOps<float>("swingAngle");

    if (currentFluxSwingState != previousFluxSwingState) {
        if (currentFluxSwingState && !isPatched) {
            patch();
            isPatched = true;
        } else if (!currentFluxSwingState && isPatched) {
            unpatch();
            isPatched = false;
        }
        previousFluxSwingState = currentFluxSwingState;
    }

    if (currentSwingAngle != previousSwingAngle) {
        updateSwingAngle(currentSwingAngle);
        previousSwingAngle = currentSwingAngle;
    }
}

void SwingAnimations::onSwingDuration(SwingDurationEvent &event) {
    if (!this->isEnabled()) return;

    float swingSpeed = getOps<float>("swingSpeed");
    int originalDuration = event.getDuration();
    int modifiedDuration = static_cast<int>(originalDuration / swingSpeed);

    event.setDuration(modifiedDuration);
}

void SwingAnimations::defaultConfig() {
    Module::defaultConfig("core");
    setDef<float>("swingSpeed", 1.0f);
    setDef<bool>("fluxSwing", true);
    setDef<float>("swingAngle", -80.0f);
}

void SwingAnimations::settingsRender(float settingsOffset) {
    initSettingsPage();

    addSlider("Swing Speed", "Controls the speed of swing animations", "swingSpeed", 3.0f, 0.1f);

    bool currentFluxSwingState = getOps<bool>("fluxSwing");
    addToggle("Flux Swing", "Enables flux swing patching for smoother animations", "fluxSwing");
    bool newFluxSwingState = getOps<bool>("fluxSwing");

    if (currentFluxSwingState != newFluxSwingState) {
        if (newFluxSwingState && !isPatched) {
            patch();
            isPatched = true;
        } else if (!newFluxSwingState && isPatched) {
            unpatch();
            isPatched = false;
        }
        previousFluxSwingState = newFluxSwingState;
    }

    float currentSwingAngle = getOps<float>("swingAngle");
    addSlider("Swing Angle", "Adjusts the swing rotation angle in degrees", "swingAngle", 90.0f, -180.0f, false);
    float newSwingAngle = getOps<float>("swingAngle");

    if (currentSwingAngle != newSwingAngle) {
        updateSwingAngle(newSwingAngle);
        previousSwingAngle = newSwingAngle;
    }

    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void SwingAnimations::onPerspectiveChange(PerspectiveEvent &event) {
    if (!this->isEnabled()) return;
    this->perspective = event.getPerspective();
}