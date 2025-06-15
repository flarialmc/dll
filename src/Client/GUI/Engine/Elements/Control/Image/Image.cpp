#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../Structs/ImagesClass.hpp"
#include "../../../../../../Assets/Assets.hpp"
#include "../../../../../Client.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include <imgui/stb.h>
#include <winrt/base.h>
#include <memory>


void FlarialGUI::image(const std::string& imageName, D2D1_RECT_F rect) {

    if (isInScrollView) {
        rect.top += scrollpos;
        rect.bottom += scrollpos;
    }
    if (shouldAdditionalY) {
        rect.top += additionalY[additionalIndex];
        rect.bottom += additionalY[additionalIndex];
    }

    std::string among = Utils::getRoamingPath() + "\\" + imageName;

    if (ImagesClass::eimages[imageName] == nullptr)
        LoadImageFromFile(FlarialGUI::to_wide(among).c_str(), &ImagesClass::eimages[imageName]);

	if (ImagesClass::eimages[imageName] == nullptr) return;


    // Draw image
    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;

    if (imageName == R"(\Flarial\Assets\transparent.png)")
        interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
}

struct ImageDataDX11 {
    int width;
    int height;
    std::vector<unsigned char> data;
};

std::optional<ImageDataDX11> LoadImageDataDX11FromResource(int resourceId, LPCTSTR type) {
    HRSRC imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
    if (!imageResHandle) return std::nullopt;

    HGLOBAL imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
    if (!imageResDataHandle) return std::nullopt;

    const unsigned char* pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
    if (!pImageFile) return std::nullopt;

    DWORD imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
    if (imageFileSize == 0) return std::nullopt;

    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory(pImageFile, imageFileSize, &width, &height, &channels, 4);
    if (!image_data) return std::nullopt;

    ImageDataDX11 result;
    result.width = width;
    result.height = height;
    result.data.assign(image_data, image_data + (width * height * 4));
    stbi_image_free(image_data);
    return result;
}

bool FlarialGUI::LoadImageFromResource(int resourceId, ID3D11ShaderResourceView** out_srv, LPCTSTR type) {
    // Load image data on a background thread
    auto future = std::async(std::launch::async, LoadImageDataDX11FromResource, resourceId, type);
    auto image_data_opt = future.get();
    if (!image_data_opt) return false;
    const auto& image_data = *image_data_opt;

    // Create texture on main thread
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_data.width;
    desc.Height = image_data.height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    winrt::com_ptr<ID3D11Texture2D> pTexture;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data.data.data();
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    HRESULT r = SwapchainHook::d3d11Device->CreateTexture2D(&desc, &subResource, pTexture.put());
    if (FAILED(r)) return false;

    // Create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    r = SwapchainHook::d3d11Device->CreateShaderResourceView(pTexture.get(), &srvDesc, out_srv);
    if (FAILED(r)) return false;

    return true;
}

#include <future>

struct ImageData {
    int width;
    int height;
    std::vector<unsigned char> data;
    winrt::com_ptr<ID3D12Resource> uploadBuffer;
    UINT uploadPitch;
    UINT uploadSize;
};

