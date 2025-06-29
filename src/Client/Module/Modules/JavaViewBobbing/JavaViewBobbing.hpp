#pragma once
#include "../Module.hpp"
#include <chrono> // For high-resolution clock
#include <algorithm> // For std::clamp

// Assuming SDK::clientInstance, SDK::clientInstance->getLocalPlayer(),
// lp->getPosition(), lp->getActorRotationComponent(), rc->rot.x, rc->rot.y
// are correctly providing player position and rotation in the expected units.
// Also assuming Vec3<float> is compatible with glm::vec3 if needed for calculations.

class JavaViewBobbing : public Module {
public:
    JavaViewBobbing()
      : Module("Java View Bobbing",
               "Makes the view bobbing exactly like Java Edition.",
               IDR_EYE_PNG, "", false)
    {}

    void onEnable() override {
        Listen(this, RenderEvent,   &JavaViewBobbing::onRender);
        Listen(this, BobHurtEvent,  &JavaViewBobbing::onBobHurt);
        Module::onEnable();

        if (!SDK::clientInstance) return;
        auto lp = SDK::clientInstance->getLocalPlayer();
        if (!lp) return;

        // Initialize last position and rotation
        lastPos = *lp->getPosition();
        if (auto rc = lp->getActorRotationComponent()) {
            lastYaw = rc->rot.y;
            lastPitch = rc->rot.x;
        }

        // Initialize velocities and offsets
        transX = 0.0f;
        transY = 0.0f;
        jumpOffset = 0.0f;
        xVelocity = 0.0f;
        yVelocity = 0.0f;
        zVelocity = 0.0f;

        // Initialize time
        lastUpdateTime = std::chrono::high_resolution_clock::now();

        speedFactor      = getOps<float>("velocityfactor");
        jumpMultiplier   = getOps<float>("jumpvelocityfactor");
    }

    void onDisable() override {
        Deafen(this, RenderEvent,   &JavaViewBobbing::onRender);
        Deafen(this, BobHurtEvent,  &JavaViewBobbing::onBobHurt);
        Module::onDisable();

        // Reset view model settings if necessary, similar to Lua's onDisable
        // This part needs to interact with your specific view model module/settings
        // For now, assuming it's handled externally or not strictly required for this module's state.
        // If you have a way to access/reset the translation/rotation values of the view model,
        // you would do it here. Example:
        // if (SDK::clientInstance && SDK::clientInstance->getViewModel()) {
        //     SDK::clientInstance->getViewModel()->setTranslationX(0.0f);
        //     SDK::clientInstance->getViewModel()->setTranslationY(0.0f);
        //     SDK::clientInstance->getViewModel()->setTranslationZ(0.0f);
        // }
    }

    void defaultConfig() override {
        Module::defaultConfig("core");
        setDef("velocityfactor", 1.0f);
        setDef("jumpvelocityfactor", 4.0f);
    }

