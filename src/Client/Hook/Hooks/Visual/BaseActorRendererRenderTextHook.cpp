#include "BaseActorRendererRenderTextHook.hpp"

#include "Client.hpp"
#include "Utils/Render/MaterialUtils.hpp"

void BaseActorRendererRenderTextHook::drawLogo(ScreenContext* screenContext, const Vec3<float>& cameraPos,
                                               const Vec3<float>& cameraTargetPos, const std::string& nameTag, const Vec3<float>& tagPos, Font* font)
{

    std::string clearedName = String::removeNonAlphanumeric(String::removeColorCodes(nameTag));
    if (clearedName.empty()) clearedName = String::removeColorCodes(nameTag); // nametag might contain some unclearable stuff

    if (MaterialUtils::getUITextured() == nullptr)
        MaterialUtils::update();

    // maintaining the old structure below, can be used in future
    /*static std::map<std::string, std::string> roleLogos = {
                {"Dev", "dev-logo.png"},
                {"Staff", "white-logo.png"},
                {"Gamer", "gamer-logo.png"},
                {"Booster", "booster-logo.png"},
                {"Regular", "red-logo.png"}
        };*/
    constexpr auto roleLogos = std::to_array<std::pair<std::string_view, std::string_view>>({
        {"Dev", "dev-logo.png"},
        {"Staff", "white-logo.png"},
        {"Gamer", "gamer-logo.png"},
        {"Booster", "booster-logo.png"},
        {"Supporter", "supporter-logo.png"},
        {"Regular", "red-logo.png"}
    });

    std::optional<ResourceLocation> loc{};

    for (const auto& [role, logo] : roleLogos) {
        if (APIUtils::hasRole(role, clearedName)) {
            loc.emplace(std::format("{}{}{}", Utils::getAssetsPath(), "\\", logo), true);
            break;
        }
    }

    if (!loc) {
        return;
    }

    if (!SDK::clientInstance->getMinecraftGame()->textureGroup) {
        return;
    }

    TexturePtr ptr = SDK::clientInstance->getMinecraftGame()->textureGroup->getTexture(*loc, false);

    if(ptr.clientTexture == nullptr || ptr.clientTexture->clientTexture.resourcePointerBlock == nullptr)
        return;

    constexpr float DEG_RAD = 180.0f / 3.1415927f;

    const auto rotPos = cameraPos.sub(tagPos);
    const auto rot = mce::MathUtility::getRotationFromNegZToVector(rotPos);
    const float yaw = rot.y * -DEG_RAD;
    const float pitch = rot.x * DEG_RAD;
    const auto pos = tagPos.sub(cameraTargetPos);

    auto& stack = SDK::clientInstance->getCamera().getWorldMatrixStack();

    stack.push();

    auto& matrix = stack.top().matrix;

    matrix = translate(matrix, {pos.x, pos.y, pos.z});
    matrix = rotate(matrix, glm::radians(yaw), {0.f, 1.f, 0.f});
    matrix = rotate(matrix, glm::radians(pitch), {1.f, 0.f, 0.f});

    const auto mScale = 0.026666669f; // 0.16f
    matrix = scale(matrix, {mScale * -1, mScale * -1, mScale});

    const float fontHeight = font->getLineHeight();
    float x;
    const float size = fontHeight;
    const float y = -1.f;

    if (std::ranges::find(nameTag.begin(), nameTag.end(), '\n') != nameTag.end()) {
        const auto split = Utils::splitString(nameTag, '\n');

        float width = 0.f;

        for (const auto& tag : split) {
            const auto w = font->getLineLength(tag, 1.f, false);

            if (w > width)
                width = w;
        }

        x = -(width / 2.f + size + 2.f);
    }
    else
        x = -(font->getLineLength(nameTag, 1.f, false) / 2.f + size + 2.f);

    const auto shaderColor = screenContext->getColorHolder();

    shaderColor->r = 1.f;
    shaderColor->g = 1.f;
    shaderColor->b = 1.f;
    shaderColor->a = 1.f;
    // shaderColor->shouldDelete = true;

    const auto tess = screenContext->getTessellator();

    tess->begin();

    tess->vertexUV(x, y, 0.f, 0.f, 0.f);
    tess->vertexUV(x, y + size, 0.f, 0.f, 1.f);
    tess->vertexUV(x + size, y, 0.f, 1.f, 0.f);

    tess->vertexUV(x, y + size, 0.f, 0.f, 1.f);
    tess->vertexUV(x + size, y + size, 0.f, 1.f, 1.f);
    tess->vertexUV(x + size, y, 0.f, 1.f, 0.f);

    MeshHelpers::renderMeshImmediately2(screenContext, tess, MaterialUtils::getNametag(), *ptr.clientTexture);

    stack.pop();
}

bool BaseActorRendererRenderTextHook::contains(const std::vector<std::string>& vec, const std::string& str)
{
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

void BaseActorRendererRenderTextHook::printVector(const std::vector<std::string>& vec)
{
    for (const auto& str : vec) {
        std::cout << str << std::endl;
    }
}

void BaseActorRendererRenderTextHook::BaseActorRenderer_renderTextCallback(ScreenContext* screenContext,
    ViewRenderData* viewData, NameTagRenderObject* tagData, Font* font, float size)
{

    if (!Client::settings.getSettingByName<bool>("nologoicon")->value)
        drawLogo(screenContext, viewData->cameraPos, viewData->cameraTargetPos, tagData->nameTag, tagData->pos, font);
    funcOriginal(screenContext, viewData, tagData, font, size);
}

void BaseActorRendererRenderTextHook::BaseActorRenderer_renderTextCallback40(ScreenContext* screenContext,
    ViewRenderData* viewData, NameTagRenderObject* tagData, Font* font, void* mesh)
{

    if (!Client::settings.getSettingByName<bool>("nologoicon")->value)
        drawLogo(screenContext, viewData->cameraPos, viewData->cameraTargetPos, tagData->nameTag, tagData->pos, font);
    funcOriginal40(screenContext, viewData, tagData, font, mesh);
}

BaseActorRendererRenderTextHook::BaseActorRendererRenderTextHook(): Hook("BaseActorRenderer renderText Hook", GET_SIG_ADDRESS("BaseActorRenderer::renderText"))
{}

void BaseActorRendererRenderTextHook::enableHook()
{
    static auto sig = Memory::offsetFromSig(address, 1);

    if (VersionUtils::checkAboveOrEqual(20, 40))
        this->manualHook( (void*) sig, (void*) BaseActorRenderer_renderTextCallback40, (void **) &funcOriginal40);
    else
        this->manualHook( (void*) sig, (void*) BaseActorRenderer_renderTextCallback, (void **) &funcOriginal);

}