std::optional<ImageData> LoadImageDataFromResource(int resourceId, LPCTSTR type, ID3D12Device* device) {
    Logger::custom(fg(fmt::color::cyan), "ImageData", "Loading resource {} from memory...", resourceId);
    
    HRSRC imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
    if (!imageResHandle) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: FindResource failed for resource {}. GetLastError: {}", resourceId, GetLastError());
        return std::nullopt;
    }

    HGLOBAL imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
    if (!imageResDataHandle) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: LoadResource failed for resource {}. GetLastError: {}", resourceId, GetLastError());
        return std::nullopt;
    }

    const unsigned char* pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
    if (!pImageFile) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: LockResource failed for resource {}", resourceId);
        return std::nullopt;
    }

    DWORD imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
    if (imageFileSize == 0) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: SizeofResource returned 0 for resource {}", resourceId);
        return std::nullopt;
    }
    
    Logger::custom(fg(fmt::color::green), "ImageData", "Resource {} loaded from memory. Size: {} bytes", resourceId, imageFileSize);

    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory(pImageFile, imageFileSize, &width, &height, &channels, 4);
    if (!image_data) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: stbi_load_from_memory failed for resource {}. STBI error: {}", resourceId, stbi_failure_reason());
        return std::nullopt;
    }
    
    Logger::custom(fg(fmt::color::green), "ImageData", "Image decoded successfully. Size: {}x{}, Channels: {}", width, height, channels);

    // Prepare upload buffer
    UINT uploadPitch = (width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    UINT uploadSize = height * uploadPitch;
    
    Logger::custom(fg(fmt::color::cyan), "ImageData", "Upload buffer params: Pitch={}, Size={}, Alignment={}", 
                   uploadPitch, uploadSize, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = uploadSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES props = {};
    props.Type = D3D12_HEAP_TYPE_UPLOAD;

    Logger::custom(fg(fmt::color::yellow), "ImageData", "Creating upload buffer...");
    winrt::com_ptr<ID3D12Resource> uploadBuffer;
    HRESULT hr = device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(uploadBuffer.put()));
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: Failed to create upload buffer. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        stbi_image_free(image_data);
        return std::nullopt;
    }
    if (!uploadBuffer) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: Upload buffer is null after creation");
        stbi_image_free(image_data);
        return std::nullopt;
    }

    Logger::custom(fg(fmt::color::yellow), "ImageData", "Mapping upload buffer for data copy...");
    void* mapped = nullptr;
    D3D12_RANGE range = { 0, uploadSize };
    hr = uploadBuffer->Map(0, &range, &mapped);
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: Failed to map upload buffer. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        stbi_image_free(image_data);
        return std::nullopt;
    }
    if (!mapped) {
        Logger::custom(fg(fmt::color::red), "ImageData", "ERROR: Mapped pointer is null");
        stbi_image_free(image_data);
        return std::nullopt;
    }

    Logger::custom(fg(fmt::color::yellow), "ImageData", "Copying image data to upload buffer...");
    for (int y = 0; y < height; y++)
        memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * width * 4, width * 4);
    uploadBuffer->Unmap(0, &range);
    Logger::custom(fg(fmt::color::green), "ImageData", "Data copy completed, upload buffer unmapped");

    ImageData result;
    result.width = width;
    result.height = height;
    result.data.assign(image_data, image_data + (width * height * 4));
    result.uploadBuffer = std::move(uploadBuffer);
    result.uploadPitch = uploadPitch;
    result.uploadSize = uploadSize;
    stbi_image_free(image_data);
    
    Logger::custom(fg(fmt::color::green), "ImageData", "ImageData creation completed successfully for resource {}", resourceId);
    return result;
}

static winrt::com_ptr<ID3D12CommandQueue> uploadCommandQueue = nullptr;
static winrt::com_ptr<ID3D12CommandAllocator> uploadCommandAllocator = nullptr;
static winrt::com_ptr<ID3D12Fence> uploadFence = nullptr;
static UINT64 uploadFenceValue = 0;
static HANDLE uploadEvent = nullptr;

