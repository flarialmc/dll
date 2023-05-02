#pragma once
#include <string>
#include <dxgi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d2d1_1.h>
#include <dxgi1_4.h>
#include <dwrite_1.h>
#include <initguid.h>

#include "../ImGui/Notifications/imgui_notify.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_dx12.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/Fonts/ProductSans.h"
#include "../ImGui/Fonts/tahoma.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dwrite.lib")

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

enum SwapChainDeviceType {
	INVALID = 0, Dx11 = 1, Dx12 = 2
};

class RenderUtils {
public:
    static ID2D1DeviceContext* D2DC;
public:
    static SwapChainDeviceType type;
public:
    bool contains(ImVec4*);
public:
    static void drawText(ImDrawList*, ImVec2, std::string, float, ImColor);
    static ImVec2 getTextSize(std::string, float);
public:
    static void fillRect(ImDrawList*, ImVec4, ImColor, float);
    static void drawRect(ImDrawList*, ImVec4, ImColor, float, float);
public:
    static bool drawTabButton(ImDrawList*, std::string);
};