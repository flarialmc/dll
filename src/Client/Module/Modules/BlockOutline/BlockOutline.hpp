#pragma once

#include <Modules/Module.hpp>
#include <Utils/Utils.hpp>
#include <Utils/Memory/CustomAllocator/Buffer.hpp>

#include <Utils/Render/DrawUtil3D.hpp>
#include <Utils/Render/MaterialUtils.hpp>

#include "Events/Render/RenderOutlineSelectionEvent.hpp"
#include "SDK/Client/Options/OptionsParser.hpp"

class BlockOutline : public Module {

public:
	OptionsParser options;
	BlockOutline() : Module("Block Outline", "Changes the block outline color", IDR_BLOCK_PNG, "") {
		Module::setup();
		options.parseOptionsFile();

	};

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void onOutlineSelection(RenderOutlineSelectionEvent& event);
};
