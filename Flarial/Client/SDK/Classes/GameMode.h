#pragma once
#include "../../Utils/Utils.h"
#include "../../Mem/Mem.h"

class GameMode {
public:
	class Player* player;
private:
	virtual ~GameMode(void);
public:
	virtual auto startDestroyBlock(Vec3<int>*, unsigned __int8, bool*) -> bool;
	virtual auto destroyBlock(Vec3<int>*, unsigned __int8) -> bool;
	virtual auto continueDestroyBlock(Vec3<int>*, unsigned __int8, Vec3<float>*, bool*) -> bool;
	virtual auto stopDestroyBlock(Vec3<int>*) -> void;
	virtual auto startBuildBlock(Vec3<int>*) -> void;
	virtual auto buildBlock(Vec3<int>*, unsigned __int8, bool) -> bool;
	virtual auto continueBuildBlock(Vec3<int>*, unsigned __int8) -> void;
	virtual auto stopBuildBlock(void) -> void;
	virtual auto tick(void) -> void;
	virtual auto getPickRange(enum InputMode*, bool) -> float;
	virtual auto useItem(class ItemStack*) -> bool;
	virtual auto useItemOn(class ItemStack*, Vec3<int>*, unsigned __int8, Vec3<float>*, class Block*) -> bool;
	virtual auto interact(class Actor*, Vec3<float>*) -> bool;
	virtual auto attack(Actor*) -> bool;
	virtual auto releaseUsingItem(void) -> void;
	virtual auto setTrialMode(bool) -> void;
	virtual auto isInTrialMode(void) -> bool;
	virtual auto registerUpsellScreenCallback(std::function<void(bool)>) -> void;
};