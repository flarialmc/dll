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


    static inline std::vector<winrt::com_ptr<ID2D1Bitmap>> previousFrames;

    void onRender(RenderEvent &event) override {

        int maxFrames = (int)round(module->settings.getSettingByName<float>("intensity2")->value);

        // TODO: Clear frames if not in HUD !!!

        if (SDK::currentScreen == "hud_screen") {

            if (previousFrames.size() >= static_cast<int>(maxFrames)) {
                // Remove excess frames if maxFrames is reduced
                int framesToRemove = (int)previousFrames.size() - static_cast<int>(maxFrames);
                previousFrames.erase(previousFrames.begin(), previousFrames.begin() + framesToRemove);
            }

            winrt::com_ptr<ID2D1Bitmap> currentFrame = nullptr;

            if (SwapchainHook::queue != nullptr)
                FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap].get(), currentFrame.put());
            else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap.get(), currentFrame.put());

            previousFrames.push_back(currentFrame);

            float alpha = 0.3f;
            for (const auto& frame: previousFrames) {
                D2D::context->DrawBitmap(frame.get(), D2D1::RectF(0.f, 0.f, MC::windowSize.x, MC::windowSize.y), alpha,
                                         D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                alpha *= module->settings.getSettingByName<float>("intensity")->value;
            }
        } else {
            MotionBlurListener::previousFrames.clear();
        }

        D2D::context->Flush();
    }
};
