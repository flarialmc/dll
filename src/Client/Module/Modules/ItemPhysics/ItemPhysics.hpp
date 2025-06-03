#pragma once

#include <random>
#include "../Module.hpp"
#include "../../../../SDK/Client/Actor/ItemActor.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp"
#include "../../../../SDK/Client/Render/ActorRenderData.hpp"
#include "Events/EventManager.hpp"
#include "SDK/Client/Render/ItemRenderer.hpp"

class INeedADecentHookClassForMemory {
public:
	void* pointer = nullptr;
	void* trampoline = nullptr;
	bool valid = false;

	INeedADecentHookClassForMemory(void* function, void* hook);

	~INeedADecentHookClassForMemory();

	void enable() const;

	void disable() const;
};

static char data[0x5], data2[0x5];

std::unique_ptr<INeedADecentHookClassForMemory> ItemRenderer_renderHook, glm_rotateHook;

class ItemPhysics : public Module {
private:
	uint32_t origPosRel = 0;
	float* newPosRel = nullptr;
	bool patched = false;

	std::unordered_map<Actor*, std::tuple<float, Vec3<float>, Vec3<int>>> actorData;
	ActorRenderData* renderData = nullptr;
public:
	ItemPhysics() : Module("Item Physics", "Changes rotation behavior of dropped items", IDR_ITEM_PHYSICS_PNG, "") {
		Module::setup();
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	static void ItemRenderer_render(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData);

	static void applyTransformation(glm::mat4x4& mat);

	static void glm_rotate(glm::mat4x4& mat, float angle, float x, float y, float z);

	static glm::mat4x4 glm_rotate2(glm::mat4x4& mat, float angle, const glm::vec3& axis);

	void onSetupAndRender(SetupAndRenderEvent& event);
};