bool FlarialGUI::LoadImageFromResource(int resourceId, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, LPCTSTR type) {
    Logger::custom(fg(fmt::color::cyan), "ImageLoad", "Starting D3D12 image load for resource ID: {}", resourceId);
    
    if (!SwapchainHook::swapchain) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: SwapchainHook::swapchain is null");
        return false;
    }
    if (!SwapchainHook::d3d12Device5) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: SwapchainHook::d3d12Device5 is null");
        return false;
    }
    
    // Log device capabilities and adapter information for Intel debugging
    static bool loggedOnce = false;
    if (!loggedOnce) {
        loggedOnce = true;
        
        // Get adapter description
        winrt::com_ptr<IDXGIFactory4> factory;
        if (SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(factory.put())))) {
            winrt::com_ptr<IDXGIAdapter1> adapter;
            if (SUCCEEDED(factory->EnumAdapters1(0, adapter.put()))) {
                DXGI_ADAPTER_DESC1 desc;
                if (SUCCEEDED(adapter->GetDesc1(&desc))) {
                    std::wstring adapterDesc(desc.Description);
                    std::string adapterStr(adapterDesc.begin(), adapterDesc.end());
                    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "GPU Adapter: {}", adapterStr);
                    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "VRAM: {} MB, System RAM: {} MB, Shared RAM: {} MB", 
                                   desc.DedicatedVideoMemory / (1024*1024), 
                                   desc.DedicatedSystemMemory / (1024*1024),
                                   desc.SharedSystemMemory / (1024*1024));
                    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "VendorID: 0x{:X}, DeviceID: 0x{:X}", desc.VendorId, desc.DeviceId);
                    
                    // Check if Intel GPU (VendorID 0x8086)
                    if (desc.VendorId == 0x8086) {
                        Logger::custom(fg(fmt::color::orange), "ImageLoad", "*** INTEL GPU DETECTED - Enhanced logging enabled ***");
                    }
                }
            }
        }
        
        // Check D3D12 feature support
        D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
        if (SUCCEEDED(SwapchainHook::d3d12Device5->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options)))) {
            Logger::custom(fg(fmt::color::yellow), "ImageLoad", "D3D12 Feature Level: ResourceBindingTier={}, ResourceHeapTier={}", 
                           static_cast<int>(options.ResourceBindingTier), static_cast<int>(options.ResourceHeapTier));
        }
    }
    
    Logger::custom(fg(fmt::color::green), "ImageLoad", "D3D12 device validation passed");

    // Initialize static resources
    if (!uploadCommandQueue) {
        Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Creating upload command queue...");
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.NodeMask = 1;
        HRESULT hr = SwapchainHook::d3d12Device5->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(uploadCommandQueue.put()));
        if (FAILED(hr)) {
            Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to create command queue. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
            return false;
        }
        Logger::custom(fg(fmt::color::green), "ImageLoad", "Upload command queue created successfully");
    }
    if (!uploadCommandAllocator) {
        Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Creating upload command allocator...");
        HRESULT hr = SwapchainHook::d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(uploadCommandAllocator.put()));
        if (FAILED(hr)) {
            Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to create command allocator. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
            return false;
        }
        Logger::custom(fg(fmt::color::green), "ImageLoad", "Upload command allocator created successfully");
    }
    if (!uploadFence) {
        Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Creating upload fence...");
        HRESULT hr = SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(uploadFence.put()));
        if (FAILED(hr)) {
            Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to create fence. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
            return false;
        }
        Logger::custom(fg(fmt::color::green), "ImageLoad", "Upload fence created successfully");
    }
    if (!uploadEvent) {
        Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Creating upload event...");
        uploadEvent = CreateEvent(0, 0, 0, 0);
        if (!uploadEvent) {
            Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to create event handle. GetLastError: {}", GetLastError());
            return false;
        }
        Logger::custom(fg(fmt::color::green), "ImageLoad", "Upload event created successfully");
    }

    // Load image data and prepare upload buffer on background thread
    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Loading image data for resource {}...", resourceId);
    auto future = std::async(std::launch::async, LoadImageDataFromResource, resourceId, type, SwapchainHook::d3d12Device5);
    auto image_data_opt = future.get();
    if (!image_data_opt) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to load image data for resource {}", resourceId);
        return false;
    }
    const auto& image_data = *image_data_opt;
    Logger::custom(fg(fmt::color::green), "ImageLoad", "Image data loaded successfully. Size: {}x{}, Upload buffer: 0x{:X}", 
                   image_data.width, image_data.height, 
                   reinterpret_cast<uintptr_t>(image_data.uploadBuffer.get()));

    // Create texture resource
    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Creating D3D12 texture resource...");
    D3D12_HEAP_PROPERTIES props = {};
    props.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = image_data.width;
    desc.Height = image_data.height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    Logger::custom(fg(fmt::color::cyan), "ImageLoad", "Texture desc: {}x{}, Format: DXGI_FORMAT_R8G8B8A8_UNORM", desc.Width, desc.Height);

    winrt::com_ptr<ID3D12Resource> pTexture;
    HRESULT hr = SwapchainHook::d3d12Device5->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(pTexture.put()));
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to create texture resource. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        return false;
    }
    if (!pTexture) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Texture resource is null after creation");
        return false;
    }
    Logger::custom(fg(fmt::color::green), "ImageLoad", "Texture resource created successfully. Address: 0x{:X}", reinterpret_cast<uintptr_t>(pTexture.get()));

    // Set up command list
    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Setting up command list for texture upload...");
    hr = uploadCommandAllocator->Reset();
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to reset command allocator. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        return false;
    }
    
    winrt::com_ptr<ID3D12GraphicsCommandList> cmdList;
    hr = SwapchainHook::d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, uploadCommandAllocator.get(), NULL, IID_PPV_ARGS(cmdList.put()));
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to create command list. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        return false;
    }
    if (!cmdList) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Command list is null after creation");
        return false;
    }
    Logger::custom(fg(fmt::color::green), "ImageLoad", "Command list created successfully");

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = image_data.uploadBuffer.get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srcLocation.PlacedFootprint.Footprint.Width = image_data.width;
    srcLocation.PlacedFootprint.Footprint.Height = image_data.height;
    srcLocation.PlacedFootprint.Footprint.Depth = 1;
    srcLocation.PlacedFootprint.Footprint.RowPitch = image_data.uploadPitch;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = pTexture.get();
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = 0;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = pTexture.get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Recording copy commands...");
    cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
    cmdList->ResourceBarrier(1, &barrier);
    hr = cmdList->Close();
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to close command list. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        return false;
    }

    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Executing command list...");
    ID3D12CommandList* cmdListPtr = cmdList.get();
    uploadCommandQueue->ExecuteCommandLists(1, &cmdListPtr);
    
    hr = uploadCommandQueue->Signal(uploadFence.get(), ++uploadFenceValue);
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to signal fence. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        return false;
    }
    
    hr = uploadFence->SetEventOnCompletion(uploadFenceValue, uploadEvent);
    if (FAILED(hr)) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Failed to set event on completion. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
        return false;
    }
    
    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Waiting for upload completion...");
    DWORD waitResult = WaitForSingleObject(uploadEvent, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        Logger::custom(fg(fmt::color::red), "ImageLoad", "ERROR: Wait failed. Result: {}, GetLastError: {}", waitResult, GetLastError());
        return false;
    }
    Logger::custom(fg(fmt::color::green), "ImageLoad", "Upload completed successfully");

    // Create shader resource view
    Logger::custom(fg(fmt::color::yellow), "ImageLoad", "Creating shader resource view...");
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    
    Logger::custom(fg(fmt::color::cyan), "ImageLoad", "SRV CPU handle: 0x{:X}", srv_cpu_handle.ptr);
    SwapchainHook::d3d12Device5->CreateShaderResourceView(pTexture.get(), &srvDesc, srv_cpu_handle);

    *out_tex_resource = pTexture.detach();
    Logger::custom(fg(fmt::color::green), "ImageLoad", "D3D12 image loading completed successfully for resource {}", resourceId);

    return true;
}

