#include "Engine.hpp"
#include "../../Client.hpp"
#include "../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include <imgui/imgui_internal.h>

// Essential DirectX headers for resource management
#include <dwrite.h>
#include <wincodec.h>
#include <winrt/base.h>
#include <wrl/client.h>

// ImGui FreeType headers
#include <imgui/misc/freetype/imgui_freetype.h>

// Resource loading and management functions

void FlarialGUI::ExtractImageResource(int resourceId, std::string fileName, LPCTSTR type) {
	LPVOID pFileData = NULL;
	DWORD dwFileSize = 0;

	HRSRC hRes = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);
	if (hRes == NULL)
		return;

	HGLOBAL hResData = LoadResource(Client::currentModule, hRes);
	if (hResData == NULL)
		return;

	pFileData = LockResource(hResData);
	if (pFileData == NULL)
		return;

	dwFileSize = SizeofResource(Client::currentModule, hRes);

	std::string fileType(type);

	std::filesystem::path path(fmt::format("{}\\{}", Utils::getAssetsPath(), fileName));

	std::ofstream outFile(path, std::ios::binary);
	if (!outFile) {
		return;
	}
	outFile.write(reinterpret_cast<const char*>(pFileData), dwFileSize);
	outFile.close();
}

void FlarialGUI::LoadFont(int resourceId) {
}

std::wstring FlarialGUI::GetFontFilePath(const std::wstring& fontName, DWRITE_FONT_WEIGHT weight) {
	Microsoft::WRL::ComPtr<IDWriteFactory> factory;
	Microsoft::WRL::ComPtr<IDWriteFontCollection> fontCollection;
	Microsoft::WRL::ComPtr<IDWriteFontFamily> fontFamily;
	Microsoft::WRL::ComPtr<IDWriteFont> font;
	Microsoft::WRL::ComPtr<IDWriteFontFace> fontFace;
	Microsoft::WRL::ComPtr<IDWriteFontFile> fontFile;
	const void* fontFileReferenceKey;
	UINT32 fontFileReferenceKeySize;
	Microsoft::WRL::ComPtr<IDWriteFontFileLoader> fontFileLoader;
	Microsoft::WRL::ComPtr<IDWriteLocalFontFileLoader> localFontFileLoader;
	UINT32 filePathLength = 0;

	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &factory);
	if (FAILED(hr)) {
		return L"";
	}

	hr = factory->GetSystemFontCollection(&fontCollection);
	if (FAILED(hr)) {
		return L"";
	}

	UINT32 index = 0;
	BOOL exists = FALSE;
	hr = fontCollection->FindFamilyName(fontName.c_str(), &index, &exists);
	if (FAILED(hr) || !exists) {
		return L"";
	}

	hr = fontCollection->GetFontFamily(index, &fontFamily);
	if (FAILED(hr)) {
		return L"";
	}

	UINT32 fontCount = fontFamily->GetFontCount();
	if (fontCount == 0) {
		return L"";
	}

	UINT32 fontIndex = -1;
	for (UINT32 i = 0; i < fontCount; ++i) {
		Microsoft::WRL::ComPtr<IDWriteFont> tempFont;
		hr = fontFamily->GetFont(i, &tempFont);
		if (FAILED(hr)) {
			continue;
		}
		if (tempFont->GetWeight() == weight) {

			fontIndex = i;
			fontFamily->GetFont(fontIndex, &font);
			break;
		}
	}


	if (fontIndex == -1) {
		if (FAILED(fontFamily->GetFont(0, &font))) return L"";
	}

	hr = font->CreateFontFace(&fontFace);
	if (FAILED(hr)) {
		return L"";
	}

	UINT32 fileCount = 0;
	hr = fontFace->GetFiles(&fileCount, nullptr);
	if (FAILED(hr) || fileCount == 0) {
		return L"";
	}

	std::vector<Microsoft::WRL::ComPtr<IDWriteFontFile>> fontFiles(fileCount);

	hr = fontFace->GetFiles(&fileCount, reinterpret_cast<IDWriteFontFile**>(fontFiles.data()));
	if (FAILED(hr)) {
		return L"";
	}

	hr = fontFiles[0]->GetReferenceKey(&fontFileReferenceKey, &fontFileReferenceKeySize);
	if (FAILED(hr)) {
		return L"";
	}

	hr = fontFiles[0]->GetLoader(&fontFileLoader);
	if (FAILED(hr)) {
		return L"";
	}

	hr = fontFileLoader->QueryInterface(__uuidof(IDWriteLocalFontFileLoader), &localFontFileLoader);
	if (FAILED(hr)) {
		return L"";
	}

	hr = localFontFileLoader->GetFilePathLengthFromKey(fontFileReferenceKey, fontFileReferenceKeySize, &filePathLength);
	if (FAILED(hr)) {
		return L"";
	}

	std::vector<wchar_t> filePathBuffer(filePathLength + 1);
	hr = localFontFileLoader->GetFilePathFromKey(fontFileReferenceKey, fontFileReferenceKeySize, filePathBuffer.data(), filePathLength + 1);
	if (FAILED(hr)) {
		return L"";
	}

	return std::wstring(filePathBuffer.data(), filePathLength);
}

