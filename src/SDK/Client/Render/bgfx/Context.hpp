#pragma once

#include "RendererContextI.hpp"

namespace bgfx {
	struct Context {

		RendererContextI* getRendererContext();

		// Only use this if you are 100% sure of the type

		template<typename T> 
		T* getRendererContextAs();
	};

	template<typename T>
	T* Context::getRendererContextAs() {
		return reinterpret_cast<T*>(this->getRendererContext());
	}
}