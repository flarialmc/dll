#pragma once
#include "../../../Utils/Memory/Memory.hpp"
#include "../../../Utils/Utils.hpp"

class MaterialPtr;
class LevelRendererPlayer
{
public:
	BUILD_ACCESS(this, MaterialPtr*, shadowBack, 0x238);
	BUILD_ACCESS(this, MaterialPtr*, shadowFront, 0x248);
	BUILD_ACCESS(this, MaterialPtr*, shadowOverlay, 0x258);
	BUILD_ACCESS(this, MaterialPtr*, stars, 0x268);
	BUILD_ACCESS(this, MaterialPtr*, skyplane, 0x278);
	BUILD_ACCESS(this, MaterialPtr*, sunMoon, 0x288);
	BUILD_ACCESS(this, MaterialPtr*, endSky, 0x298);
	BUILD_ACCESS(this, MaterialPtr*, clouds, 0x2A8);
	BUILD_ACCESS(this, MaterialPtr*, wireframe, 0x2B8);
	BUILD_ACCESS(this, MaterialPtr*, cubemap, 0x2C8);
	BUILD_ACCESS(this, Vec3<float>, cameraPos, 0x514);
};

class LevelRender {
public:
	LevelRendererPlayer* getLevelRendererPlayer()
	{
		return *reinterpret_cast<LevelRendererPlayer**>((uintptr_t)(this) + 0x2F8);
	};

	Vec3<float> getOrigin()
	{
		return getLevelRendererPlayer()->cameraPos;
	};
};