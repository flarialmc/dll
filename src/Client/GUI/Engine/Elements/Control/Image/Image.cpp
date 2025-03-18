#include "../../../Engine.hpp"
#include "../../../Constraints.hpp"
#include "../../Structs/ImagesClass.hpp"
#include "../../../../../../Assets/Assets.hpp"
#include "../../../../../Client.hpp"
#include <imgui/imgui.h>

#define STB_IMAGE_IMPLEMENTATION
#include <imgui/stb.h>


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

bool FlarialGUI::LoadImageFromResource(int resourceId, ID3D11ShaderResourceView** out_srv, LPCTSTR type) {
    HRSRC imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
    if (imageResHandle == nullptr) {
        *out_srv = nullptr;
        return false;
    }

    HGLOBAL imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
    if (imageResDataHandle == nullptr) {
        *out_srv = nullptr;
        return false;
    }

    const unsigned char* pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
    if (pImageFile == nullptr) {
        *out_srv = nullptr;
        return false;
    }

    DWORD imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
    if (imageFileSize == 0) {
        *out_srv = nullptr;
        return false;
    }

    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory(pImageFile, imageFileSize, &width, &height, &channels, 4);
    if (image_data == nullptr) {
        *out_srv = nullptr;
        return false;
    }

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = nullptr;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    HRESULT r = SwapchainHook::d3d11Device->CreateTexture2D(&desc, &subResource, &pTexture);
    if (FAILED(r)) {
        stbi_image_free(image_data);
        *out_srv = nullptr;
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    r = SwapchainHook::d3d11Device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();
    if (FAILED(r)) {
        stbi_image_free(image_data);
        *out_srv = nullptr;
        return false;
    }

    stbi_image_free(image_data);
    return true;
}
bool FlarialGUI::LoadImageFromResource(int resourceId, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, LPCTSTR type) {
    static ID3D12CommandQueue* cmdQueue = nullptr;
    static ID3D12CommandAllocator* cmdAlloc = nullptr;
    static ID3D12Fence* fence = nullptr;
    static HANDLE event = nullptr;
    static UINT64 currentFenceValue = 0;

    if (!SwapchainHook::swapchain || !SwapchainHook::d3d12Device5) {
        return false;
    }

    if (cmdQueue == nullptr) {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 1;
        if (FAILED(SwapchainHook::d3d12Device5->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue)))) {
            return false;
        }
    }

    if (cmdAlloc == nullptr) {
        if (FAILED(SwapchainHook::d3d12Device5->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc)))) {
            return false;
        }
    }

    if (fence == nullptr) {
        if (FAILED(SwapchainHook::d3d12Device5->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))) {
            return false;
        }
    }

    if (event == nullptr) {
        event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (event == nullptr) {
            return false;
        }
    }

    HRSRC imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
    if (!imageResHandle) return false;

    HGLOBAL imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
    if (!imageResDataHandle) return false;

    const unsigned char* pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
    if (!pImageFile) return false;

    DWORD imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
    if (imageFileSize == 0) return false;

    int image_width = 0, image_height = 0;
    unsigned char* image_data = stbi_load_from_memory(pImageFile, imageFileSize, &image_width, &image_height, nullptr, 4);
    if (!image_data) return false;

    D3D12_HEAP_PROPERTIES props = { D3D12_HEAP_TYPE_DEFAULT };
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = image_width;
    desc.Height = image_height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ID3D12Resource* pTexture = nullptr;
    if (FAILED(SwapchainHook::d3d12Device5->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
            D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pTexture)))) {
        stbi_image_free(image_data);
        return false;
    }

    UINT uploadPitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
    UINT uploadSize = image_height * uploadPitch;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = uploadSize;
    desc.Height = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    props.Type = D3D12_HEAP_TYPE_UPLOAD;
    ID3D12Resource* uploadBuffer = nullptr;
    if (FAILED(SwapchainHook::d3d12Device5->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer)))) {
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }

    void* mapped = nullptr;
    D3D12_RANGE range = { 0, uploadSize };
    if (FAILED(uploadBuffer->Map(0, &range, &mapped))) {
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }
    for (int y = 0; y < image_height; y++) {
        memcpy((uint8_t*)mapped + y * uploadPitch, image_data + y * image_width * 4, image_width * 4);
    }
    uploadBuffer->Unmap(0, &range);

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = uploadBuffer;
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srcLocation.PlacedFootprint.Footprint.Width = image_width;
    srcLocation.PlacedFootprint.Footprint.Height = image_height;
    srcLocation.PlacedFootprint.Footprint.Depth = 1;
    srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = pTexture;
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dstLocation.SubresourceIndex = 0;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = pTexture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

    if (FAILED(cmdAlloc->Reset())) {
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }

    ID3D12GraphicsCommandList* cmdList = nullptr;
    if (FAILED(SwapchainHook::d3d12Device5->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, nullptr, IID_PPV_ARGS(&cmdList)))) {
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }

    cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
    cmdList->ResourceBarrier(1, &barrier);
    if (FAILED(cmdList->Close())) {
        cmdList->Release();
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }

    cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
    currentFenceValue++;
    if (FAILED(cmdQueue->Signal(fence, currentFenceValue))) {
        cmdList->Release();
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }
    if (FAILED(fence->SetEventOnCompletion(currentFenceValue, event))) {
        cmdList->Release();
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }
    WaitForSingleObject(event, INFINITE);

    cmdList->Release();
    uploadBuffer->Release();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SwapchainHook::d3d12Device5->CreateShaderResourceView(pTexture, &srvDesc, srv_cpu_handle);

    *out_tex_resource = pTexture;
    stbi_image_free(image_data);

    return true;
}

