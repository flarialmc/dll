#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../Structs/ImagesClass.hpp"
#include "../../../../../../Assets/Assets.hpp"
#include "../../../../../Client.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include <imgui/stb.h>

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

    // Prepare upload buffer
    UINT uploadPitch = (width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    UINT uploadSize = height * uploadPitch;

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

    winrt::com_ptr<ID3D12Resource> uploadBuffer;
    HRESULT hr = device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(uploadBuffer.put()));
    if (FAILED(hr) || !uploadBuffer) {
        stbi_image_free(image_data);
        return std::nullopt;
    }

    void* mapped = nullptr;
    D3D12_RANGE range = { 0, uploadSize };
    if (FAILED(uploadBuffer->Map(0, &range, &mapped)) || !mapped) {
        stbi_image_free(image_data);
        return std::nullopt;
    }

    for (int y = 0; y < height; y++)
        memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * width * 4, width * 4);
    uploadBuffer->Unmap(0, &range);

    ImageData result;
    result.width = width;
    result.height = height;
    result.data.assign(image_data, image_data + (width * height * 4));
    result.uploadBuffer = std::move(uploadBuffer);
    result.uploadPitch = uploadPitch;
    result.uploadSize = uploadSize;
    stbi_image_free(image_data);
    return result;
}

static winrt::com_ptr<ID3D12CommandQueue> uploadCommandQueue = nullptr;
static winrt::com_ptr<ID3D12CommandAllocator> uploadCommandAllocator = nullptr;
static winrt::com_ptr<ID3D12Fence> uploadFence = nullptr;
static UINT64 uploadFenceValue = 0;
static HANDLE uploadEvent = nullptr;

bool FlarialGUI::LoadImageFromResource(int resourceId, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, LPCTSTR type) {
    if (!SwapchainHook::swapchain || !SwapchainHook::d3d12Device5) return false;

    // Initialize static resources
    if (!uploadCommandQueue) {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.NodeMask = 1;
        if (FAILED(SwapchainHook::d3d12Device5->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(uploadCommandQueue.put())))) return false;
    }
    if (!uploadCommandAllocator) {
        if (FAILED(SwapchainHook::d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(uploadCommandAllocator.put())))) return false;
    }
    if (!uploadFence) {
        if (FAILED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(uploadFence.put())))) return false;
    }
    if (!uploadEvent) {
        uploadEvent = CreateEvent(0, 0, 0, 0);
        if (!uploadEvent) return false;
    }

    // Load image data and prepare upload buffer on background thread
    auto future = std::async(std::launch::async, LoadImageDataFromResource, resourceId, type, SwapchainHook::d3d12Device5.get());
    auto image_data_opt = future.get();
    if (!image_data_opt) return false;
    const auto& image_data = *image_data_opt;

    // Create texture resource
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

    winrt::com_ptr<ID3D12Resource> pTexture;
    HRESULT hr = SwapchainHook::d3d12Device5->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(pTexture.put()));
    if (FAILED(hr) || !pTexture) {
        return false;
    }

    // Set up command list
    uploadCommandAllocator->Reset();
    winrt::com_ptr<ID3D12GraphicsCommandList> cmdList;
    hr = SwapchainHook::d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, uploadCommandAllocator.get(), NULL, IID_PPV_ARGS(cmdList.put()));
    if (FAILED(hr) || !cmdList) {
        return false;
    }

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

    cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
    cmdList->ResourceBarrier(1, &barrier);
    cmdList->Close();

    ID3D12CommandList* cmdListPtr = cmdList.get();
    uploadCommandQueue->ExecuteCommandLists(1, &cmdListPtr);
    uploadCommandQueue->Signal(uploadFence.get(), ++uploadFenceValue);
    uploadFence->SetEventOnCompletion(uploadFenceValue, uploadEvent);
    WaitForSingleObject(uploadEvent, INFINITE);

    // Create shader resource view
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SwapchainHook::d3d12Device5->CreateShaderResourceView(pTexture.get(), &srvDesc, srv_cpu_handle);

    *out_tex_resource = pTexture.detach();

    return true;
}

//void FlarialGUI::LoadImageFromResource()

void FlarialGUI::image(int resourceId, D2D1_RECT_F rect, LPCTSTR type, bool shouldadd, ImColor col) {

	if(Client::settings.getSettingByName<bool>("noicons")->value) return;

	if (!ImGui::GetBackgroundDrawList()) {
		LOG_ERROR("BG Drawlist is nullptr");
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
		if (ImagesClass::ImguiDX11Images[resourceId] == nullptr) {
            LoadImageFromResource(resourceId, &ImagesClass::ImguiDX11Images[resourceId], type);
		} else  {
			ImGui::GetBackgroundDrawList()->AddImage(ImTextureID(ImagesClass::ImguiDX11Images[resourceId]), ImVec2(imageRect.left, imageRect.top), ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0), ImVec2(1, 1), col);
		}
        return;
	}

		if (!ImagesClass::ImguiDX12Images[resourceId]) {
			ID3D12Resource* my_texture = nullptr;

			if (!SwapchainHook::d3d12Device5) return;

            if(!SwapchainHook::d3d12DescriptorHeapImGuiRender) return;

			bool ret = false;
            
            D3D12_CPU_DESCRIPTOR_HANDLE cpu;
            D3D12_GPU_DESCRIPTOR_HANDLE gpu;
            
            if (!SwapchainHook::AllocateImageDescriptor(resourceId, &cpu, &gpu)) return;

			ret = LoadImageFromResource(resourceId, cpu, &my_texture, type);
			if (!ret) return;
            
			ImagesClass::ImguiDX12Images[resourceId] = (ImTextureID)gpu.ptr;
			ImagesClass::ImguiDX12Textures[resourceId] = my_texture;

		}
		else {
			ImGui::GetBackgroundDrawList()->AddImage(ImagesClass::ImguiDX12Images[resourceId],
														 ImVec2(imageRect.left, imageRect.top),
														 ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0),
														 ImVec2(1, 1),
														 col);
		}
}

void FlarialGUI::LoadAllImages() {
	if(SwapchainHook::queue) {
        // Check if consolidated descriptor heap is available
        if(!SwapchainHook::d3d12DescriptorHeapImGuiRender) return;

		for(int i = 100; i <= MAX_IMAGE_ID; i++) {
			if(i != IDR_PATAR_JPG) {
				ID3D12Resource* my_texture = nullptr;

                D3D12_CPU_DESCRIPTOR_HANDLE cpu;
                D3D12_GPU_DESCRIPTOR_HANDLE gpu;
                
                if (!SwapchainHook::AllocateImageDescriptor(i, &cpu, &gpu)) continue;

				if(LoadImageFromResource(i, cpu, &my_texture, "PNG")) {
                    ImagesClass::ImguiDX12Images[i] = (ImTextureID)gpu.ptr;
                    ImagesClass::ImguiDX12Textures[i] = my_texture;
                }
			} else {
				ID3D12Resource* my_texture = nullptr;

                D3D12_CPU_DESCRIPTOR_HANDLE cpu;
                D3D12_GPU_DESCRIPTOR_HANDLE gpu;
                
                if (!SwapchainHook::AllocateImageDescriptor(i, &cpu, &gpu)) continue;

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