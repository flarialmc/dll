#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class MotionBlurListener : public Listener {

    Module* module;

public:
    explicit MotionBlurListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;

    }


    static inline std::vector<ID2D1Bitmap*> previousFrames;

    void onRender(RenderEvent& event) override {

        int maxFrames = module->settings.getSettingByName<float>("intensity2")->value;

        if(module->settings.getSettingByName<bool>("enabled")->value && SDK::CurrentScreen == "hud_screen") {

            if (previousFrames.size() >= static_cast<size_t>(maxFrames)) {
                // Remove excess frames if maxFrames is reduced
                size_t framesToRemove = previousFrames.size() - static_cast<size_t>(maxFrames);
                for (size_t i = 0; i < framesToRemove; ++i) {
                    previousFrames[i]->Release();
                }
                previousFrames.erase(previousFrames.begin(), previousFrames.begin() + framesToRemove);
            }

            ID2D1Bitmap* currentFrame = nullptr;

            if (SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &currentFrame);
            else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &currentFrame);

            previousFrames.push_back(currentFrame);

            float alpha = 0.3f;
            for (ID2D1Bitmap* frame : previousFrames)
            {
                D2D::context->DrawBitmap(frame, D2D1::RectF(0.f, 0.f, MC::windowSize.x, MC::windowSize.y), alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                alpha *= module->settings.getSettingByName<float>("intensity")->value;
            }
        } else {

            for(ID2D1Bitmap* bitmap : MotionBlurListener::previousFrames) {
                Memory::SafeRelease(bitmap);
            }

            MotionBlurListener::previousFrames.clear();

        }

        D2D::context->Flush();

    }
};
