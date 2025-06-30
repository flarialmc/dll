#pragma once

#include <glm/glm/glm.hpp>
#include <glm/glm/ext/matrix_transform.hpp>
#include <Utils/APIUtils.hpp>

#include "../Hook.hpp"
#include "../../../../SDK/Client/Render/Font.hpp"
#include "../../../../SDK/Client/Render/NameTagRenderObject.hpp"
#include "../../../../SDK/Client/Render/ScreenContext.hpp"
#include "../../../../SDK/Client/Render/ViewRenderData.hpp"
#include "../../../../SDK/Client/Render/Tessellator/MeshHelpers.hpp"
#include "../../../../SDK/Client/Util/MathUtility.hpp"
#include "../../../../Utils/Memory/Memory.hpp"
#include "../../../../Utils/Utils.hpp"

class BaseActorRendererRenderTextHook : public Hook {
    static void drawLogo(ScreenContext* screenContext, const Vec3<float>& cameraPos, const Vec3<float>& cameraTargetPos, const std::string& nameTag, const Vec3<float>& tagPos, Font* font);

    static bool contains(const std::vector<std::string>& vec, const std::string& str);

    static void printVector(const std::vector<std::string>& vec);

    static void BaseActorRenderer_renderTextCallback(ScreenContext* screenContext, ViewRenderData* viewData, NameTagRenderObject* tagData, Font* font, float size);

    static void BaseActorRenderer_renderTextCallback40(ScreenContext* screenContext, ViewRenderData* viewData, NameTagRenderObject* tagData, Font* font, void* mesh);

public:
    typedef void(__fastcall* BaseActorRenderer_renderTextOriginal)(ScreenContext*, ViewRenderData*, NameTagRenderObject*, Font*, float size);
    typedef void(__fastcall* BaseActorRenderer_renderTextOriginal40)(ScreenContext*, ViewRenderData*, NameTagRenderObject*, Font*, void* mesh);

    static inline BaseActorRenderer_renderTextOriginal funcOriginal = nullptr;
    static inline BaseActorRenderer_renderTextOriginal40 funcOriginal40 = nullptr;

    BaseActorRendererRenderTextHook();

    void enableHook() override;
};


