#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../Utils/Render/MaterialUtils.hpp"
#include "../../../../SDK/Client/GUI/ScreenRenderer.hpp"
#include "../../../../Assets/Assets.hpp"

#include "Events/Game/PerspectiveEvent.hpp"
#include "Events/Render/HudCursorRendererRenderEvent.hpp"

#include "Events/Game/PerspectiveEvent.hpp"
#include "Utils/Render/PositionUtils.hpp"
#include <filesystem>
#include "Events/Events.hpp"


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

	void onRender(RenderEvent &event);
};