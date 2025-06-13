#pragma once

class Face {
	Vec3<float> Vertex1;
	Vec3<float> Vertex2;
	Vec3<float> Vertex3;
	Vec3<float> Vertex4;
};

class Cube {

};

class cosmetic {

public:
	std::string Name;

	virtual void Render(Actor* player);

private:

};