//void FlarialGUI::LoadImageFromResource()

void FlarialGUI::image(int resourceId, D2D1_RECT_F rect, LPCTSTR type, bool shouldadd, ImColor col) {
    Logger::custom(fg(fmt::color::magenta), "Image", "=== FlarialGUI::image called for resource {} ===", resourceId);

	if(Client::settings.getSettingByName<bool>("noicons")->value) {
        Logger::custom(fg(fmt::color::yellow), "Image", "Icons disabled in settings, skipping resource {}", resourceId);
        return;
    }

	if (!ImGui::GetBackgroundDrawList()) {
		Logger::custom(fg(fmt::color::red), "Image", "ERROR: ImGui background drawlist is nullptr");
		return;
	}

    if (isInScrollView and shouldadd) {
        rect.top += scrollpos;
        rect.bottom += scrollpos;
    }
    if (shouldAdditionalY) {
        rect.top += additionalY[additionalIndex];
        rect.bottom += additionalY[additionalIndex];
    }

    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);

	/*
    if (ImagesClass::images[resourceId] == nullptr)
        LoadImageFromResource(resourceId, &ImagesClass::images[resourceId], type);
        */

    if (SwapchainHook::queue == nullptr) {
        Logger::custom(fg(fmt::color::cyan), "Image", "Using D3D11 path for resource {}", resourceId);
		if (ImagesClass::ImguiDX11Images[resourceId] == nullptr) {
            Logger::custom(fg(fmt::color::yellow), "Image", "Loading D3D11 image for resource {}", resourceId);
            if (!LoadImageFromResource(resourceId, &ImagesClass::ImguiDX11Images[resourceId], type)) {
                Logger::custom(fg(fmt::color::red), "Image", "ERROR: Failed to load D3D11 image for resource {}", resourceId);
            } else {
                Logger::custom(fg(fmt::color::green), "Image", "D3D11 image loaded successfully for resource {}", resourceId);
            }
		} else  {
            Logger::custom(fg(fmt::color::cyan), "Image", "Using cached D3D11 image for resource {}", resourceId);
			ImGui::GetBackgroundDrawList()->AddImage(ImTextureID(ImagesClass::ImguiDX11Images[resourceId]), ImVec2(imageRect.left, imageRect.top), ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0), ImVec2(1, 1), col);
		}
        return;
	}
    
    Logger::custom(fg(fmt::color::cyan), "Image", "Using D3D12 path for resource {}", resourceId);

		if (!ImagesClass::ImguiDX12Images[resourceId]) {
            Logger::custom(fg(fmt::color::yellow), "Image", "Resource {} not in cache, need to load D3D12 image", resourceId);

			ID3D12Resource* my_texture = nullptr;

			if (!SwapchainHook::d3d12Device5) { 
                Logger::custom(fg(fmt::color::red), "Image", "ERROR: D3D12 device does not exist for resource {}", resourceId);
                return;
            }

			if(!SwapchainHook::d3d12DescriptorHeapImGuiIMAGE) {
                Logger::custom(fg(fmt::color::yellow), "Image", "Creating D3D12 descriptor heap for images...");

				D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
				descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				descriptorImGuiRender.NumDescriptors = MAX_IMAGE_ID;
				descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

                Logger::custom(fg(fmt::color::cyan), "Image", "Descriptor heap config: Type=CBV_SRV_UAV, NumDescriptors={}, Flags=SHADER_VISIBLE", MAX_IMAGE_ID);

				HRESULT hr = SwapchainHook::d3d12Device5->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&SwapchainHook::d3d12DescriptorHeapImGuiIMAGE));

				if (FAILED(hr)) {
					Logger::custom(fg(fmt::color::red), "Image", "ERROR: Failed to create d3d12DescriptorHeapImGuiIMAGE. HRESULT: 0x{:X}", static_cast<uint32_t>(hr));
					return;
				}
                Logger::custom(fg(fmt::color::green), "Image", "D3D12 descriptor heap created successfully");
			} else {
                Logger::custom(fg(fmt::color::cyan), "Image", "Using existing D3D12 descriptor heap");
            }

			bool ret = false;

			UINT handle_increment = SwapchainHook::d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			int descriptor_index = resourceId - 100;
			D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
			cpu.ptr += (handle_increment * descriptor_index);

			D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
			gpu.ptr += (handle_increment * descriptor_index);

            Logger::custom(fg(fmt::color::cyan), "Image", "Descriptor handles for resource {}: CPU=0x{:X}, GPU=0x{:X}, Index={}, Increment={}", 
                           resourceId, cpu.ptr, gpu.ptr, descriptor_index, handle_increment);

			ret = LoadImageFromResource(resourceId, cpu, &my_texture, type);
			if (!ret) {
                Logger::custom(fg(fmt::color::red), "Image", "ERROR: LoadImageFromResource failed for resource {}", resourceId);
				return;
            }
            
            Logger::custom(fg(fmt::color::green), "Image", "LoadImageFromResource succeeded for resource {}, caching handles", resourceId);
			ImagesClass::ImguiDX12Images[resourceId] = (ImTextureID)gpu.ptr;
			ImagesClass::ImguiDX12Textures[resourceId] = my_texture;

		}
		else {
            Logger::custom(fg(fmt::color::cyan), "Image", "Using cached D3D12 image for resource {}, GPU handle: 0x{:X}", 
                           resourceId, static_cast<uint64_t>(static_cast<uintptr_t>(ImagesClass::ImguiDX12Images[resourceId])));
			try {
				ImGui::GetBackgroundDrawList()->AddImage(ImagesClass::ImguiDX12Images[resourceId],
														 ImVec2(imageRect.left, imageRect.top),
														 ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0),
														 ImVec2(1, 1),
														 col);
                Logger::custom(fg(fmt::color::green), "Image", "Successfully rendered D3D12 image for resource {}", resourceId);
			} catch (std::exception ex) { 
                Logger::custom(fg(fmt::color::red), "Image", "ERROR: Exception when adding D3D12 image for resource {}: {}", resourceId, ex.what());
            }
		}
    
    Logger::custom(fg(fmt::color::magenta), "Image", "=== FlarialGUI::image completed for resource {} ===", resourceId);
}

