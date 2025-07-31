#pragma once
#include "../../../Events/Events.hpp"
#include "../Hook.hpp"
#include "../../../../Utils/Memory/Game/SignatureAndOffsetManager.hpp"

#include <ImGui/stb.h>

#include <vector>
#include <string>
#include <stdexcept>

Image loadImage(const std::string& filename, int reqChannels = 4) {
    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, reqChannels);
    if (!data) {
        Logger::error(std::string("Failed to load image: ") + stbi_failure_reason());
    }

    // Fill the image structure
    Image img;
    img.mWidth = width;
    img.mHeight = height;
    img.mDepth = 0;
    img.imageFormat = ImageFormat::RGBA8Unorm;
    img.mUsage = (ImageUsage)0x3;
    img.mImageBytes = Blob::fromVector(std::vector<unsigned char>(data, data + (sizeof(data) / sizeof(data[0]))));
    stbi_image_free(data);
    return img;
}

class PlayerUpdateSkin : public Hook {
private:
    static void ApplyTurnDelta(Player* a1, PlayerSkin const& skin, int clientSubID) {

        funcOriginal(a1, skin, clientSubID);
    }

public:
    typedef void(__thiscall* original)(Player* a1, PlayerSkin const& skin, int clientSubID);

    static inline original funcOriginal = nullptr;

    PlayerUpdateSkin() : Hook("PlayerUpdateSkin", GET_SIG_ADDRESS("Player::UpdateSkin")) {}

    void enableHook() override {
        this->autoHook((void*)ApplyTurnDelta, (void**)&funcOriginal);
    }
};