#pragma once
#include <string>
#include "SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include <thread>

#include "Client.hpp"

class Cosmetic
{
    int CurrentFrames = 0;
public:
    std::string name;
    bool animated;
    int FPS;
    int Frames;
    BedrockTextureData texture;

    Cosmetic(std::string Name, bool Animated = false, int fps = 0, int frames = 0): name(Name), animated(Animated), FPS(fps), Frames(frames)
    {
        if (animated)
        {
            std::thread t([this]()
            {
                while (!Client::disable)
                {
                    std::string Path = Utils::getClientPath() + "\\Cosmetics\\" + name + "\\" + name + "_" + std::to_string(CurrentFrames) + ".png";
                    if (SDK::clientInstance and SDK::clientInstance->getMinecraftGame() and SDK::clientInstance->getMinecraftGame()->textureGroup)
                    {
                        ResourceLocation loc(Path, true);
                        TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);

                        texture = *ptr.clientTexture.get();
                    }

                    CurrentFrames++;

                    if (CurrentFrames == Frames)
                    {
                        CurrentFrames = 0;
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
                }
            });

            t.detach();
        }
    };
};
