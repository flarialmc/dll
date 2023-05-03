#include "SwapChainHook.h"
#include "../../../ImGui/Fonts/Minecraft.h"

static std::chrono::high_resolution_clock fpsclock;
static std::chrono::steady_clock::time_point start = fpsclock.now();

typedef long(__stdcall* PresentHook)(IDXGISwapChain*, UINT, UINT);
PresentHook oPresent;

typedef long(__stdcall* ResizeBuffersHook)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
ResizeBuffersHook oResizeBuffers;

uintmax_t* methodsTable = NULL;

namespace DX12Interface {
	ID3D12Device* d3d12device = nullptr;
}

namespace DX11Interface {
	ID3D11Device* d3d11device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* mainRenderTargetView = nullptr;
}

void setImGuiStyle() {
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowRounding = 5.f;
	style->FrameRounding = 3.f;
	style->GrabRounding = 3.f;
	style->ScrollbarSize = 10.f;

	style->Colors[ImGuiCol_WindowBg] = ImColor(34, 34, 34, 140);
	style->Colors[ImGuiCol_Button] = ImColor(212, 41, 50);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(140, 32, 38);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(140, 32, 38);
	style->Colors[ImGuiCol_FrameBg] = ImColor(48, 48, 48);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(48, 48, 48);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(48, 48, 48);
	style->Colors[ImGuiCol_CheckMark] = ImColor(212, 41, 50);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(212, 41, 50);
	style->Colors[ImGuiCol_ScrollbarBg] = ImColor(0, 0, 0, 0);

	style->Colors[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 70);

	// ImGuiIO& io = ImGui::GetIO();

	// io.Fonts->AddFontFromMemoryCompressedTTF(Minecraft_compressed_data, Minecraft_compressed_size, 18.f);
}

bool init = false;
static long __stdcall PresentDetour(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
	MC::frames += 1;

	if ((std::chrono::duration<float>(fpsclock.now() - start).count()) > 1.f) {
		MC::fps = MC::frames;
		MC::frames = 0;

		start = fpsclock.now();
	}

	if (init) {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();

		Manager::onRender();

        

		/*D2D1_RECT_F rect = D2D1::RectF(50.0f, 50.0f, 200.0f, 150.0f);

		ID2D1SolidColorBrush* brush = NULL;
		RenderUtils::D2DC->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);
		RenderUtils::D2DC->DrawRectangle(rect, brush);
		brush->Release();
		 */
		

		/*RenderUtils::D2DC->BeginDraw();
		D2D_RECT_F rect = D2D1::RectF(50.0f, 50.0f, 200.0f, 150.0f);
		ID2D1SolidColorBrush* brush;
		RenderUtils::D2DC->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush);
		RenderUtils::D2DC->FillRectangle(rect, brush);
		brush->Release();
		RenderUtils::D2DC->EndDraw();*/

		RenderUtils::D2DC->BeginDraw();
		FlarialGUI::RoundedButton(50.0f, 100.0f, D2D1::ColorF(D2D1::ColorF::Red), D2D1::ColorF(D2D1::ColorF::White), L"AmongUs");
		RenderUtils::D2DC->EndDraw();

		
        

		ImGui::EndFrame();
		ImGui::Render();

		DX11Interface::context->OMSetRenderTargets(1, &DX11Interface::mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	else {
		if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&DX12Interface::d3d12device)))) {
			Logger::debug("Present function called 4");

			static_cast<ID3D12Device5*>(DX12Interface::d3d12device)->RemoveDevice();
		}

		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&DX11Interface::d3d11device))) {
			DX11Interface::d3d11device->GetImmediateContext(&DX11Interface::context);

			HWND window = FindWindowA(nullptr, "Minecraft");

			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
			DX11Interface::d3d11device->CreateRenderTargetView(pBackBuffer, NULL, &DX11Interface::mainRenderTargetView);

            const D2D1_CREATION_PROPERTIES properties
                    {
                            D2D1_THREADING_MODE_SINGLE_THREADED,
                            D2D1_DEBUG_LEVEL_NONE,
                            D2D1_DEVICE_CONTEXT_OPTIONS_NONE
                    };

            ID2D1DeviceContext* d2dContext = NULL;
            IDXGISurface* eBackBuffer;
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&eBackBuffer));
            D2D1CreateDeviceContext(eBackBuffer, properties, &d2dContext);
            RenderUtils::D2DC = d2dContext;

			if (ImGui::GetCurrentContext() == nullptr)
				ImGui::CreateContext();

			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(DX11Interface::d3d11device, DX11Interface::context);

			setImGuiStyle();

			init = true;
		}
	}

	return oPresent(pSwapChain, syncInterval, flags);
}

