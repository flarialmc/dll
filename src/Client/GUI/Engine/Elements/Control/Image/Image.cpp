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

    HRSRC imageResHandle = nullptr;
    HGLOBAL imageResDataHandle = nullptr;
    const unsigned char* pImageFile = nullptr;
    DWORD imageFileSize = 0;

    // Locate the resource
    imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);

    // Load the resource
    imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);

    // Lock the resource to get a pointer to the image data
    pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
    imageFileSize = SizeofResource(Client::currentModule, imageResHandle);


    int width, height, channels;
	unsigned char* image_data = stbi_load_from_memory(pImageFile, imageFileSize, &width, &height, &channels, 4);
	if (image_data == NULL) {
		return false;
	}

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = nullptr;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	HRESULT r = SwapchainHook::d3d11Device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	r = SwapchainHook::d3d11Device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();
    
	return true;
}

bool FlarialGUI::LoadImageFromResource(int resourceId, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, LPCTSTR type) {

	HRSRC imageResHandle = nullptr;
 HGLOBAL imageResDataHandle = nullptr;
 const unsigned char* pImageFile = nullptr;
 DWORD imageFileSize = 0;

	ID3D12Device* ImageDevice4Fun = nullptr;

	if (!SwapchainHook::swapchain) {
		std::cout << "Error: SwapchainHook::swapchain is null." << std::endl;
		return false;
	}

	HRESULT hrDevice = SwapchainHook::swapchain->GetDevice(IID_PPV_ARGS(&ImageDevice4Fun));
	if (FAILED(hrDevice)) {
		std::cout << "Error: Failed to get device from swapchain. HRESULT: " << hrDevice << std::endl;
		return false;
	}
    if (!ImageDevice4Fun) {
        std::cout << "Error: ImageDevice4Fun is null after GetDevice." << std::endl;
        return false;
    }


 imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
	if (imageResHandle == nullptr) {
		std::cout << "Error: FindResource failed for resourceId: " << resourceId << " type: " << type << std::endl;
		return false;
	}

 imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);
	if (imageResDataHandle == nullptr) {
		std::cout << "Error: LoadResource failed for resourceId: " << resourceId << std::endl;
		return false;
	}

 pImageFile = (const unsigned char*)LockResource(imageResDataHandle);
	if (pImageFile == nullptr) {
		std::cout << "Error: LockResource failed for resourceId: " << resourceId << std::endl;
		return false;
	}

 imageFileSize = SizeofResource(Client::currentModule, imageResHandle);
	if (imageFileSize == 0) {
		std::cout << "Error: SizeofResource returned 0 for resourceId: " << resourceId << std::endl;
		return false;
	}

 int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory(pImageFile, imageFileSize, &image_width, &image_height, NULL, 4);
	if (image_data == nullptr) {
		std::cout << "Error: stbi_load_from_memory failed for resourceId: " << resourceId << std::endl;
		return false;
	}

	D3D12_HEAP_PROPERTIES props = {};
	props.Type = D3D12_HEAP_TYPE_DEFAULT;
	props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = image_width;
	desc.Height = image_height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pTexture = nullptr;
	HRESULT hrCreateTexture = ImageDevice4Fun->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&pTexture));
	if (FAILED(hrCreateTexture)) {
		std::cout << "Error: CreateCommittedResource (Texture) failed. HRESULT: " << hrCreateTexture << std::endl;
		stbi_image_free(image_data);
		return false;
	}
    if (!pTexture) {
        std::cout << "Error: pTexture is null after CreateCommittedResource (Texture)." << std::endl;
        stbi_image_free(image_data);
        return false;
    }


	UINT uploadPitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
	UINT uploadSize = image_height * uploadPitch;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = uploadSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	props.Type = D3D12_HEAP_TYPE_UPLOAD;
	props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	ID3D12Resource* uploadBuffer = nullptr;
	HRESULT hrCreateUploadBuffer = ImageDevice4Fun->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));
	if (FAILED(hrCreateUploadBuffer)) {
		std::cout << "Error: CreateCommittedResource (Upload Buffer) failed. HRESULT: " << hrCreateUploadBuffer << std::endl;
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}
    if (!uploadBuffer) {
        std::cout << "Error: uploadBuffer is null after CreateCommittedResource (Upload Buffer)." << std::endl;
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }


	void* mapped = nullptr;
	D3D12_RANGE range = { 0, uploadSize };
	HRESULT hrMap = uploadBuffer->Map(0, &range, &mapped);
	if (FAILED(hrMap)) {
		std::cout << "Error: uploadBuffer->Map failed. HRESULT: " << hrMap << std::endl;
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}
    if (!mapped) {
        std::cout << "Error: mapped pointer is null after uploadBuffer->Map." << std::endl;
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }

	for (int y = 0; y < image_height; y++)
		memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * image_width * 4, image_width * 4);
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
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pTexture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	ID3D12Fence* fence = nullptr;
	HRESULT hrCreateFence = ImageDevice4Fun->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if (FAILED(hrCreateFence)) {
		std::cout << "Error: CreateFence failed. HRESULT: " << hrCreateFence << std::endl;
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}
    if (!fence) {
        std::cout << "Error: fence is null after CreateFence." << std::endl;
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }


	HANDLE event = CreateEvent(0, 0, 0, 0);
	if (event == nullptr) {
		std::cout << "Error: CreateEvent failed." << std::endl;
		fence->Release();
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 1;

	ID3D12CommandQueue* cmdQueue = nullptr;
	HRESULT hrCreateCommandQueue = ImageDevice4Fun->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
	if (FAILED(hrCreateCommandQueue)) {
		std::cout << "Error: CreateCommandQueue failed. HRESULT: " << hrCreateCommandQueue << std::endl;
		CloseHandle(event);
		fence->Release();
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}
    if (!cmdQueue) {
        std::cout << "Error: cmdQueue is null after CreateCommandQueue." << std::endl;
        CloseHandle(event);
        fence->Release();
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }


	ID3D12CommandAllocator* cmdAlloc = nullptr;
	HRESULT hrCreateCommandAllocator = ImageDevice4Fun->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
	if (FAILED(hrCreateCommandAllocator)) {
		std::cout << "Error: CreateCommandAllocator failed. HRESULT: " << hrCreateCommandAllocator << std::endl;
		cmdQueue->Release();
		CloseHandle(event);
		fence->Release();
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}
    if (!cmdAlloc) {
        std::cout << "Error: cmdAlloc is null after CreateCommandAllocator." << std::endl;
        cmdQueue->Release();
        CloseHandle(event);
        fence->Release();
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }


	ID3D12GraphicsCommandList* cmdList = nullptr;
	HRESULT hrCreateCommandList = ImageDevice4Fun->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
	if (FAILED(hrCreateCommandList)) {
		std::cout << "Error: CreateCommandList failed. HRESULT: " << hrCreateCommandList << std::endl;
		cmdAlloc->Release();
		cmdQueue->Release();
		CloseHandle(event);
		fence->Release();
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}
    if (!cmdList) {
        std::cout << "Error: cmdList is null after CreateCommandList." << std::endl;
        cmdAlloc->Release();
        cmdQueue->Release();
        CloseHandle(event);
        fence->Release();
        uploadBuffer->Release();
        pTexture->Release();
        stbi_image_free(image_data);
        return false;
    }


	cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
	cmdList->ResourceBarrier(1, &barrier);

	HRESULT hrCloseCommandList = cmdList->Close();
	if (FAILED(hrCloseCommandList)) {
		std::cout << "Error: cmdList->Close failed. HRESULT: " << hrCloseCommandList << std::endl;
		cmdList->Release();
		cmdAlloc->Release();
		cmdQueue->Release();
		CloseHandle(event);
		fence->Release();
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}

	cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
	HRESULT hrSignal = cmdQueue->Signal(fence, 1);
	if (FAILED(hrSignal)) {
		std::cout << "Error: cmdQueue->Signal failed. HRESULT: " << hrSignal << std::endl;
		cmdList->Release();
		cmdAlloc->Release();
		cmdQueue->Release();
		CloseHandle(event);
		fence->Release();
		uploadBuffer->Release();
		pTexture->Release();
		stbi_image_free(image_data);
		return false;
	}

	fence->SetEventOnCompletion(1, event);
	WaitForSingleObject(event, INFINITE);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	ImageDevice4Fun->CreateShaderResourceView(pTexture, &srvDesc, srv_cpu_handle);
    if (!srv_cpu_handle.ptr) {
        std::cout << "Warning: srv_cpu_handle is null, SRV might not be created correctly, but continuing." << std::endl;
    }


	*out_tex_resource = pTexture;
    if (!*out_tex_resource) {
        std::cout << "Error: out_tex_resource is still null after all operations, something went wrong." << std::endl;
        cmdList->Release();
        cmdAlloc->Release();
        cmdQueue->Release();
        CloseHandle(event);
        fence->Release();
        uploadBuffer->Release();
        pTexture->Release(); // Redundant but safe in case of logic error.
        stbi_image_free(image_data);
        return false;
    }


    cmdList->Release();
 cmdAlloc->Release();
 cmdQueue->Release();
 CloseHandle(event);
 fence->Release();
 uploadBuffer->Release();
	stbi_image_free(image_data);

	return true;
}

