#include "D2D.hpp"

ID2D1DeviceContext* D2D::context = NULL;
ID3D11Device* D2D::device;
ID3D11RenderTargetView* D2D::renderTarget;


    void D2D::terminate() {
        context->Release();
        device->Release();
        renderTarget->Release();
    }
