
#pragma once

#include "../Event.hpp"
#include "../Cancellable.hpp"
#include <d2d1_3.h>
#include <d3d12.h>
#include <d3d11.h>
#include <d3d11on12.h>

class RenderEvent : public Event {
public:
	ID3D11RenderTargetView* RTV;
};