//void FlarialGUI::LoadImageFromResource()

void FlarialGUI::image(int resourceId, D2D1_RECT_F rect, LPCTSTR type, bool shouldadd) {

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
			ImGui::GetBackgroundDrawList()->AddImage(ImTextureID(ImagesClass::ImguiDX11Images[resourceId]), ImVec2(imageRect.left, imageRect.top), ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE);
		}
        return;
	}

		if (!ImagesClass::ImguiDX12Images[resourceId]) {
			if (sizeof(ImTextureID) >= sizeof(D3D12_CPU_DESCRIPTOR_HANDLE)) Logger::error("imtexID >= descriptor handle");
			static_assert(sizeof(ImTextureID) >= sizeof(D3D12_CPU_DESCRIPTOR_HANDLE), "D3D12_CPU_DESCRIPTOR_HANDLE is too large to fit in an ImTextureID");

			int my_image_width = 0;
			int my_image_height = 0;
			ID3D12Resource* my_texture = NULL;

			ID3D12Device* ImageDevice4Fun;
			SwapchainHook::swapchain->GetDevice(IID_PPV_ARGS(&ImageDevice4Fun));
			if (!ImageDevice4Fun){ Logger::error("Image device does not exist."); return;}

			if(!SwapchainHook::d3d12DescriptorHeapImGuiIMAGE) {

				D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
				descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				descriptorImGuiRender.NumDescriptors = MAX_IMAGE_ID;
				descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

				HRESULT hr = ImageDevice4Fun->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&SwapchainHook::d3d12DescriptorHeapImGuiIMAGE));

				if (FAILED(hr)) {
					Logger::error("Fail at creating d3d12DescriptorHeapImGuiIMAGE: ");
					return;
				}
			}

			bool ret = false;

			UINT handle_increment = ImageDevice4Fun->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			int descriptor_index = ImagesClass::ImguiDX12Images.size();
			D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
			cpu.ptr += (handle_increment * descriptor_index);

			D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
			gpu.ptr += (handle_increment * descriptor_index);

			Memory::SafeRelease(ImageDevice4Fun);

			ret = LoadImageFromResource(resourceId, cpu, &my_texture, type);
			if (!ret)
				return;
			ImagesClass::ImguiDX12Images[resourceId] = (ImTextureID)gpu.ptr;

		}
		else {
			ImGui::GetBackgroundDrawList()->AddImage(ImagesClass::ImguiDX12Images[resourceId], ImVec2(imageRect.left, imageRect.top), ImVec2(imageRect.right, imageRect.bottom), ImVec2(0, 0), ImVec2(1, 1), FlarialGUI::D2DColorToImColor(FlarialGUI::HexToColorF("FAFAFA")));
		}



}

