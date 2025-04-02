#include <dxgi.h>
#include "kiero/kiero.h"
#include "../Hook.hpp"

class ResizeHook : public Hook {
private:
    static void
    resizeCallback(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat,
                   UINT flags);

public:
    typedef void(__thiscall *resizeOriginal)(IDXGISwapChain *pSwapChain, UINT bufferCount, UINT width, UINT height,
                                             DXGI_FORMAT newFormat, UINT flags);

    static inline resizeOriginal funcOriginal = nullptr;

    ResizeHook();

    void enableHook() override;

    static void cleanShit(bool isResize = false);

    static void call();
};