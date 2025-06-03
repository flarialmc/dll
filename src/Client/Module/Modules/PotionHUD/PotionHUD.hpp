#pragma once

#include "../Module.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "Events/Render/RenderPotionHUDEvent.hpp"
#include "Events/Render/SetupAndRenderEvent.hpp"
#include "Utils/Render/PositionUtils.hpp"

class PotionHUD : public Module {
private:
	Vec2<float> currentPos{};
	bool enabled = false;

	float testSpacing = 20;
public:

	PotionHUD();;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	void renderText();

	void onRender(RenderEvent& event);

	void onRenderPotionHUD(RenderPotionHUDEvent& event);

	void onSetupAndRender(const SetupAndRenderEvent& event) {
		if (this->isEnabled())
			if (ClientInstance::getTopScreenName() == "hud_screen") {
				Vec2<float> scaledPos = PositionUtils::getScaledPos(currentPos);
				float uiscale = getOps<float>("uiscale");

				if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
					auto ui_icon_scale = uiscale / 2.f;
					float spacing = 16 * uiscale * getOps<float>("spacing");

					float xmodifier = 0.0f;
					float ymodifier = 0.0f;

					auto muirc = event.getMuirc();
					auto effects = SDK::clientInstance->getLocalPlayer()->getMobEffects();
					for (const auto& effect : effects) {
						if (!effect.isValid()) continue;
						auto location = effect.getTextureLocation();
						auto texture = muirc->createTexture(location, false);
						auto position = Vec2<float>(scaledPos.x + xmodifier, scaledPos.y + ymodifier);
						auto size = Vec2<float>(16.0f * uiscale, 16.0f * uiscale);
						static auto uvPos = Vec2<float>(0.f, 0.f);
						static auto uvSize = Vec2<float>(1.0f, 1.0f);
						muirc->drawImage(texture, position, size, uvPos, uvSize);
						static auto color = mce::Color();
						static auto flushLayer = HashedString("ui_flush");
						muirc->flushImages(color, 1.0f, flushLayer);
						ymodifier += spacing * (getOps<bool>("bottomUp") ? -1 : 1);
					}
				}
			}
	}
};