    void settingsRender(float settingsOffset) override {
        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");
        const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);
        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));
        addHeader("Java View Bobbing");
        addSlider("Velocity Factor", "Speed of the hand moving", "velocityfactor", 10, 0.1);
        addSlider("Jump Velocity Factor", "The factor jumping has on the hand movement", "jumpvelocityfactor", 10, 0);
        FlarialGUI::UnsetScrollView();
        resetPadding();
    }

    void onRender(RenderEvent& ev) {
        if (!SDK::clientInstance) return;
        auto lp = SDK::clientInstance->getLocalPlayer();
        if (!lp) return;
        auto rc = lp->getActorRotationComponent();
        if (!rc) return;

        speedFactor      = getOps<float>("velocityfactor");
        jumpMultiplier   = getOps<float>("jumpvelocityfactor");

        // Calculate delta time
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> dt_duration = now - lastUpdateTime;
        float dt = dt_duration.count();
        if (dt <= 0.0f) return;

        // --- Calculate velocity (matching Lua's calculateVelocity) ---
        Vec3<float> curPos = *lp->getPosition();

        if (lastPos.x != 0.0f || lastPos.y != 0.0f || lastPos.z != 0.0f) { // Check if lastPos has been initialized
            xVelocity = (curPos.x - lastPos.x) / dt;
            yVelocity = (curPos.y - lastPos.y) / dt;
            zVelocity = (curPos.z - lastPos.z) / dt;
        }
        lastPos = curPos;
        lastUpdateTime = now;

        // --- View bobbing logic (matching Lua's render3d) ---
        float outSpeed = 0.025f;

        float currentYaw = rc->rot.y;
        float currentPitch = rc->rot.x;

        float deltaYaw = lastYaw - currentYaw;
        if (deltaYaw < -180.0f) deltaYaw += 360.0f;
        else if (deltaYaw >  180.0f) deltaYaw -= 360.0f;

        float deltaPitch = -(lastPitch - currentPitch); // Lua script uses -(prevPitch - pitch)

        // These are the target values, not the final blended values yet
        float xTarget = (deltaYaw * 0.1f) * speedFactor;
        float yTarget = (deltaPitch * 0.1f) * speedFactor;

        // Lerp towards the target values
        // Note: std::lerp is C++20. If you are on an older standard, implement it or use a math library's version.
        // float alpha = outSpeed; // Lua's lerp directly uses 'outSpeed' as the interpolation factor
        // The Lua script does `math.lerp(outSpeed, current, target)`. This is equivalent to `current + (target - current) * outSpeed`.
        // Your current `alpha = outSpeedPerFrame * (1.0f / (60.0f * dt))` is for fixed framerate independent blending.
        // To match Lua, we'll use a simpler blend based on `outSpeed`.
        transX = transX + (xTarget - transX) * outSpeed;
        transY = transY + (yTarget - transY) * outSpeed;
        // z translation is always 0 in Lua, so we don't need a transZ.

        // --- Jump velocity factor (matching Lua's render3d) ---
        constexpr float inBase = 0.0625f;
        float inSpeed = inBase * jumpMultiplier;
        constexpr float velocityMultiplier = 0.1f;

        // The Lua script applies a Y offset based on yVelocity here
        // modSettings.translation.y.value = math.clamp(yValueSet, -0.3, 0.3)
        // Where yValueSet = (modSettings.translation.y.value - localYVelocity * dt * inSpeed * (velocityMultiplier))
        // This means the jumpOffset is applied to the Y translation *before* the clamping.

        // Re-evaluate jumpOffset calculation to match Lua more closely
        float yValueSet = transY - yVelocity * dt * inSpeed * velocityMultiplier;
        jumpOffset = std::clamp(yValueSet, -0.3f, 0.3f);
        // Note: Lua's clamp is applied to the full y translation, not just an offset.
        // We'll apply this `jumpOffset` to `transY` in `onBobHurt`.

        lastYaw = currentYaw;
        lastPitch = currentPitch;
    }

    void onBobHurt(BobHurtEvent& ev) {
        glm::mat4 m = *ev.matrix;

        // Apply the accumulated translations and the jump offset
        // The Lua script applies the jump factor *directly* to the Y translation
        // and then clamps it. Your current implementation adds jumpOffset to transY.
        // Let's ensure transY effectively includes the jump component for the clamp.
        // Since `onBobHurt` gets the final matrix, we apply `transX` and `transY`
        // which now has the combined effect from `onRender`.
        m = glm::translate(m, glm::vec3(transX, jumpOffset, 0.0f)); // `jumpOffset` now holds the clamped Y value.
        *ev.matrix = m;
    }

private:
    Vec3<float> lastPos{};
    float lastYaw = 0.0f;
    float lastPitch = 0.0f;

    std::chrono::high_resolution_clock::time_point lastUpdateTime; // Use for more accurate time

    float transX = 0.0f;
    float transY = 0.0f; // This will hold the combined Y translation including bobbing and jump
    float jumpOffset = 0.0f; // This now holds the clamped final Y translation component from jump

    float xVelocity = 0.0f;
    float yVelocity = 0.0f;
    float zVelocity = 0.0f;

    float speedFactor = 1.0f;
    float jumpMultiplier = 4.0f;

    // Helper for std::lerp if C++20 is not available
    template <typename T>
    T lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }
};