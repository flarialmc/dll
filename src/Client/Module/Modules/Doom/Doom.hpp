#pragma once  

#include "../Module.hpp"  
#include <GUI/Engine/EngineImGui.hpp>
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "../../../../Assets/Assets.hpp"

#define DOOMGENERIC_RESX 640
#define DOOMGENERIC_RESY 400
#include "doomgeneric/doomgeneric.h"
#include "doomgeneric/doomkeys.h"



#define M_PI 3.14159265358979323846






inline void generate_picture(uint32_t *picture, int width, int height, int frame_number) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Normalize x and y to a 0.0 to 1.0 range
            float normalized_x = static_cast<float>(x) / (width - 1);
            float normalized_y = static_cast<float>(y) / (height - 1);

            // Introduce 'frame_number' for dynamic effects
            // Example 1: Shifting colors based on frame_number
            // float r_val = fmodf(normalized_x + (float)frame_number * 0.01, 1.0);
            // float g_val = fmodf(normalized_y + (float)frame_number * 0.005, 1.0);
            // float b_val = fmodf((normalized_x + normalized_y) / 2.0 + (float)frame_number * 0.007, 1.0);

            // Example 2: Pulsating colors using sine waves
            float r_val = fabsf(sinf(normalized_x * M_PI + (float)frame_number * 0.05));
            float g_val = fabsf(cosf(normalized_y * M_PI + (float)frame_number * 0.03));
            float b_val = fabsf(sinf((normalized_x + normalized_y) * M_PI / 2.0 + (float)frame_number * 0.04));


            uint8_t r = (uint8_t)(r_val * 255);
            uint8_t g = (uint8_t)(g_val * 255);
            uint8_t b = (uint8_t)(b_val * 255);

            // XRGB8888 format: [X][R][G][B] (X is typically 0xFF for opaque)
            picture[y * width + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
        }
    }
}


inline bool isDoomReady = false;
inline std::string doompath;
inline bool textureReady = false;

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

inline static unsigned char convertToDoomKey(unsigned char key)
{
    switch (key)
    {
    case VK_RETURN:
        key = KEY_ENTER;
        break;
    case VK_ESCAPE:
        key = KEY_ESCAPE;
        break;
    case VK_LEFT:
        key = KEY_LEFTARROW;
        break;
    case VK_RIGHT:
        key = KEY_RIGHTARROW;
        break;
    case VK_UP:
        key = KEY_UPARROW;
        break;
    case VK_DOWN:
        key = KEY_DOWNARROW;
        break;
    case VK_CONTROL:
        key = KEY_FIRE;
        break;
    case VK_SPACE:
        key = KEY_USE;
        break;
    case VK_SHIFT:
        key = KEY_RSHIFT;
        break;
    default:
        key = tolower(key);
        break;
    }

    return key;
}


inline static void addKeyToQueue(int pressed, unsigned char keyCode)
{
    unsigned char key = convertToDoomKey(keyCode);

    unsigned short keyData = (pressed << 8) | key;

    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

class DoomModule : public Module {
private:
    ID3D11Texture2D* doomTexture = nullptr;
    ID3D11ShaderResourceView* doomSRV = nullptr;
    bool textureInitialized = false;

public:
    // uint32_t displaybuffer[320*200]; // 320x200 XRGB8888 buffer from doomgeneric
    int frame = 0;

    // char* doomwad;

    DoomModule() : Module("Doom", "Display Doom game from doomgeneric", IDR_SKULL_PNG, "") {
    };

    void onEnable() override {
        if (SwapchainHook::isDX12) {
            FlarialGUI::Notify("Please enable Better Frames on the settings."); 
            this->setEnabled(false);
            return;
        }

        std::string wadpath = Utils::getRoamingPath() + "\\Flarial\\doom.wad";
        if (!std::filesystem::exists(wadpath)) {
            FlarialGUI::Notify("DOOM.wad not found in Flarial folder! Please add the file to play Doom.");
            this->setEnabled(false);
            return;
        }
        char dummy[] = "flarial.dll";
        char iwad[] = "-iwad";
        doompath = wadpath;


        char* fakeargv[] = {dummy, iwad, const_cast<char*>(doompath.c_str()), NULL};

        doomgeneric_Create(3, fakeargv);

        Listen(this, RenderEvent, &DoomModule::onRender)
			Listen(this, KeyEvent, &DoomModule::onKey);
            Module::onEnable();
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &DoomModule::onRender)
			Deafen(this, KeyEvent, &DoomModule::onKey);
            cleanupTexture();
        Module::onDisable();
    }

    void defaultConfig() override {
        Module::defaultConfig();
        if (settings.getSettingByName<float>("scale") == nullptr)
            settings.addSetting("scale", 2.0f);
    }

    void normalRender(int index, std::string& value) override {
        Module::normalRender(index, value);
    }

    void onRender(RenderEvent& event);
    void initializeTexture();
    void updateTexture(uint32_t* buffer);
    void cleanupTexture();
    void onKey(KeyEvent& event);

};