static long __stdcall ResizeBuffersDetour(IDXGISwapChain* pSwapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT flags) {
	if (init) {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		Mem::SafeRelease<ID3D11RenderTargetView>(&DX11Interface::mainRenderTargetView);
		Mem::SafeRelease<ID3D11Device>(&DX11Interface::d3d11device);
		Mem::SafeRelease<ID3D11DeviceContext>(&DX11Interface::context);

		init = false;
	}

	MC::windowSize = Vec2<int16_t>(width, height);

	return oResizeBuffers(pSwapChain, bufferCount, width, height, newFormat, flags);
}

bool hookD3D12() {
	WNDCLASSEXW windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"SwapChain Window";
	windowClass.hIconSm = NULL;

	RegisterClassExW(&windowClass);

	HWND window = CreateWindowW(windowClass.lpszClassName, L"DirectX Window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);

	HMODULE libDXGI;
	HMODULE libD3D12;

	if ((libDXGI = GetModuleHandleW(L"dxgi.dll")) == NULL || (libD3D12 = GetModuleHandleW(L"d3d12.dll")) == NULL) {
		return false;
	}

	void* CreateDXGIFactory;
	CreateDXGIFactory = GetProcAddress(libDXGI, "CreateDXGIFactory");

	IDXGIFactory* factory;
	((long(__stdcall*)(const IID&, void**))(CreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&factory);

	IDXGIAdapter* adapter;
	factory->EnumAdapters(0, &adapter);

	void* D3D12CreateDevice;
	D3D12CreateDevice = GetProcAddress(libD3D12, "D3D12CreateDevice");

	ID3D12Device* device;
	((long(__stdcall*)(IUnknown*, D3D_FEATURE_LEVEL, const IID&, void**))(D3D12CreateDevice))(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device);

	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = 0;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	ID3D12CommandQueue* commandQueue;
	device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**)&commandQueue);

	ID3D12CommandAllocator* commandAllocator;
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&commandAllocator);

	ID3D12GraphicsCommandList* commandList;
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&commandList);

	DXGI_RATIONAL refreshRate;
	refreshRate.Numerator = 60;
	refreshRate.Denominator = 1;

	DXGI_MODE_DESC bufferDesc;
	bufferDesc.Width = 100;
	bufferDesc.Height = 100;
	bufferDesc.RefreshRate = refreshRate;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGISwapChain* swapChain;
	factory->CreateSwapChain(commandQueue, &swapChainDesc, &swapChain);

	methodsTable = (uintmax_t*)::calloc(150, sizeof(uintmax_t));
	memcpy(methodsTable, *(uintmax_t**)device, 44 * sizeof(uintmax_t));
	memcpy(methodsTable + 44, *(uintmax_t**)commandQueue, 19 * sizeof(uintmax_t));
	memcpy(methodsTable + 44 + 19, *(uintmax_t**)commandAllocator, 9 * sizeof(uintmax_t));
	memcpy(methodsTable + 44 + 19 + 9, *(uintmax_t**)commandList, 60 * sizeof(uintmax_t));
	memcpy(methodsTable + 44 + 19 + 9 + 60, *(uintmax_t**)swapChain, 18 * sizeof(uintmax_t));

	device->Release();
	commandQueue->Release();
	commandAllocator->Release();
	commandList->Release();
	swapChain->Release();

	return true;
}

SwapChainHook::SwapChainHook() : Hook() {
	if (hookD3D12()) {
		addr = methodsTable[140];

		Mem::hookFunc((void*)addr, (void*)PresentDetour, (void**)&oPresent, "Present");
		Mem::hookFunc((void*)methodsTable[145], (void*)ResizeBuffersDetour, (void**)&oResizeBuffers, "ResizeBuffers");
	}
}