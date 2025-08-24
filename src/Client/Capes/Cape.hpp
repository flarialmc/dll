#pragma once

#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <ImGui/stb.h>

#include "Client.hpp"
#include "Events/EventManager.hpp"
#include "SDK/Client/Network/Packet/PlayerList.hpp"
#include "SDK/Client/Network/Packet/PlayerSkinPacket.hpp"
#include "Utils/Logger/Logger.hpp"

class Cape
{
public:
    static Image GetCapeFromPng(int id)
    {
        HRSRC imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(id), "PNG");
        if (!imageResHandle) return Image();

        HGLOBAL imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
        if (!imageResDataHandle) return Image();

        const unsigned char* pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
        if (!pImageFile) return Image();

        DWORD imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
        if (imageFileSize == 0) return Image();

        int width, height, channels;
        unsigned char* image_data = stbi_load_from_memory(pImageFile, imageFileSize, &width, &height, &channels, 4)

        std::vector<unsigned char> rgba_data(image_data, image_data + (width * height * 4));
        stbi_image_free(image_data);

        Image Cape;

        Cape.mHeight = 32;
        Cape.mWidth = 64;
        Cape.mDepth = 0;
        Cape.mUsage = (ImageUsage)1;
        Cape.imageFormat = (ImageFormat)4;
        Cape.mImageBytes = Blob::fromVector(rgba_data);

        return Cape;
    }
};