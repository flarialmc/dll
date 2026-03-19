#pragma once

namespace bgfx {

	enum RendererType {
		Noop,      
		Agc,       
		Direct3D11,
		Direct3D12, 
		Direct3D12RTX,
		Gnm,        
		Metal,    
		Nvn,   
		OpenGLES, 
		OpenGL,  
		Vulkan,
		WebGPU,   
		Count
	};

	struct RendererContextI {
		virtual ~RendererContextI();
		virtual RendererType getRendererType();
	};
}