//void FlarialGUI::LoadImageFromResource()

void FlarialGUI::image(int resourceId, D2D1_RECT_F rect, LPCTSTR type, bool shouldadd, ImColor col) {

	if(Client::settings.getSettingByName<bool>("noicons")->value) return;

	if (!ImGui::GetBackgroundDrawList()) {
		Logger::error("BG Drawlist is nullptr");
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
            if (!LoadImageFromResource(resourceId, &ImagesClass::ImguiDX11Images[resourceId], type)) Logger::custom(fg(fmt::color::crimson), "Image", "Failed to load image");
		} else  {
			ImGui::GetBackgroundDrawList()->AddImage(ImTextureID(ImagesClass::ImguiDX11Images[resourceId]), ImVec2(imageRect.left, imageRect.top), ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0), ImVec2(1, 1), col);
		}
        return;
	}

		if (!ImagesClass::ImguiDX12Images[resourceId]) {

			ID3D12Resource* my_texture = NULL;


			if (!SwapchainHook::d3d12Device5){ Logger::error("Image device does not exist."); return;}

			if(!SwapchainHook::d3d12DescriptorHeapImGuiIMAGE) {

				D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
				descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				descriptorImGuiRender.NumDescriptors = MAX_IMAGE_ID;
				descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

				HRESULT hr = SwapchainHook::d3d12Device5->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&SwapchainHook::d3d12DescriptorHeapImGuiIMAGE));

				if (FAILED(hr)) {
					Logger::error("Fail at creating d3d12DescriptorHeapImGuiIMAGE: ");
					return;
				}
			}

			bool ret = false;

			UINT handle_increment = SwapchainHook::d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			int descriptor_index = ImagesClass::ImguiDX12Images.size();
			D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
			cpu.ptr += (handle_increment * descriptor_index);

			D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
			gpu.ptr += (handle_increment * descriptor_index);

			//Memory::SafeRelease(SwapchainHook::d3d12Device5);

			ret = LoadImageFromResource(resourceId, cpu, &my_texture, type);
			if (!ret)
				return;
			ImagesClass::ImguiDX12Images[resourceId] = (ImTextureID)gpu.ptr;

		}
		else {
			try {
				ImGui::GetBackgroundDrawList()->AddImage(ImagesClass::ImguiDX12Images[resourceId],
														 ImVec2(imageRect.left, imageRect.top),
														 ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0),
														 ImVec2(1, 1),
														 col);
			} catch (std::exception ex) { Logger::error("error addming image: ", ex.what()); }
		}
}

void FlarialGUI::LoadAllImages() {
    if (SwapchainHook::queue) {
        // Create the descriptor heap if it does not exist.
        if (!SwapchainHook::d3d12DescriptorHeapImGuiIMAGE) {
            D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
            descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            descriptorImGuiRender.NumDescriptors = MAX_IMAGE_ID;
            descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            SwapchainHook::d3d12Device5->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&SwapchainHook::d3d12DescriptorHeapImGuiIMAGE));
        }

        // Use an atomic counter to safely allocate descriptor indices across threads.
        std::atomic<int> descriptorCounter{0};
        std::vector<std::future<void>> futures;

        // Launch each image load asynchronously.
        for (int i = 100; i <= MAX_IMAGE_ID; i++) {
            futures.push_back(std::async(std::launch::async, [i, &descriptorCounter]() {
                ID3D12Resource* my_texture = nullptr;
                UINT handle_increment = SwapchainHook::d3d12Device5->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

                // Safely get a unique descriptor index.
                int descriptor_index = descriptorCounter.fetch_add(1);
                D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
                cpu.ptr += (handle_increment * descriptor_index);
                D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
                gpu.ptr += (handle_increment * descriptor_index);

                // Load image with appropriate format.
                if (i != IDR_PATAR_JPG) {
                    if (LoadImageFromResource(i, cpu, &my_texture, "PNG"))
                        ImagesClass::ImguiDX12Images[i] = (ImTextureID)gpu.ptr;
                } else {
                    if (LoadImageFromResource(i, cpu, &my_texture, "JPG"))
                        ImagesClass::ImguiDX12Images[i] = (ImTextureID)gpu.ptr;
                }
            }));
        }

        // Wait for all asynchronous tasks to complete.
        for (auto &f : futures)
            f.get();
    } else {
        // For the non-DX12 (DX11) case, load images asynchronously too.
        std::vector<std::future<void>> futures;
        for (int i = 100; i <= MAX_IMAGE_ID; i++) {
            futures.push_back(std::async(std::launch::async, [i]() {
                if (i != IDR_PATAR_JPG)
                    LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "PNG");
                else
                    LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "JPG");
            }));
        }
        for (auto &f : futures)
            f.get();
    }
}