#pragma once

#include <random>
#include "../Module.hpp"
#include "../../../../SDK/Client/Actor/ItemActor.hpp"
#include "../../../../Utils/Memory/CustomAllocator/Buffer.hpp"
#include "../../../../SDK/Client/Render/ActorRenderData.hpp"
#include "../../../Hook/Hooks/Hook.hpp"

#include "SDK/Client/Render/ItemRenderer.hpp"

class ItemRendererRenderHook : public Hook {
private:
    static void callback(ItemRenderer* _this, BaseActorRenderContext* renderCtx, ActorRenderData* renderData);

public:
    typedef void(__thiscall *original)(ItemRenderer*, BaseActorRenderContext*, ActorRenderData*);
    static inline original funcOriginal = nullptr;

    ItemRendererRenderHook();
    void enableHook() override;
};

class GlmRotateHook : public Hook {
private:
    static void callback(glm::mat4x4& mat, float angle, float x, float y, float z);
    static glm::mat4x4 callback2(glm::mat4x4& mat, float angle, const glm::vec3& axis);

public:
    typedef void(__fastcall *original)(glm::mat4x4&, float, float, float, float);
    typedef glm::mat4x4(__fastcall *original2)(glm::mat4x4&, float, const glm::vec3&);
    static inline original funcOriginal = nullptr;
    static inline original2 funcOriginal2 = nullptr;

    GlmRotateHook();
    void enableHook() override;
};

static char data[0x5], data2[0x5];

inline std::unique_ptr<ItemRendererRenderHook> itemRendererHook;
inline std::unique_ptr<GlmRotateHook> glmRotateHook;

class ItemPhysics : public Module {
private:
	uint32_t origPosRel = 0;
	float* newPosRel = nullptr;
	bool patched = false;

	std::unordered_map<Actor*, std::tuple<float, Vec3<float>, Vec3<int>>> actorData;
public:
	ActorRenderData* renderData = nullptr;
	ItemPhysics() : Module("Item Physics", "Changes rotation behavior of dropped items",
		IDR_ITEM_PHYSICS_PNG, "", false, {"animation"}) {
		
	}

	void onEnable() override;

	void onDisable() override;

	void defaultConfig() override;

	void settingsRender(float settingsOffset) override;

	static void applyTransformation(glm::mat4x4& mat);

	void onSetupAndRender(SetupAndRenderEvent& event);
};
