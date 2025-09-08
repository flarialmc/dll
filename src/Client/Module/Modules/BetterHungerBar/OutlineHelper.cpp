// TODO: (important) - Get the full resource path in 26 and 75 so stbi_load can find the file!!

#include "BetterHungerBar.hpp"

#include <chrono>
#include <filesystem>
#include <ImGui/stb.h>
#include <stb_image_write.h>

static std::chrono::steady_clock::time_point lastProcessTime;
static ResourceLocation cachedOutline;
static bool cacheInitialized = false;

ResourceLocation BetterHungerBar::getOutlineU(ResourceLocation& originalOutlineTexture)
{
    auto currentTime = std::chrono::steady_clock::now();
    
    // Check if we need to reprocess (every 10s)
    if (cacheInitialized) {
        auto timeSinceLastProcess = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastProcessTime);
        if (timeSinceLastProcess.count() < 10) {
            return cachedOutline;
        }
    }
    
    std::string texturePath = originalOutlineTexture.filePath; // FIXME: how do you get the full path?
    
    // Load the image
    OutlineImage outline(texturePath);

    if (outline.PixelData.empty() || outline.Size <= 0) { 
        return originalOutlineTexture;
    }

    // Process the image to keep only white non-transparent pixels
    for (int i = 0; i < outline.Size * outline.Size; i++) {
        unsigned char r = outline.PixelData[i].x;
        unsigned char g = outline.PixelData[i].y;
        unsigned char b = outline.PixelData[i].z;
        unsigned char a = outline.PixelData[i].w;

        if (!(r > 245 && g > 245 && b > 245) || a == 0) {
            // Set non-white or transparent pixels to fully transparent
            outline.PixelData[i] = Vec4<int>(0, 0, 0, 0);
        }
    }

    outline.SaveImage("hunger_blink_cleaned");

    std::string newPath = Utils::getClientPath() + "//Cache//hunger_blink_cleaned.png";

    lastProcessTime = currentTime;
    cachedOutline = ResourceLocation(newPath, true);
    cacheInitialized = true;
    
    return cachedOutline;
}

static std::chrono::steady_clock::time_point lastProcessTime2;
static ResourceLocation cachedOutline2;
static bool cacheInitialized2 = false;

ResourceLocation BetterHungerBar::getOutlineG(ResourceLocation& originalOutlineTexture)
{
    auto currentTime = std::chrono::steady_clock::now();
    
    // Check if we need to reprocess (every 6s)
    if (cacheInitialized2) {
        auto timeSinceLastProcess = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastProcessTime2);
        if (timeSinceLastProcess.count() < 6) {
            return cachedOutline2;
        }
    }
    
    std::string texturePath = originalOutlineTexture.filePath; // FIXME: how do you get the full path?
    
    // Load the image
    OutlineImage outline(texturePath);

    if (outline.PixelData.empty() || outline.Size <= 0) { 
        return originalOutlineTexture;
    }

    // Process the image to keep only white non-transparent pixels
    for (int i = 0; i < outline.Size * outline.Size; i++) {
        unsigned char r = outline.PixelData[i].x;
        unsigned char g = outline.PixelData[i].y;
        unsigned char b = outline.PixelData[i].z;
        unsigned char a = outline.PixelData[i].w;

        if (!(r > 245 && g > 245 && b > 245) || a == 0) {
            // Set non-white or transparent pixels to fully transparent
            outline.PixelData[i] = Vec4<int>(0, 0, 0, 0);
        }
    }

    outline.SaveImage("icons_cleaned");

    std::string newPath = Utils::getClientPath() + "//Cache//icons_cleaned.png";

    lastProcessTime2 = currentTime;
    cachedOutline2 = ResourceLocation(newPath, true);
    cacheInitialized2 = true;
    
    return cachedOutline2;
}

// recycled code from CustomCrosshair //

OutlineImage::OutlineImage(std::string Path)
{
    int w;
    int h;
    int comp;
    unsigned char* image = stbi_load(Path.c_str(), &w, &h, &comp, STBI_rgb_alpha); 

    if (w != h) {
        Size = 0;
        if (image) stbi_image_free(image);
        return;
    }

    Size = w;
    PixelData.clear();

    // Copy the image data to PixelData
    for (int i = 0; i < Size * Size * 4; i += 4)
    {
        PixelData.push_back(Vec4<int>(image[i], image[i + 1], image[i + 2], image[i + 3]));
    }

    if (image) stbi_image_free(image);
}

const unsigned char* OutlineImage::getImageData()
{
    // Ensure PixelData is properly sized
    if (PixelData.size() != Size * Size) {
        PixelData.resize(Size * Size, Vec4<int>(0, 0, 0, 0));
    }

    unsigned char* data = new unsigned char[Size * Size * 4];

    for (int i = 0; i < Size * Size; i++) {
        data[i * 4 + 0] = PixelData[i].x; // R
        data[i * 4 + 1] = PixelData[i].y; // G
        data[i * 4 + 2] = PixelData[i].z; // B
        data[i * 4 + 3] = PixelData[i].w; // A
    }

    return data;
}

void OutlineImage::SaveImage(std::string name)
{
    // Create cache directory if it doesn't exist
    std::string cachePath = Utils::getClientPath() + "//Cache//";
    std::filesystem::create_directories(cachePath);
    
    const unsigned char* data = getImageData();

    // Use stb_image_write to save the PNG
    std::string fullPath = cachePath + name + ".png";
    int result = stbi_write_png(fullPath.c_str(), Size, Size, 4, data, Size * 4);

    delete[] data;

    if (!result) {}
}