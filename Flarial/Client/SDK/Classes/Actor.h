#pragma once
#include "Level.h"
#include "GameMode.h"
#include "PlayerInventory.h"

enum GameType { Survival = 0, Creative = 1, Adventure = 2, Default = 5 };

template<typename A, typename B>
class AutomaticID;

class Actor {
public:
	auto getRuntimeId(void) -> uint64_t;
public:
	AS_FIELD(uint64_t, runtimeId, getRuntimeId);
public:
	BUILD_ACCESS(*(uintptr_t**)((uintptr_t)(this) + 0x250), AABB<float>, boundingBox, 0x0);
public:
	auto setPos(Vec3<float>) -> void;
public:
	auto getLevel(void) -> Level*;
public:
	auto getHealth(void) -> int;
public:
	auto isHostile(void) -> bool;
	auto isPassive(void) -> bool;
private:
	virtual ~Actor(void);
public:
	virtual bool getStatusFlag(enum ActorFlags);
	virtual void setStatusFlag(enum ActorFlags, bool);
	virtual bool hasComponent(class HanshedString*);
	virtual Actor* getLastHurtByMob();
	virtual void setLastHurtByMob(Actor*);
	virtual Player* getLastHurtByPlayer();
	virtual void setLastHurtByPlayer(class Player*);
	virtual Actor* getLastHurtMob();
	virtual void setLastHurtMob(Actor*);
	virtual void outOfWorld();
	virtual void reloadHardcoded(enum ActorInitializationMethod, class VariantParameterList const&);
	virtual void reloadHardcodedClient(enum ActorInitializationMethod, class VariantParameterList const&);
	virtual void initializeComponents(enum ActorInitializationMethod, class VariantParameterList const&);
	virtual void reloadComponents(enum ActorInitializationMethod, class VariantParameterList const&);
	virtual auto _serverInitItemStackIds(void) -> void;
	virtual auto _doInitialMove(void) -> void;
	virtual auto resetUserPos(bool) -> void;
	virtual auto getDimensionId(void) -> __int64;
	virtual auto getOwnerEntityType(void) -> __int64;
	virtual auto remove(void) -> void;
	virtual auto setPos(Vec3<float>*) -> void;
	virtual auto isRuntimePredictedMovementEnabled(void) -> bool;
	virtual auto getPredictedMovementValues(void) -> class PredictedMovementValues*;
	virtual auto getPos(void) -> Vec3<float>*;
	virtual auto getPosPrev(void) -> Vec3<float>*;
	virtual auto getPosExtrapolated(float) -> const Vec3<float>;
	virtual auto getAttachPos(enum ActorLocation, float) -> Vec3<float>;
	virtual auto getFiringPos(void) -> Vec3<float>;
	virtual auto move(class IActorMovementProxy*, Vec3<float>*) -> void;
	virtual auto move(Vec3<float>*) -> void;
	virtual auto getInterpolatedRidingPosition(float) -> Vec3<float>;
	virtual auto getInterpolatedBodyRot(float) -> float;
	virtual auto getInterpolatedHeadRot(float) -> float;
	virtual auto getInterpolatedBodyYaw(float) -> float;
	virtual auto getYawSpeedInDegreesPerSecond(void) -> float;
	virtual auto getInterpolatedWalkAnimSpeed(float) -> float;
	virtual auto getInterpolatedRidingOffset(float, int) -> Vec3<float>;
	virtual auto resetInterpolated(void) -> void;
	virtual auto updateEntityInside(void) -> void;
	virtual auto updateEntityInside(AABB<float>*) -> void;
	virtual auto isFireImmune(void) -> bool;
	virtual auto breaksFallingBlocks(void) -> bool;
	virtual auto blockedByShield(class ActorDamageSource*, Actor*) -> void;
	virtual auto canDisableShield(void) -> bool;
	virtual auto teleportTo(Vec3<float>*, bool, int, int, bool) -> void;
	virtual auto tryTeleportTo(Vec3<float>*, bool, int, int, bool) -> bool;
	virtual auto chorusFruitTeleport(Vec3<float>*) -> void;
	virtual auto lerpMotion(Vec3<float>*) -> void;
	virtual auto tryCreateAddActorPacket(void) -> class Packet*;
	virtual auto normalTick(void) -> void;
	virtual auto baseTick(void) -> void;
	virtual auto passengerTick(void) -> void;
	virtual auto positionPassenger(Actor*, float) -> void;
	virtual auto startRiding(Actor*) -> bool;
	virtual auto addPassenger(Actor*) -> void;
	virtual auto flagPassengerToRemove(Actor*) -> void;
	virtual auto getExitTip(std::string const&, enum InputMode) -> void;
	virtual auto getEntityLocNameString(void) -> std::string const&;
	virtual auto intersects(Vec3<float>*, Vec3<float>*) -> bool;
	virtual auto isInWall(void) -> bool;
	virtual auto isInvisible(void) -> bool;
	virtual auto canShowNameTag(void) -> bool;
	virtual auto canExistInPeaceful(void) -> bool;
	virtual auto setNameTagVisible(bool) -> void;
	virtual auto getNameTag(void) -> std::string const&;
	virtual auto getNameTagAsHash(void) -> class HashedString*;
	virtual auto getFormattedNameTag(void) -> std::string const&;
	virtual auto filterFormattedNameTag(class UIProfanityContext*) -> void;
	virtual auto setNameTag(std::string const&) -> void;
	virtual auto getAlwaysShowNameTag(void) -> bool;
	virtual auto setScoreTag(std::string const&) -> void;
	virtual auto getScoreTag(void) -> std::string const&;
	virtual auto isInWater(void) -> bool;
	virtual auto hasEnteredWater(void) -> bool;
	virtual auto isInLava(class IConstBlockSource*) -> bool;
	virtual auto isUnderLiquid(enum MaterialType) -> bool;
	virtual auto isOverWater(void) -> bool;
	virtual auto setBlockMovementSlowdownMultiplier(class BlockLegacy*, Vec3<float>*) -> void;
	virtual auto resetBlockMovementSlowdownMultiplier(void) -> void;
	virtual auto getCameraOffset(void) -> float;
	virtual auto getShadowHeightOffs(void) -> float;
	virtual auto getShadowRadius(void) -> float;
	virtual auto getHeadLookVector(float) -> Vec3<float>;
	virtual auto canSeeInvisible(void) -> bool; /* Spectator, function detour see spectators? */
	virtual auto canSee(Vec3<float>*) -> bool;
	virtual auto canSee(Actor*) -> bool;
	virtual auto canInteractWithOtherEntitiesInGame(void) -> bool;
	virtual auto isSkyLit(float) -> bool;
	virtual auto getBrightness(float) -> float;
	virtual auto interactPreventDefault(void) -> void;
	virtual auto playerTouch(class Player*) -> void;
	virtual auto onAboveBubbleColumn(bool) -> void;
	virtual auto onInsideBubbleColumn(bool) -> void;
	virtual auto isImmobile(void) -> bool;
	virtual auto isSilent(void) -> bool;
	virtual auto _doSprintParticleEffect(void) -> void;
	virtual auto isPickable(void) -> bool;
	virtual auto isFishable(void) -> bool;
	virtual auto isSleeping(void) -> bool;
	virtual auto setSleeping(bool) -> void;
	virtual auto isShootable(void) -> bool;
	virtual auto setSneaking(bool) -> void;
	virtual auto isBlocking(void) -> bool;
	virtual auto isDamageBlocked(class ActorDamageSource*) -> bool;
	virtual auto isAlive(void) -> bool;
	virtual auto isOnFire(void) -> bool;
	virtual auto isOnHotBlock(void) -> bool;
	virtual auto isCreativeModeAllowed(void) -> bool;
	virtual auto isSurfaceMob(void) -> bool;
	virtual auto isTargetable(void) -> bool;
	virtual auto isLocalPlayer(void) -> bool;
	virtual auto isRemotePlayer(void) -> bool;
	virtual auto isPlayer(void) -> bool;
	virtual auto isAffectedByWaterBottle(void) -> bool;
	virtual auto canAttack(Actor*, bool) -> bool;
	virtual auto setTarget(Actor*) -> bool;
	virtual auto isValidTarget(Actor*) -> bool;
	virtual auto attack(Actor*, enum ActorDamageCause*) -> bool;
	virtual auto performRangedAttack(Actor*, float) -> void;
	virtual auto getEquipmentCount(void) -> __int64;
	virtual auto setOwner(uint64_t) -> void;
	virtual auto setSitting(bool) -> void;
	virtual auto onTame(void) -> void;
	virtual auto onFailedTame(void) -> void;
	virtual auto getInventorySize(void) -> __int64;
	virtual auto getEquipSlots(void) -> __int64;
	virtual auto getChestSlots(void) -> __int64;
	virtual auto setStanding(bool) -> void;
	virtual auto canPowerJump(void) -> bool;
	virtual auto setCanPowerJump(bool) -> void;
	virtual auto isJumping(void) -> bool;
	virtual auto isEnchanted(void) -> bool;
	virtual auto vehicleLanded(Vec3<float>&, Vec3<float>&) -> void;
	virtual auto shouldRender(void) -> bool;
	virtual auto playAmbientSound(void) -> void;
	virtual auto getAmbientSound(void) -> enum LevelSoundEvent;
	virtual auto isInvulnerableTo(class ActorDamageSource*) -> bool;
	virtual auto getBlockDamageCause(class Block&) -> enum ActorDamageCause;
	virtual auto animateHurt(void) -> void;
	virtual auto doFireHurt(int) -> void;
	virtual auto onLightningHit(void) -> void;
	virtual auto onBounceStarted(Vec3<int>&, class Block&) -> void;
	virtual auto feed(int) -> void;
	virtual auto handleEntityEvent(enum ActorEvent, int) -> void;
	virtual auto getPickRadius(void) -> float;
	virtual auto getActorRendererId(void) -> class HashedStruct*;
	virtual auto spawnAtLocation(class ItemStack*, float) -> class ItemActor*;
	virtual auto spawnAtLocation(int, int, float) -> class ItemActor*;
	virtual auto spawnAtLocation(int, int) -> class ItemActor*;
	virtual auto despawn(void) -> void;
	virtual auto killed(Actor*) -> void;
	virtual auto awardKillScore(Actor*, int) -> void;
	virtual auto setArmor(enum ArmorSlot, class ItemStack const&) -> void;
	virtual auto getArmor(enum ArmorSlot) -> void;
	virtual auto getAllArmor(void) -> std::vector<class ItemStack*>;
	virtual auto getArmorMaterialTypeInSlot(enum ArmorSlot) -> enum ArmorMaterialType;
	virtual auto getArmorMaterialTextureTypeInSlot(enum ArmorSlot) -> enum ArmorTextureType;
	virtual auto getArmorColorInSlot(enum ArmorSlot, int) -> float;
	virtual auto getEquippedSlot(enum EquipmentSlot) -> class ItemStack const&;
	virtual auto setEquippedSlot(enum EquipmentSlot, class ItemStack const&) -> void;
	virtual auto setCarriedItem(class ItemStack*) -> void;
	virtual auto getCarriedItem(void) -> class ItemStack*;
	virtual auto setOffhandSlot(class ItemStack*) -> void;
	virtual auto getEquippedTotem(void) -> class ItemStack*;
	virtual auto consumeTotem(void) -> void;
	virtual auto save(class CompoundTag*) -> bool;
	virtual auto saveWithoutId(class CompoundTag*) -> void;
	virtual auto load(class CompoundTag*, class DataLoadHelper*) -> bool;
	virtual auto loadLinks(class CompoundTag const&, class std::vector<class ActorLink, class std::allocator<class ActorLink>>&, class DataLoadHelper&) -> void;
	virtual auto getEntityTypeId(void) -> uint8_t;
	virtual auto queryEntityRenderer(void) -> void;
	virtual auto getSourceUniqueID(void) -> uint64_t;
	virtual auto thawFreezeEffect(void) -> void;
	virtual auto canFreeze(void) -> bool;
	virtual auto isWearingLeatherArmor(void) -> bool;
	virtual auto getLiquidAABB(enum MaterialType) -> AABB<float>;
	virtual auto handleInsidePortal(Vec3<int>*) -> void;
	virtual auto getPortalCooldown(void) -> __int64;
	virtual auto getPortalWaitTime(void) -> __int64;
	virtual auto canChangeDimensionsUsingPortal(void) -> bool;
	virtual auto changeDimension(class AutomaticID<class Dimension, int>) -> void;
	virtual auto changeDimension(Packet*) -> void;
	virtual auto getControllingPlayer(void) -> uint64_t;
	virtual auto checkFallDamage(float, bool) -> void;
	virtual auto causeFallDamage(float, float, class ActorDamageSource) -> void;
	virtual auto handleFallDistanceOnServer(float, float, bool) -> void;
	virtual auto playSynchronizedSound(enum LevelSoundEvent, Vec3<float> const&, int, bool) -> void;
	virtual auto playSynchronizedSound(enum LevelSoundEvent, Vec3<float> const&, class Block const&, bool) -> void;
	virtual auto onSynchedFlagUpdate(int, __int64, __int64) -> void;
	virtual auto onSynchedDataUpdate(int) -> void;
	virtual auto canAddPassenger(Actor*) -> bool;
	virtual auto canPickupItem(class ItemStack*) -> bool;
	virtual auto canBePulledIntoVehicle(void) -> bool;
	virtual auto inCaravan(void) -> bool;
	virtual auto isLeashableType(void) -> bool;
	virtual auto tickLeash(void) -> void;
	virtual auto sendMotionPacketIfNeeded(void) -> void;
	virtual auto canSynchronizeNewEntity(void) -> bool;
	virtual auto stopRiding(bool, bool, bool) -> void;
	virtual auto startSwimming(void) -> void;
	virtual auto stopSwimming(void) -> void;
	virtual auto buildDebugInfo(std::string&) -> __int64;
	virtual auto getCommandPermissionLevel(void) -> enum CommandPermissionLevel;
	virtual auto isClientSide(void) -> bool;
	virtual auto getMutableAttribute(class Attribute*) -> class AttributeInstance*;
	virtual auto getAttribute(class Attribute*) -> class AttributeInstance*;
	virtual auto getDeathTime(void) -> __int64;
	virtual auto heal(int) -> void;
	virtual auto isInvertedHealAndHarm(void) -> bool;
	virtual auto canBeAffected(UCHAR) -> bool;
	virtual auto canBeAffectedByArrow(class MobEffectInstance*) -> bool;
	virtual auto onEffectAdded(class MobEffectInstance*) -> void;
	virtual auto onEffectUpdated(class MobEffectInstance*) -> void;
	virtual auto onEffectRemoved(class MobEffectInstance*) -> void;
	virtual auto canObstructSpawningAndBlockPlacement(void) -> bool;
	virtual auto getAnimationComponent(void) -> class AnimationComponent*;
	virtual auto openContainerComponent(class Player*) -> void;
	virtual auto swing(void) -> void;
	virtual auto useItem(class ItemStackBase*, enum ItemUseMethod, bool) -> void;
	virtual auto hasOutputSignal(unsigned __int8) -> bool;
	virtual auto getOutputSignal(void) -> __int64;
	virtual auto getDebugText(std::vector<std::string>&) -> void;
	virtual auto getMapDecorationRotation(void) -> float;
	virtual auto getPassengerYRotation(Actor*) -> float;
	virtual auto isWorldBuilder(void) -> bool;
	virtual auto isCreative(void) -> bool;
	virtual auto isAdventure(void) -> bool;
	virtual auto isSurvival(void) -> bool;
	virtual auto isSpectator(void) -> bool;
	virtual auto isAttackableGamemode(void) -> bool;
	virtual auto add(class ItemStack*) -> bool;
	virtual auto drop(class ItemStack*, bool) -> bool;
	virtual auto getInteraction(class Player*, class ActorInteraction*, Vec3<float>*) -> bool;
	virtual auto canDestroyBlock(class Block*) -> bool;
	virtual auto setAuxValue(int) -> void;
	virtual auto setSize(float, float) -> void;
	virtual auto onOrphan(void) -> void;
	virtual auto wobble(void) -> void;
	virtual auto wasHurt(void) -> bool;
	virtual auto startSpinAttack(void) -> void;
	virtual auto stopSpinAttack(void) -> void;
	virtual auto setDamageNearbyMobs(bool) -> void;
	virtual auto renderDebugServerState(class Options*) -> void;
	virtual auto reloadLootTable(void) -> void;
	virtual auto reloadLootTable(class EquipmentTableDefinition*) -> void;
	virtual auto getDeletionDelayTimeSeconds(void) -> __int64;
	virtual auto kill(void) -> void;
	virtual auto die(class ActorDamageSource*) -> void;
	virtual auto shouldDropDeathLoot(void) -> bool;
	virtual auto shouldTick(void) -> bool;
	virtual auto applySnapshot(class EntityContext*, class EntityContext*) -> void;
	virtual auto getNextStep(float) -> float;
	virtual auto getLootTable(void) -> class LootTable*;
	virtual auto interpolatorTick(void) -> void;
	virtual auto onPush(class Player*) -> void;
	virtual auto getLastDeathPos(void) -> std::optional<Vec3<int>>;
	virtual auto getLastDeathDimension(void) -> std::optional<class AutomaticID<class Dimension, int>>;
	virtual auto hasDiedBefore(void) -> bool;
	virtual auto doWaterSplashEffect(void) -> void;
	virtual auto _shouldProvideFeedbackOnHandContainerItemSet(enum HandSlot, class ItemStack*) -> bool;
	virtual auto _shouldProvideFeedbackOnArmorSet(ArmorSlot, ItemStack*) -> void;
	virtual auto updateEntitySpecificMolangVariables(class RenderParams*) -> void;
	virtual auto shouldTryMakeStepSound(void) -> bool;
	virtual auto canMakeStepSound(void) -> bool;
	virtual auto _hurt(class ActorDamageSource*, float, bool, bool) -> bool;
	virtual auto markHurt(void) -> void;
	virtual auto _getAnimationComponent(std::shared_ptr<class AnimationComponent>&, class AnimationComponentGroupType) -> class AnimationComponent&;
	virtual auto readAdditionalSaveData(class CompoundTag*, class DataLoadHelper*) -> void;
	virtual auto addAdditionalSaveData(class CompoundTag*) -> void;
	virtual auto _playStepSound(Vec3<int>*, class Block*) -> void;
	virtual auto _playFlySound(Vec3<int>*, class Block*) -> void;
	virtual auto _makeFlySound(void) -> void;
	virtual auto checkInsideBlocks(float) -> void;
	virtual auto pushOutOfBlocks(Vec3<float>*) -> void;
	virtual auto spawnTrailBubbles(void) -> void;
	virtual auto updateInsideBlock(void) -> void;
	virtual auto _removePassenger(uint64_t, bool, bool, bool) -> void;
	virtual auto _onSizeUpdated(void) -> void;
	virtual auto _doAutoAttackOnTouch(Actor*) -> void;
};

