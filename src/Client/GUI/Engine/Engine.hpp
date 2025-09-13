#pragma once

// Split header includes for better compilation performance
#include "EngineCore.hpp"
#include "EngineDirectX.hpp" 
#include "EngineImGui.hpp"

// Template implementation moved here to avoid repeated instantiation
namespace FlarialGUI {
	template<typename T>
	inline std::string cached_to_string(const T& value) {
		auto& cache = detail::getCache<T>();
		{
			std::lock_guard<std::mutex> lock(detail::getMutex<T>());
			auto it = cache.find(value);
			if (it != cache.end()) {
				return it->second;
			}
		}
		std::string result = std::to_string(value);
		{
			std::lock_guard<std::mutex> lock(detail::getMutex<T>());
			cache[value] = result;
		}
		return result;
	}

	template<typename T>
	void lerp(T& a, const T& b, float t);
}
