#pragma once
#include "../Module.hpp"
#include <chrono>
#include <algorithm>


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

        lastPos = *lp->getPosition();
        if (auto rc = lp->getActorRotationComponent()) {
            lastYaw = rc->rot.y;
            lastPitch = rc->rot.x;
        }

        transX = 0.0f;
        transY = 0.0f;
        jumpOffset = 0.0f;
        xVelocity = 0.0f;
        yVelocity = 0.0f;
        zVelocity = 0.0f;

        lastUpdateTime = std::chrono::high_resolution_clock::now();

        speedFactor      = getOps<float>("velocityfactor");
        jumpMultiplier   = getOps<float>("jumpvelocityfactor");
    }

    void onDisable() override {
        Deafen(this, RenderEvent,   &JavaViewBobbing::onRender);
        Deafen(this, BobHurtEvent,  &JavaViewBobbing::onBobHurt);
        Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig("core");
        setDef("velocityfactor", 1.0f);
        setDef("jumpvelocityfactor", 4.0f);
    }

    void settingsRender(float settingsOffset) override {
        initSettingsPage();
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

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> dt_duration = now - lastUpdateTime;
        float dt = dt_duration.count();
        if (dt <= 0.0f) return;

        Vec3<float> curPos = *lp->getPosition();

        if (lastPos.x != 0.0f || lastPos.y != 0.0f || lastPos.z != 0.0f) {
            xVelocity = (curPos.x - lastPos.x) / dt;
            yVelocity = (curPos.y - lastPos.y) / dt;
            zVelocity = (curPos.z - lastPos.z) / dt;
        }
        lastPos = curPos;
        lastUpdateTime = now;

        float outSpeed = 0.025f;

        float currentYaw = rc->rot.y;
        float currentPitch = rc->rot.x;

        float deltaYaw = lastYaw - currentYaw;
        if (deltaYaw < -180.0f) deltaYaw += 360.0f;
        else if (deltaYaw >  180.0f) deltaYaw -= 360.0f;

        float deltaPitch = -(lastPitch - currentPitch);

        float xTarget = (deltaYaw * 0.1f) * speedFactor;
        float yTarget = (deltaPitch * 0.1f) * speedFactor;

        transX = transX + (xTarget - transX) * outSpeed;
        transY = transY + (yTarget - transY) * outSpeed;

        constexpr float inBase = 0.0625f;
        float inSpeed = inBase * jumpMultiplier;
        constexpr float velocityMultiplier = 0.1f;

        float yValueSet = transY - yVelocity * dt * inSpeed * velocityMultiplier;
        jumpOffset = std::clamp(yValueSet, -0.3f, 0.3f);

        lastYaw = currentYaw;
        lastPitch = currentPitch;
    }

    void onBobHurt(BobHurtEvent& ev) {
        glm::mat4 m = *ev.matrix;


        m = glm::translate(m, glm::vec3(transX, jumpOffset, 0.0f));
        *ev.matrix = m;
    }

private:
    Vec3<float> lastPos{};
    float lastYaw = 0.0f;
    float lastPitch = 0.0f;

    std::chrono::high_resolution_clock::time_point lastUpdateTime;

    float transX = 0.0f;
    float transY = 0.0f;
    float jumpOffset = 0.0f;

    float xVelocity = 0.0f;
    float yVelocity = 0.0f;
    float zVelocity = 0.0f;

    float speedFactor = 1.0f;
    float jumpMultiplier = 4.0f;

    template <typename T>
    T lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }
};