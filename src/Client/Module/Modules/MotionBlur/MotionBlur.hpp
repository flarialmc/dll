#pragma once

#include "AvgPixelMotionBlurHelper.hpp"
#include "RealMotionBlurHelper.hpp"
#include "../Module.hpp"


class MotionBlur : public Module {
public:
    static inline bool initted = false;

    MotionBlur() : Module("Motion Blur",
                          "Make fast movements appear smoother and more realistic by\nblurring the image slightly in the direction of motion.",
                          IDR_BLUR_PNG, "") {
        Module::setup();

    };

    bool once = false;

    void onEnable() override {
        if (SwapchainHook::queue) { if (!once) { FlarialGUI::Notify("Please turn on Better Frames in Settings!"); once = true; } }
        else {
            ListenOrdered(this, RenderUnderUIEvent, &MotionBlur::onRender, EventOrder::IMMEDIATE)
            Module::onEnable();
        }
    }

    void onDisable() override {
        Deafen(this, RenderUnderUIEvent, &MotionBlur::onRender)
        previousFrames.clear();
        Module::onDisable();
    }

    void defaultConfig() override { Module::defaultConfig();
        if (settings.getSettingByName<float>("intensity") == nullptr) settings.addSetting("intensity", 0.88f);
        if (settings.getSettingByName<float>("intensity2") == nullptr) settings.addSetting("intensity2", 6.0f);
        if (settings.getSettingByName<bool>("avgpixel") == nullptr) settings.addSetting("avgpixel", false);
        if (settings.getSettingByName<bool>("dynamic") == nullptr) settings.addSetting("dynamic", true);
        if (settings.getSettingByName<float>("samples") == nullptr) settings.addSetting("samples", 64.f);


    }

    void settingsRender(float settingsOffset) override {


        float x = Constraints::PercentageConstraint(0.019, "left");
        float y = Constraints::PercentageConstraint(0.10, "top");

        const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


        FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
        FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
                                  Constraints::RelativeConstraint(1.0, "width"),
                                  Constraints::RelativeConstraint(0.88f, "height"));

        this->addHeader("Misc");
        this->addToggle("Average Pixel Mode", "Disabling this will likely look better on high FPS.", this->settings.getSettingByName<bool>("avgpixel")->value);
        if (this->settings.getSettingByName<bool>("avgpixel")->value) {

            this->addToggle("Dynamic Mode", "Automatically adjusts intensity according to FPS", this->settings.getSettingByName<bool>("dynamic")->value);
            this->addConditionalSlider(!this->settings.getSettingByName<bool>("dynamic")->value, "Intensity", "Amount of previous frames to render.", this->settings.getSettingByName<float>("intensity2")->value, 30, 0, true);

        } else {
          this->addSlider("Intensity", "Control how strong the motion blur is.", this->settings.getSettingByName<float>("intensity")->value, 2, 0.05f, true);
          this->addSlider("Samples", "", this->settings.getSettingByName<float>("samples")->value, 256, 8, true);
        }
        FlarialGUI::UnsetScrollView();

        this->resetPadding();
    }

    static inline std::vector<winrt::com_ptr<ID3D11ShaderResourceView>> previousFrames;

    void onRender(RenderUnderUIEvent& event) {

        if (SwapchainHook::queue) return;

        int maxFrames = (int)round(this->settings.getSettingByName<float>("intensity2")->value);

        if (this->settings.getSettingByName<bool>("dynamic")->value) {
            if (MC::fps < 75) maxFrames = 1;
            else if (MC::fps < 100) maxFrames = 2;
            else if (MC::fps < 180) maxFrames = 3;
            else if (MC::fps > 300) maxFrames = 4;
            else if (MC::fps > 450) maxFrames = 5;
        }

        if (this->settings.getSettingByName<bool>("avgpixel")->value) maxFrames = 1;

        if (SDK::getCurrentScreen() == "hud_screen" && initted && this->isEnabled()) {

            // Remove excess frames if maxFrames is reduced
            if (previousFrames.size() > static_cast<size_t>(maxFrames)) {
                previousFrames.erase(previousFrames.begin(),
                                     previousFrames.begin() + (previousFrames.size() - maxFrames));
            }

            auto buffer = BackbufferToSRVExtraMode();
            if (buffer) {
                previousFrames.push_back(std::move(buffer));
            }

            if (this->settings.getSettingByName<bool>("avgpixel")->value) AvgPixelMotionBlurHelper::Render(event.RTV, previousFrames);
            else RealMotionBlurHelper::Render(event.RTV, previousFrames.back());

        } else {
            previousFrames.clear();
        }
    }


    void ImageWithOpacity(const winrt::com_ptr<ID3D11ShaderResourceView>& srv, ImVec2 size, float opacity)
    {
        if (opacity <= 0.0f) {
            //std::cout << "alpha: " + FlarialGUI::cached_to_string(opacity) << std::endl;
            return;
        }

        opacity = opacity > 1.0f ? 1.0f : opacity < 0.0f ? 0.0f : opacity;
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        ImVec2 pos = {0, 0};
        ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(opacity * 255));
        draw_list->AddImage(ImTextureID(srv.get()), pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), col);
        ImGui::SetCursorScreenPos(ImVec2(pos.x + size.x, pos.y));
    }

    static winrt::com_ptr<ID3D11ShaderResourceView> BackbufferToSRVExtraMode() {

        if (!FlarialGUI::needsBackBuffer) return nullptr;
        if (SwapchainHook::queue) return BackbufferToSRV();
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