std::vector<std::byte> ConvertFontDataToVector(LPVOID data, size_t size) {
	auto* bytePtr = static_cast<std::byte*>(data);
	return std::vector<std::byte>(bytePtr, bytePtr + size);
}

void FlarialGUI::queueFontMemoryLoad(std::wstring filepath, FontKey fontK, int ResourceID) {
	std::string tral = WideToNarrow(filepath);
	Logger::debug("Queueing font load for: {}, {}, {}", fontK.name, cached_to_string(fontK.weight), fontK.size);
	std::thread([tral, filepath, fontK, ResourceID]() {
		if (ResourceID > 0) {
			LPVOID pFontData = NULL;
			DWORD dwFontSize = 0;

			HRSRC hRes = FindResource(Client::currentModule, MAKEINTRESOURCE(ResourceID), RT_RCDATA);
			if (hRes == NULL)
				return;

			HGLOBAL hResData = LoadResource(Client::currentModule, hRes);
			if (hResData == NULL)
				return;

			pFontData = LockResource(hResData);
			if (pFontData == NULL)
				return;

			dwFontSize = SizeofResource(Client::currentModule, hRes);

			FontMemoryToLoad.push_back(std::pair(ConvertFontDataToVector(pFontData, dwFontSize), fontK));
		}
		std::ifstream fontFile(tral, std::ios::binary);
		if (fontFile.is_open()) {
			Logger::debug("Path {}", tral);
			std::vector<std::byte> fontData = Memory::readFile(filepath);
			fontFile.close();
			FontMemoryToLoad.push_back(std::pair(fontData, fontK));
		}
		}).detach();
}

