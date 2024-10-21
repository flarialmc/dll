#pragma once

#include "../Module.hpp"


class MotionBlur : public Module {
public:
    MotionBlur() : Module("Motion Blur",
                          "Make fast movements appear smoother and more realistic by\nblurring the image slightly in the direction of motion.",
                          IDR_BLUR_PNG, "") {
        Module::setup();
    };

    void onEnable() override {
        if(SwapchainHook::queue) FlarialGUI::Notify("Please turn on Better Frames in Settings!");
        else {
            Listen(this, RenderEvent, &MotionBlur::onRender)
            Module::onEnable();
        }
    }

    void onDisable() override {
        Deafen(this, RenderEvent, &MotionBlur::onRender)
        previousFrames.clear();
        Module::onDisable();
    }

    void defaultConfig() override {
        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.88f);
        if (settings.getSettingByName<float>("intensity2") == nullptr) settings.addSetting("intensity2", 6.0f);
    }

    void settingsRender() override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addSlider("Bleed Factor", "The scale at which previous frames bleed into the current one.", this->settings.getSettingByName<float>("intensity")->value, 1.0f, 0.f, true);
        this->addSlider("Intensity", "Amount of previous frames to render.", this->settings.getSettingByName<float>("intensity2")->value, 30, 0, true);

        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    static inline std::vector<winrt::com_ptr<ID3D11ShaderResourceView>> previousFrames;

    void onRender(RenderEvent &event) {

        if(ModuleManager::getModule("ClickGUI")->isEnabled()) return;

        int maxFrames = (int)round(this->settings.getSettingByName<float>("intensity2")->value);

        if (SDK::getCurrentScreen() == "hud_screen" && !SwapchainHook::queue && this->isEnabled()) {
            if (previousFrames.size() >= static_cast<int>(maxFrames)) {
                // Remove excess frames if maxFrames is reduced
                int framesToRemove = (int)previousFrames.size() - static_cast<int>(maxFrames);
                for (int i = 0; i < framesToRemove; ++i) {
                    if(previousFrames[i]) previousFrames[i]->Release();
                }
                previousFrames.erase(previousFrames.begin(), previousFrames.begin() + framesToRemove);
            }

            auto buffer = BackbufferToSRVExtraMode();
            if(buffer) previousFrames.push_back(BackbufferToSRVExtraMode());
            else std::cout << "Couldn't save buffer for Motion Blur." << std::endl;


            float alpha = 0.3f;

            for (const auto& frame: previousFrames) {

                if(!SwapchainHook::queue) {
                    ImageWithOpacity(frame, {MC::windowSize.x, MC::windowSize.y}, alpha);
                }
                alpha *= this->settings.getSettingByName<float>("intensity")->value;
            }


        } else {
            previousFrames.clear();
        }
    }

    void ImageWithOpacity(const winrt::com_ptr<ID3D11ShaderResourceView>& srv, ImVec2 size, float opacity)
    {
        if (opacity <= 0.0f) {
            //std::cout << "alpha: " + std::to_string(opacity) << std::endl;
            return;
        }

        opacity = opacity > 1.0f ? 1.0f : opacity < 0.0f ? 0.0f : opacity;
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        ImVec2 pos = {0, 0};
        ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(opacity * 255));
        draw_list->AddImage(srv.get(), pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), col);
        ImGui::SetCursorScreenPos(ImVec2(pos.x + size.x, pos.y));
    }

    static winrt::com_ptr<ID3D11ShaderResourceView> BackbufferToSRVExtraMode() {

        if(!FlarialGUI::needsBackBuffer) return nullptr;

        HRESULT hr;

        D3D11_TEXTURE2D_DESC d;
        SwapchainHook::ExtraSavedD3D11BackBuffer->GetDesc(&d);
        winrt::com_ptr<ID3D11ShaderResourceView> outSRV;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = d.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = d.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        if (FAILED(hr = SwapchainHook::d3d11Device->CreateShaderResourceView(SwapchainHook::ExtraSavedD3D11BackBuffer, &srvDesc, outSRV.put())))
        {
            std::cout << "Failed to create shader resource view: " << std::hex << hr << std::endl;
        }

        return outSRV;
    }

    static winrt::com_ptr<ID3D11ShaderResourceView> BackbufferToSRV() {

        HRESULT hr;

        D3D11_TEXTURE2D_DESC d;
        SwapchainHook::SavedD3D11BackBuffer->GetDesc(&d);
        winrt::com_ptr<ID3D11ShaderResourceView> outSRV;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = d.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = d.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        if (FAILED(hr = SwapchainHook::d3d11Device->CreateShaderResourceView(SwapchainHook::SavedD3D11BackBuffer, &srvDesc, outSRV.put())))
        {
            std::cout << "Failed to create shader resource view: " << std::hex << hr << std::endl;
        }

        return outSRV;
    }
};