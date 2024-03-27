#include "../../../../Engine.hpp"
#include "../../../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::BlurRect(D2D1_ROUNDED_RECT rect, float intensity) {


    if (SwapchainHook::init && FlarialGUI::blurbrush != nullptr) {

        if (factory == nullptr) D2D::context->GetFactory(&factory);

        ID2D1RoundedRectangleGeometry *geo;
        factory->CreateRoundedRectangleGeometry(rect, &geo);

        D2D::context->FillGeometry(geo, FlarialGUI::blurbrush);

        Memory::SafeRelease(factory);
        //Memory::SafeRelease(geo); factory does not need to be destroyed
    }

}