void FlarialGUI::LoadAllImages() {
	if(SwapchainHook::queue) {
		ID3D12Device* ImageDevice4Fun;
		SwapchainHook::swapchain->GetDevice(IID_PPV_ARGS(&ImageDevice4Fun));

		if(!SwapchainHook::d3d12DescriptorHeapImGuiIMAGE) {

			D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
			descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorImGuiRender.NumDescriptors = MAX_IMAGE_ID;
			descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			ImageDevice4Fun->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&SwapchainHook::d3d12DescriptorHeapImGuiIMAGE));
		}

		for(int i = 100; i <= MAX_IMAGE_ID; i++) {
			if(i != IDR_PATAR_JPG) {
				ID3D12Resource* my_texture = NULL;

				UINT handle_increment = ImageDevice4Fun->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				int descriptor_index = ImagesClass::ImguiDX12Images.size();
				D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
				cpu.ptr += (handle_increment * descriptor_index);
				D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
				gpu.ptr += (handle_increment * descriptor_index);
				if(LoadImageFromResource(i, cpu, &my_texture, "PNG")) ImagesClass::ImguiDX12Images[i] = (ImTextureID)gpu.ptr;
			} else {
				ID3D12Resource* my_texture = NULL;

				UINT handle_increment = ImageDevice4Fun->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				int descriptor_index = ImagesClass::ImguiDX12Images.size();
				D3D12_CPU_DESCRIPTOR_HANDLE cpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetCPUDescriptorHandleForHeapStart();
				cpu.ptr += (handle_increment * descriptor_index);
				D3D12_GPU_DESCRIPTOR_HANDLE gpu = SwapchainHook::d3d12DescriptorHeapImGuiIMAGE->GetGPUDescriptorHandleForHeapStart();
				gpu.ptr += (handle_increment * descriptor_index);
				if(LoadImageFromResource(i, cpu, &my_texture, "JPG")) ImagesClass::ImguiDX12Images[i] = (ImTextureID)gpu.ptr;
			}
		}

	} else {
		for(int i = 100; i <= MAX_IMAGE_ID; i++) {
			std::cout << i << std::endl;
			if(i != IDR_PATAR_JPG) LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "PNG");
			else LoadImageFromResource(i, &ImagesClass::ImguiDX11Images[i], "JPG");
		}
	}
}