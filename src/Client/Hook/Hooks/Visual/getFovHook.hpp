#pragma once

#include "../Hook.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../Module/Manager.hpp"
#include "../../../Module/Modules/Zoom/Zoom.hpp"
#include "../../../Module/Modules/Zoom/ZoomListener.hpp"
#include "../../../Module/Manager.hpp"
#include "../Game/RaknetTick.hpp"
#include "../../../Client.hpp"

class getFovHook : public Hook
{
private:

	static inline bool notified150FOV = false;

	static inline float currentZoomVal = 0.0f;

    static float getFovCallback(void* a1, float f, void* a3, void* a4) {

		float fov = func_original(a1, f, a3, a4);

		auto fovchanger = ModuleManager::getModule("FOV Changer");
		auto zom = reinterpret_cast<Zoom*>(ModuleManager::getModule("Zoom"));
		auto upsidedown = ModuleManager::getModule("Upside Down");

		if (upsidedown != nullptr && upsidedown->settings.getSettingByName<bool>("enabled")->value) {
			fov = 360 - upsidedown->settings.getSettingByName<float>("fovvalue")->value;
		}
		else if (fovchanger != nullptr && fovchanger->settings.getSettingByName<bool>("enabled")->value) {
			bool inserver;

			if (RaknetTickHook::towriteip.find("none") != std::string::npos) inserver = false;
			else if (!RaknetTickHook::towriteip.empty()) inserver = true;
			else inserver = false;

			if (inserver) {
				if (fovchanger->settings.getSettingByName<float>("fovvalue")->value > 150) {
					if (!notified150FOV) {
						FlarialGUI::Notify("FOV Changer has been limmited to 150 on servers.");
						notified150FOV = true;
					}
					fovchanger->settings.getSettingByName<float>("fovvalue")->value = 150;
				}
				// fovchanger->settings.getSettingByName<bool>("enabled")->value = false;
			}
			
			fov = fovchanger->settings.getSettingByName<float>("fovvalue")->value;
		}

		if (zom != nullptr && fov != 70) {
			float animspeed = zom->settings.getSettingByName<float>("anim")->value;
			if (zom->settings.getSettingByName<bool>("disableanim")->value) {
				if (zom->settings.getSettingByName<bool>("enabled")->value) {
					if (fov > 180) currentZoomVal = fov + ZoomListener::zoomValue;
					else currentZoomVal = ZoomListener::zoomValue;
				}
				else currentZoomVal = fov;
			}
			else {
				if (zom->settings.getSettingByName<bool>("enabled")->value) {
					if (fov > 180) currentZoomVal = std::lerp(currentZoomVal, fov + ZoomListener::zoomValue, animspeed * FlarialGUI::frameFactor);
					else currentZoomVal = std::lerp(currentZoomVal, ZoomListener::zoomValue, animspeed * FlarialGUI::frameFactor);
				}
				else currentZoomVal = std::lerp(currentZoomVal, fov, zom->settings.getSettingByName<float>("anim")->value * FlarialGUI::frameFactor);
			}
			fov = currentZoomVal;
		}

        currentFov = fov;

		return fov;
	}

public:
	typedef float(__thiscall* getFovOriginal)(void*, float, void*, void*);
	static inline getFovOriginal func_original = nullptr;

	getFovHook() : Hook("getFovHook", "48 8B ? 48 89 ? ? 48 89 ? ? 57 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 41 0F") {}

	void enableHook() override {
		this->autoHook(getFovCallback, (void**)&func_original);
	}

    static inline float currentFov = 0.0f;
};