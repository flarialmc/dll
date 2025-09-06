#pragma once

#include "AvgPixelMotionBlurHelper.hpp"
#include "RealMotionBlurHelper.hpp"
#include "../Module.hpp"
#include "Events/Render/RenderUnderUIEvent.hpp"
#include "Events/Render/RenderEvent.hpp"
#include "../../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"


class MotionBlur : public Module {
public:
	static inline bool initted = false;

	MotionBlur();;

	bool once = false;

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	static inline std::vector<winrt::com_ptr<ID3D11ShaderResourceView>> previousFrames;

	void onRender(RenderUnderUIEvent& event);

	void onRenderNormal(RenderEvent& event);


	void ImageWithOpacity(const winrt::com_ptr<ID3D11ShaderResourceView>& srv, ImVec2 size, float opacity);

	static winrt::com_ptr<ID3D11ShaderResourceView> BackbufferToSRVExtraMode();


	static winrt::com_ptr<ID3D11ShaderResourceView> BackbufferToSRV();
};
