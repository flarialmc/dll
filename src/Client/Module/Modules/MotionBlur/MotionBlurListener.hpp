#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class MotionBlurListener : public Listener {

    Module *module;

public:
    explicit MotionBlurListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

    }

    static inline std::vector<ID3D11ShaderResourceView*> previousFrames;


    void onRender(RenderEvent &event) override {
        int maxFrames = (int)round(module->settings.getSettingByName<float>("intensity2")->value);

        if (SDK::currentScreen == "hud_screen") {
            if (previousFrames.size() >= static_cast<int>(maxFrames)) {
                // Remove excess frames if maxFrames is reduced
                int framesToRemove = (int)previousFrames.size() - static_cast<int>(maxFrames);
                for (int i = 0; i < framesToRemove; ++i) {
                    if(previousFrames[i]) previousFrames[i]->Release();
                }
                previousFrames.erase(previousFrames.begin(), previousFrames.begin() + framesToRemove);
            }

            previousFrames.push_back(SaveBackbuffer());


            float alpha = 0.3f;

            if(!SwapchainHook::queue) {
                for (ID3D11ShaderResourceView* frame: previousFrames) {

                    //if(!frame) std::cout << "TROLLED" << std::endl;
                    RenderTextureWithOpacity(frame, alpha);
                    alpha *= module->settings.getSettingByName<float>("intensity")->value;
                }
            } else {
                // do later
            }

            } else {

                for (ID3D11ShaderResourceView* bitmap: previousFrames) Memory::SafeRelease(bitmap);
                previousFrames.clear();
            }
        }


    ID3D11ShaderResourceView* SaveBackbuffer()
    {

        ID3D11DeviceContext* deviceContext;
        SwapchainHook::d3d11Device->GetImmediateContext(&deviceContext);
        IDXGISurface1* backBuffer = nullptr;
        HRESULT hr;
        SwapchainHook::swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

        ID3D11Texture2D* buffer2D = nullptr;
        if(FAILED(backBuffer->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer2D)))) std::cout << "failed to get 2d" << std::endl;

        D3D11_TEXTURE2D_DESC desc;
        buffer2D->GetDesc(&desc);

        ID3D11Texture2D* pTexture = nullptr;
        HRESULT r = SwapchainHook::d3d11Device->CreateTexture2D(&desc, nullptr, &pTexture);

        deviceContext->CopyResource(pTexture, buffer2D);

        ID3D11ShaderResourceView* outSRV;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        if (FAILED(SwapchainHook::d3d11Device->CreateShaderResourceView(pTexture, &srvDesc, &outSRV)))
        {
            std::cout << "Failed to create shader resource view: " << std::endl;
        }

        backBuffer->Release();
        buffer2D->Release();
        pTexture->Release();

        return outSRV;
    }

    void RenderTextureWithOpacity(ID3D11ShaderResourceView* textureSRV, float opacity)
    {

        ID3D11DeviceContext* deviceContext;
        SwapchainHook::d3d11Device->GetImmediateContext(&deviceContext);

        if (!deviceContext || !textureSRV)
            return;

        struct PixelShaderConstants
        {
            float opacity;
            float padding[3];
        } psConstants;

        psConstants.opacity = opacity;

        deviceContext->PSSetShaderResources(0, 1, &textureSRV);
        deviceContext->UpdateSubresource(
            nullptr,
            0,
            nullptr,
            &psConstants,
            0,
            0
        );
        deviceContext->PSSetConstantBuffers(0, 1, nullptr);
        deviceContext->Draw(6, 0);
    }
};