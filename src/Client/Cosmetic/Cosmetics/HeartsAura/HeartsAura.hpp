#pragma once
#include "../Cosmetic.hpp"
#include "Events/EventManager.hpp"
#include <Utils/Render/DrawUtil3D.hpp>
#include "../../ModelLoader.hpp"
#include <random>

std::unique_ptr<cosmetic::JsonModel> model;

struct HeartsAuraParticle
{
public:
    Vec3<float> position;
    Vec3<float> origin;
    float MaxHeight;
    float size = 0.0;
    float rotation;
    std::chrono::time_point<std::chrono::high_resolution_clock> CreatedAt = std::chrono::high_resolution_clock::now();

    HeartsAuraParticle()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::bernoulli_distribution d(0.5);

        bool negativeX = d(gen);
        bool negativeZ = d(gen);


        const double inner_radius = 0.5;
        const double outer_radius = 1.0;

        std::uniform_real_distribution<> angle_dist(0, 2 * M_PI);
        std::uniform_real_distribution<> radius_dist(0, 1);

        // Generate random angle between 0 and 2*pi
        double angle = angle_dist(gen);

        // Generate random radius with sqrt to ensure uniform distribution in area
        double radius = std::sqrt(radius_dist(gen) * (outer_radius * outer_radius - inner_radius * inner_radius) + inner_radius * inner_radius);

        // Calculate x, y coordinates
        double x = radius * std::cos(angle);
        double y = radius * std::sin(angle);
        std::uniform_real_distribution<> disY(-2, -1.5);

        std::uniform_real_distribution<> disYMax(-0.5, 0);

        std::uniform_real_distribution<> disRot(0, 45);

        origin.x = position.x = x;
        origin.z = position.z = y;
        origin.y = position.y = disY(gen);

        MaxHeight = disYMax(gen);
        rotation = disRot(gen);

    }

    void render(Vec3<float> PlayerPos)
    {
        ResourceLocation loc4(Utils::getClientPath() + "\\Cosmetics\\HeartAura\\texture.png", true);
        TexturePtr Texture = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc4, false);

        model->render(SDK::scn, Texture, position.add(PlayerPos), Vec3<float>(0, rotation, 0), Vec3<float>(size, size, size), true);

        FlarialGUI::lerp(position.y, MaxHeight, 0.015 * FlarialGUI::frameFactor);
        if ((std::chrono::high_resolution_clock::now() - CreatedAt).count() < 1)
            FlarialGUI::lerp(size, 1.f, 0.06 * FlarialGUI::frameFactor);
        else
        {
            FlarialGUI::lerp(size, 0.045f, 0.04 * FlarialGUI::frameFactor);
        }

        FlarialGUI::lerp(rotation, 360.f, 0.005 * FlarialGUI::frameFactor);
    }
};

class HeartAura: public Cosmetic {

public:
    using clock = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<double>;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_time = clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> last_add = clock::now();
    std::vector<HeartsAuraParticle> particles;
    HeartAura(): Cosmetic("HeartAura")
    {
        Listen(this, Render3DEvent, &HeartAura::onRender3D)
        model = cosmetic::ModelLoader::LoadFromFile(Utils::getClientPath() + "\\Cosmetics\\HeartAura\\HeartAura.geo.json", "geometry.HeartAura");

    }

    void onRender3D(Render3DEvent& event)
    {

        Vec3<float> playerPos = SDK::clientInstance->getLocalPlayer()->getLerpedPosition();


        auto current_time = clock::now();
        duration elapsed = current_time - last_time;

        if (elapsed.count() >= 0.2) {
            particles.push_back(HeartsAuraParticle());
            particles.erase(particles.begin());

            last_time = current_time;
        }

        else if ((current_time - last_add).count() >= .1 and particles.size() < 16)
        {
            particles.push_back(HeartsAuraParticle());
            last_add = current_time;
        }

        for (auto& p : particles)
        {
            p.render(playerPos);
        }
    }
};