enum ActorType {

	/* Drops */

	item = 64,
	xp_orb = 69,

	/* Blocks */

	tnt = 65,
	falling_block = 66,
	moving_block = 67,

	/* Immobile and projectiles */

	armor_stand = 61,
	xp_bottle = 68,
	eye_of_ender_signal = 70,
	ender_crystal = 71,
	fireworks_rocket = 72,
	thrown_trident = 73,
	shulker_bullet = 76,
	fishing_hook = 77,
	dragon_fireball = 79,
	arrow = 80,
	snowball = 81,
	egg = 82,
	painting = 83,
	minecart = 84,
	fireball = 85,
	splash_potion = 86,
	ender_pearl = 87,
	leash_knot = 88,
	wither_skull = 89,
	boat = 90,
	wither_skull_dangerous = 91,
	lightning_bolt = 93,
	small_fireball = 94,
	area_effect_cloud = 95,
	hopper_minecart = 96,
	tnt_minecart = 97,
	chest_minecart = 98,
	command_block_minecart = 100,
	lingering_potion = 101,
	llama_spit = 102,
	evocation_fang = 103,
	ice_bomb = 106,
	balloon = 107,
	chest_boat = 218,

	/* Hostile mobs */

	zombie = 32,
	creeper = 33,
	skeleton = 34,
	spider = 35,
	zombie_pigman = 36,
	slime = 37,
	enderman = 38,
	silverfish = 39,
	cave_spider = 40,
	ghast = 41,
	magma_cube = 42,
	blaze = 43,
	zombie_villager = 44,
	witch = 45,
	stray = 46,
	husk = 47,
	wither_skeleton = 48,
	guardian = 49,
	elder_guardian = 50,
	wither = 52,
	ender_dragon = 53,
	shulker = 54,
	endermite = 55,
	vindicator = 57,
	phantom = 58,
	ravager = 59,
	evocation_illager = 104,
	vex = 105,
	drowned = 110,
	pillager = 114,
	zombie_villager_v2 = 116,
	piglin = 123,
	hoglin = 124,
	zoglin = 126,
	piglin_brute = 127,
	warden = 131,

	/* Passive and neutral mobs */

	chicken = 10,
	cow = 11,
	pig = 12,
	sheep = 13,
	wolf = 14,
	villager = 15,
	mooshroom = 16,
	squid = 17,
	rabbit = 18,
	bat = 19,
	iron_golem = 20,
	snow_golem = 21,
	ocelot = 22,
	horse = 23,
	donkey = 24,
	mule = 25,
	skeleton_horse = 26,
	zombie_horse = 27,
	polar_bear = 28,
	llama = 29,
	parrot = 30,
	dolphin = 31,
	turtle = 74,
	cat = 75,
	pufferfish = 108,
	salmon = 109,
	tropicalfish = 111,
	cod = 112,
	panda = 113,
	villager_v2 = 115,
	wandering_trader = 118,
	fox = 121,
	bee = 122,
	strider = 125,
	goat = 128,
	frog = 132,
	tadpole = 133,
	allay = 134,

	/* Other */

	player = 63,
	shield = 117,

	/* Unused */

	elder_guardian_ghost = 120,

	/* Education Edition */

	npc = 51,
	agent = 56,
	tripod_camera = 62,
	chalkboard = 78

};