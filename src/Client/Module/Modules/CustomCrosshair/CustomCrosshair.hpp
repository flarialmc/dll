#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../Utils/Render/MaterialUtils.hpp"
#include "../../../Events/Game/TickEvent.hpp"
#include "../../../../SDK/Client/GUI/ScreenRenderer.hpp"


class CustomCrosshair : public Module {
private:
	Perspective currentPerspective;
public:
	CustomCrosshair() : Module("Custom Crosshair", "Allows for dynamic crosshair colors.",
		IDR_SPEED_PNG, "") {

		Module::setup();
	};

	void onEnable() override {
		Listen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
			Listen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
			Deafen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
			Module::onDisable();
	}

	void defaultConfig() override {
		setDef("uiscale", 1.f);
		Module::defaultConfig("core");
		setDef("CustomCrosshair", false);
		setDef("highlightOnEntity", false);
		setDef("solidColorWhenHighlighted", true);
		setDef("solidColor", false);
		setDef("renderInThirdPerson", false);
		setDef("default", (std::string)"fafafa", 0.55f, false);
		setDef("enemy", (std::string)"FF0000", 1.f, false);
	}

	void settingsRender(float settingsOffset) override {

		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.12, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));

		addHeader("Custom Crosshair");
		addToggle("Solid Color", "Make crosshair a solid color / more visible", getOps<bool>("solidColor"));
		addToggle("Render in Third Person", "Weather or not to render in third person", getOps<bool>("renderInThirdPerson"));
		addToggle("Highlight on Entity", "Highlight when enemy in reach", getOps<bool>("highlightOnEntity"));
		addConditionalToggle(getOps<bool>("highlightOnEntity"), "Solid Color When Highlighted", "Use solid color when highlighted", getOps<bool>("solidColorWhenHighlighted"));
		extraPadding();

		// addHeader("Misc");
		// addSlider("UI Scale", "The size of the Crosshair (only for custom)", getOps<float>("uiscale"), 10.f, 0.f, true);

		extraPadding();

		addHeader("Colors");
		addColorPicker("Default Color", "When the enemy is not in view.", "default");
		addConditionalColorPicker(getOps<bool>("highlightOnEntity"), "Enemy Color", "When the enemy is in view.", "enemy");
		FlarialGUI::UnsetScrollView();

		resetPadding();
	}

	void onGetViewPerspective(PerspectiveEvent& event) {
		currentPerspective = event.getPerspective();
	}

	void onHudCursorRendererRender(HudCursorRendererRenderEvent& event) {
		event.cancel();
		if (!SDK::clientInstance) return;
		auto player = SDK::clientInstance->getLocalPlayer();
		if (!player) return;
		if (SDK::getCurrentScreen() != "hud_screen") return;

		auto renderInThirdPerson = getOps<bool>("renderInThirdPerson");
		if (!renderInThirdPerson && currentPerspective != Perspective::FirstPerson) return;
		bool isHoveringEnemy = (player->getLevel()->getHitResult().type == HitResultType::Entity);

		auto screenContext = event.getMinecraftUIRenderContext()->getScreenContext();

		const ResourceLocation loc("textures/ui/cross_hair", false);
		TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc, false);
		if (ptr.clientTexture == nullptr || ptr.clientTexture->clientTexture.resourcePointerBlock == nullptr) {
			return;
		}
		const ResourceLocation loc2(Utils::getAssetsPath() + "\\ch1.png", true);
		TexturePtr ptr2 = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(loc2, false);
		if (ptr2.clientTexture == nullptr || ptr2.clientTexture->clientTexture.resourcePointerBlock == nullptr) {
			return;
		}
		const auto tess = screenContext->getTessellator();

		tess->begin(mce::PrimitiveMode::QuadList, 4);

		auto shouldHighlight = getOps<bool>("highlightOnEntity");
		D2D1_COLOR_F color = isHoveringEnemy && shouldHighlight ? getColor("enemy") : getColor("default");

		bool isDefault = true; //!getOps<bool>("CustomCrosshair");

		tess->color(color.r, color.g, color.b, color.a);

		Vec2<float> size = Vec2<float>(17, 17);
		auto scale = getOps<float>("uiscale");
		Vec2<float> sizeScaled = PositionUtils::getCustomScreenScaledPos(size, scale);

		Vec2<float> sizeUnscaled = PositionUtils::getScreenScaledPos(size);

		auto SizeToUse = isDefault ? sizeUnscaled : sizeScaled;

		Vec2<float> pos = PositionUtils::getScaledPos(Vec2<float>((MC::windowSize.x / 2) - (SizeToUse.x / 2), (MC::windowSize.y / 2) - (SizeToUse.y / 2)));

		auto useSolidColor = getOps<bool>("solidColor");
		auto useSolidColorWhenHighlighted = getOps<bool>("solidColorWhenHighlighted");

		bool useSolid = false;

		if (isHoveringEnemy && shouldHighlight && useSolidColorWhenHighlighted || (!isHoveringEnemy && useSolidColor)) {
			useSolid = true;
		}

		mce::MaterialPtr* material = useSolid ? MaterialUtils::getUITextured() : MaterialUtils::getUICrosshair();

		if (isDefault) {
			// Pack crosshairs have textures placed whereever so this will figure it out
			IntRectangle rect = IntRectangle(pos.x, pos.y, size.x, size.y);
			ScreenRenderer::blit(screenContext, &ptr, &rect, material);
		}
		else {
			tess->vertexUV(pos.x, pos.y + (sizeScaled.y), 0.f, 0.f, 1.f);
			tess->vertexUV(pos.x + (sizeScaled.x), pos.y + (sizeScaled.y), 0.f, 1.f, 1.f);
			tess->vertexUV(pos.x + (sizeScaled.x), pos.y, 0.f, 1.f, 0.f);
			tess->vertexUV(pos.x, pos.y, 0.f, 0.f, 0.f);
			MeshHelpers::renderMeshImmediately2(screenContext, tess, material, *ptr2.clientTexture);
		}
	}
};