bool FlarialGUI::LoadFontFromFontFamily(FontKey fontK) {

	if (!FlarialGUI::FontMemoryToLoad.empty()) {
		for (auto it = FlarialGUI::FontMemoryToLoad.begin(); it != FlarialGUI::FontMemoryToLoad.end(); ) {
			if (fontK.name == "Space Grotesk") fontK.name = "162";
			auto ogit = it;
			if (!FontMap[it->second])
			{

				ImFontConfig config;
				if (Client::settings.getSettingByName<bool>("pixelateFonts")->value) {
					config.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_Monochrome;
					config.OversampleH = 1;
					config.OversampleV = 1;
				} else config.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_MonoHinting;

				config.FontDataOwnedByAtlas = false;
				int FontDataSize = static_cast<int>(it->first.size());

				if (FontDataSize < 100) {
					LOG_ERROR("Error Loading Font. Font size is less than 100");
					return false;
				};
				const std::vector<int> fontSizeBuckets = { 16, 32, 64, 128, 256 };
				FontKey og = it->second;
				for (int rsize : fontSizeBuckets) {
					it->second.size = rsize;
					if (!FontMap[it->second])
						FontMap[it->second] = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(it->first.data(), static_cast<int>(it->first.size()), it->second.size, &config, ImGui::GetIO().Fonts->GetGlyphRangesDefault());
				}
				it->second.size = og.size;
			}

			it = FlarialGUI::FontMemoryToLoad.erase(ogit);
			HasAFontLoaded = true;
		}
	}

	std::string name = fontK.name;
	std::ranges::transform(name, name.begin(), ::tolower);
	std::wstring fontName = to_wide(name);
	std::wstring fontFilePath = GetFontFilePath(fontName, fontK.weight);
	std::string path;

	if (!FontMap[fontK])
	{
		Logger::debug("Queuing font load for: {}, {}, {}", fontK.name, cached_to_string(fontK.weight), fontK.size);

		if (fontK.name == "162" or name == "space grotesk") {
			queueFontMemoryLoad(L"", fontK, 162);
			FontKey l = fontK;
			l.name = "163";
			l.weight = DWRITE_FONT_WEIGHT_BOLD;
			queueFontMemoryLoad(L"", l, 163);

			return true;
		}
		else if (fontK.name == "164" or name == "minecraft") {
			queueFontMemoryLoad(L"", fontK, 164);
			return true;
		}

		if (!fontFilePath.empty()) {

			std::ifstream fontFile(fontFilePath, std::ios::binary);
			if (fontFile.is_open()) {
				Logger::debug("Path {}", WideToNarrow(fontFilePath));
				queueFontMemoryLoad(fontFilePath, fontK);

				return true;
			}
		}

		FontsNotFound[fontK] = true;
		return false;
	}
	return false;

}

void FlarialGUI::LoadImageFromResource(int resourceId, ID2D1Bitmap** bitmap, LPCTSTR type) {
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pFrame = nullptr;
	IWICStream* pStream = nullptr;
	HRSRC imageResHandle = nullptr;
	HGLOBAL imageResDataHandle = nullptr;
	void* pImageFile = nullptr;
	DWORD imageFileSize = 0;

	imageResHandle = FindResource(Client::currentModule, MAKEINTRESOURCE(resourceId), type);

	imageResDataHandle = LoadResource(Client::currentModule, imageResHandle);

	pImageFile = LockResource(imageResDataHandle);
	imageFileSize = SizeofResource(Client::currentModule, imageResHandle);

	IWICImagingFactory* imagingFactory = nullptr;
	CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));

	imagingFactory->CreateStream(&pStream);

	pStream->InitializeFromMemory(static_cast<BYTE*>(pImageFile), imageFileSize);

	imagingFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);

	pDecoder->GetFrame(0, &pFrame);

	IWICFormatConverter* pConverter = NULL;
	imagingFactory->CreateFormatConverter(&pConverter);
	pConverter->Initialize(pFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0,
		WICBitmapPaletteTypeMedianCut);

	D2D::context->CreateBitmapFromWicBitmap(pConverter, nullptr, bitmap);

	if (pConverter)
		pConverter->Release();
	if (pDecoder)
		pDecoder->Release();
	if (pFrame)
		pFrame->Release();
	if (pStream)
		pStream->Release();
}

void FlarialGUI::LoadImageFromFile(const wchar_t* filename, ID2D1Bitmap** bitmap) {
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IWICBitmapDecoder* bitmapDecoder = nullptr;
	IWICBitmapFrameDecode* frame = nullptr;
	IWICFormatConverter* formatConverter = nullptr;

	IWICImagingFactory* imagingFactory = nullptr;
	CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));

	imagingFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand,
		&bitmapDecoder);

	bitmapDecoder->GetFrame(0, &frame);

	imagingFactory->CreateFormatConverter(&formatConverter);
	formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0,
		WICBitmapPaletteTypeMedianCut);

	D2D::context->CreateBitmapFromWicBitmap(formatConverter, nullptr, bitmap);

	bitmapDecoder->Release();
	frame->Release();
	formatConverter->Release();
	imagingFactory->Release();
}