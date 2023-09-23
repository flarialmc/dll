#pragma once

#include "../Actor/LocalPlayer.hpp"
#include "MinecraftGame.hpp"
#include "../Block/BlockSource.hpp"
#include "../Render/GuiData.hpp"
#include "LevelRender.hpp"
#include <cstdint>
#include "../../../Utils/Memory/Memory.hpp"
#include "../Network/Packet/LoopbackPacketSender.hpp"
#include "Minecraft.hpp"
#include "../Render/GLMatrix.hpp"


class ClientInstance {
public:

	GLMatrix Matrix1;

    Minecraft* getMinecraft()
    {
        return *reinterpret_cast<Minecraft**>((uintptr_t)this + 0xD0);
    }

    BUILD_ACCESS(this, MinecraftGame*, mcgame, 0x0C8);
    BUILD_ACCESS(this, GuiData*, guiData, 0x560);
    //BUILD_ACCESS(this, Vec2<float>, mousePos, 0x498);

    LocalPlayer* getLocalPlayer();
    BlockSource* getBlockSource();
    void grabMouse();
    void releaseMouse();
    void refocusMouse();
    std::string getTopScreenName();

	GLMatrix* getMatrixCorrection()
	{
		GLMatrix toReturn = GLMatrix();

		for (int i = 0; i < 4; i++) {
			toReturn.matrix[i * 4 + 0] = Matrix1.matrix[0 + i];
			toReturn.matrix[i * 4 + 1] = Matrix1.matrix[4 + i];
			toReturn.matrix[i * 4 + 2] = Matrix1.matrix[8 + i];
			toReturn.matrix[i * 4 + 3] = Matrix1.matrix[12 + i];
		}

		return &toReturn;
	};

	__forceinline float transformx(const Vec3<float>& p)
	{
		auto matrix = getMatrixCorrection()->matrix;
		return p.x * matrix[0] + p.y * matrix[4] + p.z * matrix[8] + matrix[12];
	}

	__forceinline float transformy(const Vec3<float>& p)
	{
		auto matrix = getMatrixCorrection()->matrix;
		return p.x * matrix[1] + p.y * matrix[5] + p.z * matrix[9] + matrix[13];
	}

	__forceinline float transformz(const Vec3<float>& p)
	{
		auto matrix = getMatrixCorrection()->matrix;
		return p.x * matrix[2] + p.y * matrix[6] + p.z * matrix[10] + matrix[14];
	}

	LevelRender* getLevelRender()
	{
		uintptr_t address = reinterpret_cast<uintptr_t>(this);
		return *reinterpret_cast<LevelRender**>(address + 0xE0);
	}

	float* getFovX()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x6A0);
	};

	float* getFovY()
	{
		return reinterpret_cast<float*>((uintptr_t)(this) + 0x6B4);
	};

	Vec2<float> getFov()
	{
		return Vec2(*getFovX(), *getFovY());
	};

    LoopbackPacketSender* getPacketSender()
    {
        return *reinterpret_cast<LoopbackPacketSender**>((uintptr_t)this + 0xF0);
    }

	inline bool WorldToScreen(Vec3<float> pos, Vec2<float>& screen)
	{ // pos = pos 2 w2s, screen = output screen coords
		Vec2<float> displaySize = guiData->getScreenSize();
		LevelRender* lr = getLevelRender();
		Vec3<float> origin = lr->getOrigin();
		Vec2<float> fov = getFov();

		pos.x -= origin.x;
		pos.y -= origin.y;
		pos.z -= origin.z;

		float x = transformx(pos);
		float y = transformy(pos);
		float z = transformz(pos);

		if (z > 0) return false;

		float mX = (float)displaySize.x / 2.0F;
		float mY = (float)displaySize.y / 2.0F;

		screen.x = mX + (mX * x / -z * fov.x);
		screen.y = mY - (mY * y / -z * fov.y);

		return true;
	}
};
