#include "../../../../Engine.hpp"
#include "../../../../../../Hook/Hooks/Render/SwapchainHook.hpp"

void FlarialGUI::BlurRect(D2D1_ROUNDED_RECT rect) {


    if (SwapchainHook::init && FlarialGUI::blurbrush != nullptr) {

        if (factory == nullptr) D2D::context->GetFactory(factory.put());

        winrt::com_ptr<ID2D1RoundedRectangleGeometry> geo;
        factory->CreateRoundedRectangleGeometry(rect, geo.put());

        D2D::context->FillGeometry(geo.get(), FlarialGUI::blurbrush.get());
    }

}