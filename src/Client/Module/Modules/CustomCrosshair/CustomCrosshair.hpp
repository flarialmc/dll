#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../Utils/Render/MaterialUtils.hpp"
#include "../../../../SDK/Client/GUI/ScreenRenderer.hpp"
#include "../../../../Assets/Assets.hpp"

#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Render/HudCursorRendererRenderEvent.hpp"
#include "Events/Game/TickEvent.hpp"

#include "Events/Game/PerspectiveEvent.hpp"
#include "Utils/Render/PositionUtils.hpp"
#include <filesystem>
#include <map>
#include <atomic>
#include <d3d11.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>


class CrosshairImage
{
public:
	std::vector<Vec4<int>> PixelData = std::vector<Vec4<int>>(256, Vec4<int>(0, 0, 0, 0));
	int Size = 16;
	bool valid = false;
	CrosshairImage(int Size);
	CrosshairImage(std::string Path);
	CrosshairImage() {};
	const unsigned char* getImageData();
	void SaveImage(std::string name);
};

class CustomCrosshair : public Module
{
private:
	Perspective currentPerspective;
	bool blankWindow;
	bool CrosshairReloaded = false;
	std::map<std::string, CrosshairImage*> crosshairs;
	std::string CurrentSelectedCrosshair;
	std::string CrosshairText = "";
	bool actuallyRenderWindow = false;

	// Thread-safe cached values
	bool isHoveringEntity = false;
	bool isValidPlayer = false;
	bool isHudScreen = false;

	// Safety flag to prevent crashes during disable
	std::atomic<bool> isRenderingSafe = true;

public:
	std::map<std::string, winrt::com_ptr<ID3D11ShaderResourceView>> crosshairTextures;
	std::map<std::string, Vec2<int>> crosshairSizes;
	std::map<std::string, std::pair<ImTextureID, winrt::com_ptr<ID3D12Resource>>> crosshairTexturesDX12;
	static inline winrt::com_ptr<ID3D11SamplerState> pointSampler = nullptr;

private:
	winrt::com_ptr<ID3D11ShaderResourceView> defaultCrosshairTexture = nullptr;
	Vec2<int> defaultCrosshairSize = Vec2<int>(16, 16);

	winrt::com_ptr<ID3D12Resource> defaultCrosshairTextureDX12 = nullptr;
	ImTextureID defaultCrosshairTextureDX12Handle = 0;
public:
	CustomCrosshair() : Module("Custom Crosshair", "Allows for dynamic crosshair colors.",
		IDR_CROSSHAIR_PNG, "") {
		
	};


	void onEnable() override;

	void onDisable() override;

	void CrosshairEditorWindow();

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onGetViewPerspective(PerspectiveEvent& event);

	void onHudCursorRendererRender(HudCursorRendererRenderEvent& event);

	void onTick(TickEvent& event);

	void onRender(RenderEvent &event);

	void invalidateCrosshairTexture(const std::string& crosshairName);
	void cleanupTextures();
	void cleanupTexturesDX12();
	void reinitializeAfterResize();

private:
	ImTextureID loadCrosshairTexture(const std::string& crosshairName);
	ImTextureID loadCrosshairTextureDX12(const std::string& crosshairName);
	void loadDefaultCrosshairTexture();
	void loadDefaultCrosshairTextureDX12();
	void renderImGuiCrosshair();
	static void cleanupSamplerStates();
};
