#pragma once

#include "../Module.hpp"
#include "Events/Game/TickEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"
#include "Utils/Render/PositionUtils.hpp"
#include "../../../../SDK/Client/Block/BlockLegacy.hpp"
#include "../../../../SDK/Client/Block/BlockSource.hpp"
#include "../../../../SDK/Client/Block/Block.hpp"

#include <string>
#include <chrono>
#include <filesystem>

class BetterHungerBar : public Module {
private:

    int gameMode = 0;
    int currentHunger = 0;
    int predictedHunger = 0;
    
    double currentSaturation = 0;
    double predictedSaturation = 0;
    double getCurrentTime();
    
    bool hasHungerEffect = false;
    bool useNewTextures = false;
    
    std::string itemName = "";
    
    // textures/ui/hunger_... system
    TexturePtr hungerFullTexture;
    TexturePtr hungerHalfTexture;
    TexturePtr hungerEffectFullTexture;
    TexturePtr hungerEffectHalfTexture;
    TexturePtr hungerBlinkTexture;
    TexturePtr hungerBackgroundTexture;
    TexturePtr hungerEffectBackgroundTexture;
    
    void loadTextures(MinecraftUIRenderContext* muirc);
    
public:

    BetterHungerBar() : Module("Better Hunger Bar", "Adds a hunger and saturation overlay similar to AppleSkin.\nLimited Texture Pack support.",
        IDR_BETTERHUNGERBAR_PNG, "") {
        
    };
    
    void onEnable() override;
    
    void onDisable() override;
    
    void defaultConfig() override;
    
    void settingsRender(float settingsOffset) override;
    
    void onTick(TickEvent& event);

    void onSetupAndRender(const SetupAndRenderEvent& event);
    
    int getPredictedHunger();
    
    double getPredictedSaturation();

	// "U" stands for the location: textures/ui/hunger_... 
	ResourceLocation getOutlineU(ResourceLocation& originalOutlineTexture);

	// "G" stands for the location: textures/gui/icons
	ResourceLocation getOutlineG(ResourceLocation& originalOutlineTexture);

};

// recycled code from CustomCrosshair //

class OutlineImage {
public:

	std::vector<Vec4<int>> PixelData = std::vector<Vec4<int>>(256, Vec4<int>(0, 0, 0, 0));
	int Size = 9;
	OutlineImage(std::string Path);
	OutlineImage() {};
	const unsigned char* getImageData();
	void SaveImage(std::string name);

};