#include "D2D.hpp"

namespace D2D {

    void terminate() {
        context->Release();
        device->Release();
        renderTarget->Release();
    }

}