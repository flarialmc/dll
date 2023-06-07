#include "D2D.hpp"
#include "../../Utils/Logger/Logger.hpp"

ID2D1DeviceContext* D2D::context;


    void D2D::terminate() {
        if(D2D::context != nullptr) {
            D2D::context->Release();
            Logger::debug("isreal");
        }

        D2D::context = nullptr;
    }
