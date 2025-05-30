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
		if (settings.getSettingByName<float>("uiscale") == nullptr) settings.addSetting("uiscale", 1.f);
		Module::defaultConfig("core");

		if (settings.getSettingByName<bool>("CustomCrosshair") == nullptr) settings.addSetting("CustomCrosshair", false);

		if (settings.getSettingByName<bool>("highlightOnEntity") == nullptr) settings.addSetting("highlightOnEntity", false);
		if (settings.getSettingByName<bool>("solidColorWhenHighlighted") == nullptr) settings.addSetting("solidColorWhenHighlighted", true);
		if (settings.getSettingByName<bool>("solidColor") == nullptr) settings.addSetting("solidColor", false);
		if (settings.getSettingByName<bool>("renderInThirdPerson") == nullptr) settings.addSetting("renderInThirdPerson", false);

		if (settings.getSettingByName<std::string>("defaultColor") == nullptr) settings.addSetting("defaultColor", (std::string)"fafafa");
		if (settings.getSettingByName<bool>("defaultColorRGB") == nullptr) settings.addSetting("defaultColorRGB", false);
		if (settings.getSettingByName<float>("defaultOpacity") == nullptr) settings.addSetting("defaultOpacity", 0.55f);

		if (settings.getSettingByName<std::string>("enemyColor") == nullptr) settings.addSetting("enemyColor", (std::string)"FF0000");
		if (settings.getSettingByName<bool>("enemyColorRGB") == nullptr) settings.addSetting("enemyColorRGB", false);
		if (settings.getSettingByName<float>("enemyOpacity") == nullptr) settings.addSetting("enemyOpacity", 1.f);
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
		addToggle("Solid Color", "Make crosshair a solid color / more visible", settings.getSettingByName<bool>("solidColor")->value);
		addToggle("Render in Third Person", "Weather or not to render in third person", settings.getSettingByName<bool>("renderInThirdPerson")->value);
		addToggle("Highlight on Entity", "Highlight when enemy in reach", settings.getSettingByName<bool>("highlightOnEntity")->value);
		addConditionalToggle(settings.getSettingByName<bool>("highlightOnEntity")->value, "Solid Color When Highlighted", "Use solid color when highlighted", settings.getSettingByName<bool>("solidColorWhenHighlighted")->value);
		extraPadding();

		// addHeader("Misc");
		// addSlider("UI Scale", "The size of the Crosshair (only for custom)", settings.getSettingByName<float>("uiscale")->value, 10.f, 0.f, true);

		extraPadding();

		addHeader("Colors");
		addColorPicker("Default Color", "When the enemy is not in view.", settings.getSettingByName<std::string>("defaultColor")->value, settings.getSettingByName<float>("defaultOpacity")->value, settings.getSettingByName<bool>("defaultColorRGB")->value);
		addConditionalColorPicker(settings.getSettingByName<bool>("highlightOnEntity")->value, "Enemy Color", "When the enemy is in view.",
			settings.getSettingByName<std::string>("enemyColor")->value,
			settings.getSettingByName<float>("enemyOpacity")->value,
			settings.getSettingByName<bool>("enemyColorRGB")->value);
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

		auto renderInThirdPerson = settings.getSettingByName<bool>("renderInThirdPerson")->value;
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

		D2D1_COLOR_F enemyColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("enemyColor")->value);
		enemyColor.a = settings.getSettingByName<float>("enemyOpacity")->value;

		D2D1_COLOR_F defaultColor = FlarialGUI::HexToColorF(settings.getSettingByName<std::string>("defaultColor")->value);
		defaultColor.a = settings.getSettingByName<float>("defaultOpacity")->value;

		auto shouldHighlight = settings.getSettingByName<bool>("highlightOnEntity")->value;
		D2D1_COLOR_F color = isHoveringEnemy && shouldHighlight ? enemyColor : defaultColor;

		bool isDefault = true; //!settings.getSettingByName<bool>("CustomCrosshair")->value;

		tess->color(color.r, color.g, color.b, color.a);

		Vec2<float> size = Vec2<float>(17, 17);
		auto scale = settings.getSettingByName<float>("uiscale")->value;
		Vec2<float> sizeScaled = PositionUtils::getCustomScreenScaledPos(size, scale);

		Vec2<float> sizeUnscaled = PositionUtils::getScreenScaledPos(size);

		auto SizeToUse = isDefault ? sizeUnscaled : sizeScaled;

		Vec2<float> pos = PositionUtils::getScaledPos(Vec2<float>((MC::windowSize.x / 2) - (SizeToUse.x / 2), (MC::windowSize.y / 2) - (SizeToUse.y / 2)));

		auto useSolidColor = settings.getSettingByName<bool>("solidColor")->value;
		auto useSolidColorWhenHighlighted = settings.getSettingByName<bool>("solidColorWhenHighlighted")->value;

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
