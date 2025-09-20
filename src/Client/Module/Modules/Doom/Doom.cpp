#include "Doom.hpp"
#include "../../../GUI/Engine/Engine.hpp"  


extern "C" {
    #include "doomgeneric/doomgeneric.h"
}

extern "C" void DG_Init() {
    isDoomReady = true;
    Logger::info("Doom is initializing");
}

extern "C" void DG_DrawFrame(pixel_t* buffer) {
    textureReady = true;
    std::shared_ptr<DoomModule> doomModule = std::dynamic_pointer_cast<DoomModule>(ModuleManager::getModule("Doom"));

    // printf("DG_DrawFrame called\n");
    // printf("DG_ScreenBuffer address: %p\n", buffer);

    if (buffer) {
        // Check first few pixels  
        int nonZeroCount = 0;
        for (int i = 0; i < 100; i++) {
            if (buffer[i] != 0) {
                nonZeroCount++;
            }
        }
        // printf("Non-zero pixels in first 100: %d\n", nonZeroCount);
    } else {
        // printf("DG_ScreenBuffer is NULL\n");
	}

    if(doomModule)
	    doomModule->updateTexture(buffer);
}

extern "C" void DG_SleepMs(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

extern "C" uint32_t DG_GetTicksMs() {
    //return static_cast<uint32_t>(Utils::getCurrentMs() - Client::start);
	return GetTickCount64(); // Use Windows API for cross-platform compatibility
}

extern "C" void DG_SetWindowTitle(const char * title) {
}
    


/* Implement doom funcs above*/



void DoomModule::onRender(RenderEvent& event) {
    // DOOM_GENERIC

    if (!this->isEnabled()) return;


    if (!isDoomReady) return;
    // Logger::debug("Doom is ready");

    // Initialize texture on first render
    if (!textureInitialized) {
        Logger::debug("Initializing texture");
        initializeTexture();
    }

    // Update texture with doomgeneric buffer data
    frame++;
    // fixedRateCaller.update();
#ifdef COMPILE_DOOM
    doomgeneric_Tick();
#endif //fix compiler complaining about this shit missing
    Logger::debug("Doom ticked");
    if (!textureReady) return;
    if (textureReady)
        textureReady = false;



    // Render the doom display  
    if (doomSRV) {
        float scale = settings.getSettingByName<float>("scale")->value;
        float width = DOOMGENERIC_RESX * scale;
        float height = DOOMGENERIC_RESY * scale;

        // Center the display  
        float x = (MC::windowSize.x - width) / 2.0f;
        float y = (MC::windowSize.y - height) / 2.0f;

        // Use ImGui to render the texture  
        ImGui::GetBackgroundDrawList()->AddImage(
            ImTextureID(doomSRV),
            ImVec2(x, y),
            ImVec2(x + width, y + height),
            ImVec2(0, 0),
            ImVec2(1, 1),
            IM_COL32_WHITE
        );
    }
}

void DoomModule::initializeTexture() {
    if (!SwapchainHook::d3d11Device || textureInitialized) return;
    Logger::debug("Initializing texture in DoomModule::initializeTexture");

    // Create dynamic texture for doomgeneric buffer  
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = DOOMGENERIC_RESX;
    desc.Height = DOOMGENERIC_RESY;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8X8_UNORM; // Direct XRGB support
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = SwapchainHook::d3d11Device->CreateTexture2D(&desc, nullptr, &doomTexture);
    if (FAILED(hr)) {
        Logger::error("Failed to create Doom texture");
        return;
    }

    // Create shader resource view  
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    hr = SwapchainHook::d3d11Device->CreateShaderResourceView(doomTexture, &srvDesc, &doomSRV);
    if (FAILED(hr)) {
        Logger::error("Failed to create Doom shader resource view");
        cleanupTexture();
        return;
    }

    textureInitialized = true;
}

void DoomModule::updateTexture(pixel_t *frame) {
    // Logger::debug("SwapchainHook::context: {}", SwapchainHook::context);
     if (!doomTexture 
         || !SwapchainHook::context 
         || !frame) return;

    int nonZeroPixels = 0;
    for (int i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY && i < 1000; i++) {
        if (frame[i] != 0) {
            nonZeroPixels++;
        }
    }
    // printf("Non-zero pixels in first 1000: %d\n", nonZeroPixels);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = SwapchainHook::context->Map(doomTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (SUCCEEDED(hr)) {
        // Copy doomgeneric's uint32_t buffer row by row  
        for (int y = 0; y < DOOMGENERIC_RESY; y++) {
            memcpy(
                (uint8_t*)mappedResource.pData + y * mappedResource.RowPitch,
                frame + y * DOOMGENERIC_RESX,
                DOOMGENERIC_RESX * 4
            );
        }
        SwapchainHook::context->Unmap(doomTexture, 0);
    }
}

void DoomModule::cleanupTexture() {
    if (doomSRV) {
        doomSRV->Release();
        doomSRV = nullptr;
    }
    if (doomTexture) {
        doomTexture->Release();
        doomTexture = nullptr;
    }
    textureInitialized = false;
}

/* KEY EVENTS */




extern "C" int DG_GetKey(int* pressed, unsigned char* key) {
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
    {

        return 0;
    }
    else
    {
        unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
        s_KeyQueueReadIndex++;
        s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

        *pressed = keyData >> 8;
        *key = keyData & 0xFF;

        return 1;
    }
}

void DoomModule::onKey(KeyEvent& event)
{
    addKeyToQueue(static_cast<int>(event.getAction()), static_cast<unsigned char>(event.getKey()));
}
