#pragma once

#include "../Module.hpp"
#include "../../../../SDK/Client/Network/Packet/EntityEventPacket.hpp"

using Duration = std::chrono::duration<double>;

struct HitInfo {
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorStableTouch;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorTouch;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastActorHit;
	Vec3<float> lastActorTouchPos;
	Duration lastActorHitDelay{};
};

class HitPing : public Module {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
	Duration hitDelay = std::chrono::milliseconds(0);
	float pingReach{};

	std::map<uint64_t, std::pair<bool, HitInfo>> actorsHit;
public:
	HitPing() : Module("Hit ping", "Measures your hit delay!",
		IDR_PING_PNG, "") {
		Module::setup();
	};

	void onEnable() override {
		Listen(this, RenderEvent, &HitPing::onRender)
			Listen(this, AttackEvent, &HitPing::onAttack)
			Listen(this, PacketEvent, &HitPing::onPacketReceive)
			Listen(this, TickEvent, &HitPing::onTick)
			Module::onEnable();
	}

	void onDisable() override {
		Deafen(this, RenderEvent, &HitPing::onRender)
			Deafen(this, AttackEvent, &HitPing::onAttack)
			Deafen(this, PacketEvent, &HitPing::onPacketReceive)
			Deafen(this, TickEvent, &HitPing::onTick)
			Module::onDisable();
	}

	void defaultConfig() override {
		Module::defaultConfig();
		if (settings.getSettingByName<std::string>("text") == nullptr) settings.addSetting("text", (std::string)"{value} ms");
		if (settings.getSettingByName<float>("textscale") == nullptr) settings.addSetting("textscale", 0.80f);
	}

	void settingsRender(float settingsOffset) override {
		float x = Constraints::PercentageConstraint(0.019, "left");
		float y = Constraints::PercentageConstraint(0.10, "top");

		const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);


		FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
		FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
			Constraints::RelativeConstraint(1.0, "width"),
			Constraints::RelativeConstraint(0.88f, "height"));


		this->addHeader("Main");
		this->addSlider("UI Scale", "", this->settings.getSettingByName<float>("uiscale")->value, 2.0f);
		this->addToggle("Border", "", this->settings.getSettingByName<bool>("border")->value);
		this->addToggle("Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", this->settings.getSettingByName<bool>(
			"BlurEffect")->value);
		this->addConditionalSlider(this->settings.getSettingByName<bool>(
			"border")->value, "Border Thickness", "", this->settings.getSettingByName<float>("borderWidth")->value, 4.f);
		this->addSlider("Rounding", "Rounding of the rectangle", this->settings.getSettingByName<float>("rounding")->value);

		this->extraPadding();

		this->addHeader("Text");
		this->addTextBox("Format", "", settings.getSettingByName<std::string>("text")->value);
		this->addSlider("Text Scale", "", this->settings.getSettingByName<float>("textscale")->value, 2.0f);
		this->addDropdown("Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, this->settings.getSettingByName<std::string>("textalignment")->value);
		this->addColorPicker("Color", "Text Color", settings.getSettingByName<std::string>("textColor")->value,
			settings.getSettingByName<float>("textOpacity")->value,
			settings.getSettingByName<bool>("textRGB")->value);
		this->addToggle("Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow")->value);
		this->addColorPicker("Shadow Color", "Text Shadow Color", settings.getSettingByName<std::string>("textShadowCol")->value,
			settings.getSettingByName<float>("textShadowOpacity")->value,
			settings.getSettingByName<bool>("textShadowRGB")->value);
		this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("textShadowOffset")->value, 0.02f, 0.001f);

		this->extraPadding();

		this->addHeader("Colors");
		this->addColorPicker("Background Color", "", settings.getSettingByName<std::string>("bgColor")->value,
			settings.getSettingByName<float>("bgOpacity")->value,
			settings.getSettingByName<bool>("bgRGB")->value);
		this->addToggle("Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow")->value);
		this->addColorPicker("Shadow Color", "Background Shadow Color", settings.getSettingByName<std::string>("rectShadowCol")->value,
			settings.getSettingByName<float>("rectShadowOpacity")->value,
			settings.getSettingByName<bool>("rectShadowRGB")->value);
		this->addSlider("Shadow Offset", "How far the shadow will be.", this->settings.getSettingByName<float>("rectShadowOffset")->value, 0.02f, 0.001f);

		this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value,
			settings.getSettingByName<float>("borderOpacity")->value,
			settings.getSettingByName<bool>("borderRGB")->value);        this->addColorPicker("Border Color", "", settings.getSettingByName<std::string>("borderColor")->value, settings.getSettingByName<float>("borderOpacity")->value, settings.getSettingByName<bool>("borderRGB")->value);

		this->extraPadding();

		this->addHeader("Misc Customizations");

		this->addToggle("Reverse Padding X", "For Text Position", this->settings.getSettingByName<bool>(
			"reversepaddingx")->value);

		this->addToggle("Reverse Padding Y", "For Text Position", this->settings.getSettingByName<bool>(
			"reversepaddingy")->value);

		this->addSlider("Padding X", "For Text Position", this->settings.getSettingByName<float>("padx")->value);
		this->addSlider("Padding Y", "For Text Position", this->settings.getSettingByName<float>("pady")->value);

		this->addSlider("Rectangle Width", "", this->settings.getSettingByName<float>("rectwidth")->value, 2.f, 0.001f);
		this->addSlider("Rectangle Height", "", this->settings.getSettingByName<float>("rectheight")->value, 2.f, 0.001f);

		this->addToggle("Responsive Rectangle", "Rectangle resizes with text", this->settings.getSettingByName<bool>(
			"responsivewidth")->value);

		this->addSlider("Rotation", "see for yourself!", this->settings.getSettingByName<float>("rotation")->value, 360.f, 0, false);

		FlarialGUI::UnsetScrollView();
		this->resetPadding();
	}

	void onRender(RenderEvent& event) {
		if (this->isEnabled()) {
			auto delayStr = FlarialGUI::cached_to_string(pingReach) + "\n" + FlarialGUI::cached_to_string((int)round(hitDelay.count() * 1000));

			this->normalRender(21, delayStr);
		}
	}

	void onAttack(AttackEvent& event) { // only calculate ping on first hit
		if (!event.getActor()->isValid()) return;
		if (!event.getActor()->isValidAABB()) return;
		ClearOldHits();
		auto playerPos = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent()->Pos;
		auto entity = event.getActor();
		auto hitActors = &actorsHit;
		auto id = entity->getRuntimeIDComponent()->runtimeID;

		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

		auto it = hitActors->find(id);
		if (it != hitActors->end()) {
			if (start - (*hitActors)[id].second.lastActorTouch <= std::chrono::milliseconds(480)) {
				// hitting too fast
				(*hitActors)[id].second.lastActorTouch = start;
				return;
			}
			else if (!(*hitActors)[id].first) {
				(*hitActors)[id].first = true;
				(*hitActors)[id].second.lastActorTouch = start;
				(*hitActors)[id].second.lastActorStableTouch = start;
				(*hitActors)[id].second.lastActorTouchPos = playerPos;
			}
		}
		else {
			(*hitActors)[id].first = true;
			(*hitActors)[id].second.lastActorTouch = start;
			(*hitActors)[id].second.lastActorStableTouch = start;
			(*hitActors)[id].second.lastActorTouchPos = playerPos;
		}
	}

	void onPacketReceive(PacketEvent& event) {
		if (event.getPacket()->getId() == MinecraftPacketIds::ActorEvent) {
			auto packet = (EntityEventPacket*)event.getPacket();
			if (packet->EventID == ActorEvent::Hurt) {
				auto id = packet->RuntimeID;
				auto actorHitInfo = actorsHit.find(id);
				if (actorHitInfo != actorsHit.end()) {
					auto now = std::chrono::high_resolution_clock::now();
					auto start = actorHitInfo->second.second.lastActorStableTouch;

					if (!actorHitInfo->second.first) {
						return; // hit registered
					}

					actorHitInfo->second.first = false;

					if (start + std::chrono::milliseconds(500) > now) {
						std::chrono::duration<double> time = now - start;

						if (time.count() >= 0.0001) {
							hitDelay = time;
							auto actors = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList();
							auto actor = std::find_if(actors.begin(), actors.end(), [&](auto& a) {
								return a->getRuntimeIDComponent()->runtimeID == id;
								});
							if (actor != actors.end()) {

								auto actorPtr = (*actor);
								auto actorHitBox = actorPtr->getAABBShapeComponent()->aabb.expandedXZ(0.1);

								auto upper = actorHitBox.upper;
								auto lower = actorHitBox.lower;

								auto posAtTimeOfHit = actorHitInfo->second.second.lastActorTouchPos;

								auto closestPoint = Vec3<float>{ std::clamp(posAtTimeOfHit.x, lower.x, upper.x),
																 std::clamp(posAtTimeOfHit.y, lower.y, upper.y),
																 std::clamp(posAtTimeOfHit.z, lower.z, upper.z) };

								pingReach = actorHitInfo->second.second.lastActorTouchPos.dist(closestPoint);
								last_hit = now;
								actorHitInfo->second.second.lastActorHit = now;
								actorHitInfo->second.second.lastActorHitDelay = time;
							}
						}
					}
				}
			}
		}
	}

	void onTick(TickEvent& event) {
		std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
		if (duration.count() >= 15) {
			pingReach = 0;
			hitDelay = std::chrono::milliseconds(0);
		};
	}

	void ClearOldHits() {
		auto now = std::chrono::steady_clock::now();
		for (auto it = actorsHit.begin(); it != actorsHit.end();) {
			if (now - it->second.second.lastActorTouch > std::chrono::milliseconds(3000)) {
				it = actorsHit.erase(it);
			}
			else {
				++it;
			}
		}
	}
};

