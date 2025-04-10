#pragma once
#include <Utils/Utils.hpp>
#include <Utils/Render/DrawUtil3D.hpp>
#include <Utils/Render/MaterialUtils.hpp>

class Cosmetic {
public:

	std::string name;

	virtual void render(Vec3<float> pos, MCDrawUtil3D& dc, float headYaw, float headPitch) {}

	Cosmetic(std::string name) {
		this->name = name;
	}
};