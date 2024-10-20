#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

using namespace DirectX;

class MotionBlurListener : public Listener {

    Module *module;

public:
    explicit MotionBlurListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;

    }

    static inline std::vector<ID3D11ShaderResourceView*> previousFrames;


    void onRender(RenderEvent &event) override {

        if(ModuleManager::getModule("ClickGUI")->isEnabled()) return;

        int maxFrames = (int)round(module->settings.getSettingByName<float>("intensity2")->value);

        if (SDK::getCurrentScreen() == "hud_screen" && !SwapchainHook::queue && this->module->isEnabled()) {
            if (previousFrames.size() >= static_cast<int>(maxFrames)) {
                // Remove excess frames if maxFrames is reduced
                int framesToRemove = (int)previousFrames.size() - static_cast<int>(maxFrames);
                for (int i = 0; i < framesToRemove; ++i) {
                    if(previousFrames[i]) previousFrames[i]->Release();
                }
                previousFrames.erase(previousFrames.begin(), previousFrames.begin() + framesToRemove);
            }

            ID3D11ShaderResourceView* buffer = BackbufferToSRV();
            if(buffer) previousFrames.push_back(buffer);
            else std::cout << "Couldn't save buffer for Motion Blur.";


            float alpha = 0.3f;

                for (ID3D11ShaderResourceView* frame: previousFrames) {

                    if(!SwapchainHook::queue) {
                        ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                    }
                    alpha *= module->settings.getSettingByName<float>("intensity")->value;
                }


            } else {

                for (ID3D11ShaderResourceView* bitmap: previousFrames) Memory::SafeRelease(bitmap);
                previousFrames.clear();
            }
        }

    void ImageWithOpacity(ID3D11ShaderResourceView* srv, ImVec2 size, float opacity)
    {
        if (opacity <= 0.0f) {
            //std::cout << "alpha: " + std::to_string(opacity) << std::endl;
            return;
        }

        opacity = opacity > 1.0f ? 1.0f : opacity < 0.0f ? 0.0f : opacity;
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        ImVec2 pos = {0, 0};
        ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(opacity * 255));
        draw_list->AddImage(srv, pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), col);
        ImGui::SetCursorScreenPos(ImVec2(pos.x + size.x, pos.y));
    }

    static ID3D11ShaderResourceView* BackbufferToSRV() {

        HRESULT hr;

        D3D11_TEXTURE2D_DESC d;
        SwapchainHook::ExtraSavedD3D11BackBuffer->GetDesc(&d);
        ID3D11ShaderResourceView* outSRV;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = d.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = d.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        if (FAILED(hr = SwapchainHook::d3d11Device->CreateShaderResourceView(SwapchainHook::ExtraSavedD3D11BackBuffer, &srvDesc, &outSRV)))
        {
            std::cout << "Failed to create shader resource view: " << std::hex << hr << std::endl;
        }

        return outSRV;
    }
};