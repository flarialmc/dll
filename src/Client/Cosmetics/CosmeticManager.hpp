#pragma once

#include <Utils/Render/DrawUtil3D.hpp>
#include "cosmetic.hpp"

class CosmeticMnager {
	std::map<std::string, std::unique_ptr<cosmetic>> Comsetics;
	template<typename T>
	void AddCosmetic() {
		auto Cosmetic = std::make_unique<T>();

		Comsetics[Cosmetic->Name] = std::move(Cosmetic);
	}

public:

};