void FlarialGUI::LoadAllImages() {
	if(SwapchainHook::queue) {

		if(!SwapchainHook::d3d12DescriptorHeapImGuiIMAGE) {

			D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
			descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorImGuiRender.NumDescriptors = MAX_IMAGE_ID;
			descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			SwapchainHook::d3d12Device5->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&SwapchainHook::d3d12DescriptorHeapImGuiIMAGE));
		}

		for(int i = 100; i <= MAX_IMAGE_ID; i++) {
			if(i != IDR_PATAR_JPG) {
				ID3D12Resource* my_texture = nullptr;

				UINT handle_increment = SwapchainHook::d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				int descriptor_index = i - 100;
				D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
				cpu.ptr += (handle_increment * descriptor_index);
				D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
				gpu.ptr += (handle_increment * descriptor_index);
				if(LoadImageFromResource(i, cpu, &my_texture, "PNG")) {
				ImagesClass::ImguiDX12Images[i] = (ImTextureID)gpu.ptr;
				ImagesClass::ImguiDX12Textures[i] = my_texture;
			}
			} else {
				ID3D12Resource* my_texture = nullptr;

				UINT handle_increment = SwapchainHook::d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				int descriptor_index = i - 100;
				D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
				cpu.ptr += (handle_increment * descriptor_index);
				D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
				gpu.ptr += (handle_increment * descriptor_index);
				if(LoadImageFromResource(i, cpu, &my_texture, "JPG")) {
				ImagesClass::ImguiDX12Images[i] = (ImTextureID)gpu.ptr;
				ImagesClass::ImguiDX12Textures[i] = my_texture;
			}
			}
		}

	} else {
		for(int i = 100; i <= MAX_IMAGE_ID; i++) {
			if(i != IDR_PATAR_JPG)
				LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "PNG");
			else LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "JPG");
		}
	}
}


std::future<void> FlarialGUI::LoadImagesAsync() {
	std::promise<void> promise;
	auto future = promise.get_future();
	std::thread loadThread([promise = std::move(promise)]() mutable {
		FlarialGUI::LoadAllImages();
		promise.set_value();
	});
	loadThread.detach();
	return future;
}

void FlarialGUI::CleanupImageResources() {
	// Clean up static D3D12 upload resources
	uploadCommandQueue = nullptr;
	uploadCommandAllocator = nullptr;
	uploadFence = nullptr;
	uploadFenceValue = 0;
	if (uploadEvent) {
		CloseHandle(uploadEvent);
		uploadEvent = nullptr;
	}
}