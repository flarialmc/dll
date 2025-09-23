#include "CustomCrosshair.hpp"
#include "Client.hpp"

#include "Utils/Render/PositionUtils.hpp"
#include "GUI/Engine/Engine.hpp"
#include "Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include <thread>
#include <chrono>

void CustomCrosshair::onEnable() {
    Listen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
    Listen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
    Listen(this, TickEvent, &CustomCrosshair::onTick)
    Listen(this, RenderEvent, &CustomCrosshair::onRender)
    Module::onEnable();

    std::string Path = Utils::getClientPath() + "\\Crosshairs";

    for (const auto& entry : std::filesystem::directory_iterator(Path))
    {
        auto ch = new CrosshairImage(entry.path().string());
        Logger::debug("Crosshair: " + entry.path().string());

        std::string name = entry.path().filename().string();
        crosshairs[name.substr(0, name.size()-4)] = ch;

        Logger::debug("Crosshair loaded: " + name);

    }


    if (crosshairs.empty()) crosshairs["Crosshair1"] = new CrosshairImage();

    loadDefaultCrosshairTexture();
}

void CustomCrosshair::onDisable() {

    isRenderingSafe = false;

    Deafen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
    Deafen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
    Deafen(this, TickEvent, &CustomCrosshair::onTick)
    Deafen(this, RenderEvent, &CustomCrosshair::onRender)

    // Small delay to ensure any ongoing render operations complete
    std::this_thread::sleep_for(std::chrono::milliseconds(16));

    cleanupTextures();

    for (auto& pair : crosshairs) {
        delete pair.second;
    }
    crosshairs.clear();

    cleanupSamplerStates();

    Module::onDisable();

    // Re-enable rendering for next time module is enabled
    isRenderingSafe = true;
}

void CustomCrosshair::defaultConfig() {
    settings.renameSetting("defaultColor", "defaultOpacity", "defaultColorRGB", "default");
    settings.renameSetting("enemyColor", "enemyOpacity", "enemyColorRGB", "enemy");
    setDef("uiscale", 1.f);
    Module::defaultConfig("core");
    setDef("CustomCrosshair", false);
    setDef("highlightOnEntity", false);
    setDef("solidColorWhenHighlighted", true);
    setDef("solidColor", false);
    setDef("renderInThirdPerson", false);
    setDef("default", (std::string) "fafafa", 0.55f, false);
    setDef("enemy", (std::string) "FF0000", 1.f, false);
    setDef("CurrentSelectedColor", (std::string)"FFFFFF", 1.f, false);
    setDef("CurrentCrosshair", (std::string)"Crosshair1");
    setDef("ShowGridLines", true);
    setDef("HighlightMiddleLine", true);

    getOps<std::string>("CurrentSelectedColorCol") = "FFFFFF";
    getOps<float>("CurrentSelectedColorOpacity") = 1.f;
    
}

void CustomCrosshair::settingsRender(float settingsOffset) {
    actuallyRenderWindow = true;
    auto button = MC::mouseButton;
    if (blankWindow)
        MC::mouseButton = MouseButton::None;

    initSettingsPage();


        addHeader("Crosshair Editor");
        addButton("Crosshair Editor", "Opens the crosshair editor menu", "open", [&]() {
            blankWindow = !blankWindow;
        });
        addButton("Reload Crosshair", "Reloads Crosshair to apply any changes", "reload", [&]() {
            CrosshairReloaded = true;
        });

        addToggle("Show Grid Lines", "Shows the grid of the canvas.", "ShowGridLines");
        addToggle("Highlight Middle Line", "Adds 2 red lines to show the middle of the canvas", "HighlightMiddleLine");

        extraPadding();


    addHeader("Main");
    addToggle("Use Custom Crosshair", "Uses a custom crosshair.","CustomCrosshair");
    addToggle("Solid Color", "Make crosshair a solid color / more visible", "solidColor");
    addToggle("Render in Third Person", "Weather or not to render in third person", "renderInThirdPerson");
    addToggle("Highlight on Entity", "Highlight when enemy in reach", "highlightOnEntity");
    addConditionalToggle(getOps<bool>("highlightOnEntity"), "Solid Color When Highlighted", "Use solid color when highlighted", "solidColorWhenHighlighted");

    extraPadding();

    addHeader("Misc");
    addSlider("Crosshair Scale", "The size of the Crosshair (only for custom)", "uiscale", 10.f, 0.f, true);

    extraPadding();

    addHeader("Colors");
    addColorPicker("Default Color", "When the enemy is not in view.", "default");
    addConditionalColorPicker(getOps<bool>("highlightOnEntity"), "Enemy Color", "When the enemy is in view.", "enemy");


    FlarialGUI::UnsetScrollView();

    resetPadding();

    if (blankWindow) MC::mouseButton = button;
}

void CustomCrosshair::onGetViewPerspective(PerspectiveEvent &event) {
    if (!this->isEnabled()) return;
    currentPerspective = event.getPerspective();
}

void CustomCrosshair::onTick(TickEvent &event) {
    if (!this->isEnabled()) return;


    if (SDK::clientInstance) {
        auto player = SDK::clientInstance->getLocalPlayer();
        if (player) {
            isValidPlayer = true;

            if (isHudScreen) {
                isHoveringEntity = (player->getLevel()->getHitResult().type == HitResultType::Entity);
            }
        }
    }
}

void CustomCrosshair::onHudCursorRendererRender(HudCursorRendererRenderEvent &event) {
    if (!this->isEnabled() || !isRenderingSafe) return;
    if (!isValidPlayer || !isHudScreen) return;

    bool useCustomCrosshair = settings.getSettingByName<bool>("CustomCrosshair")->value;
    if (useCustomCrosshair) {
        event.cancel();
        return;
    }

}

void CustomCrosshair::onRender(RenderEvent &event) {
    if (!this->isEnabled()) return;
    isHudScreen = (SDK::getCurrentScreen() == "hud_screen");
    if (actuallyRenderWindow)
        CrosshairEditorWindow();
    else {
        blankWindow = false;
    }
    actuallyRenderWindow = false;
    if (!blankWindow) CurrentSelectedCrosshair = settings.getSettingByName<std::string>("CurrentCrosshair")->value;

    renderImGuiCrosshair();
}

ImTextureID CustomCrosshair::loadCrosshairTexture(const std::string& crosshairName) {
    std::string filePath = Utils::getClientPath() + "\\Crosshairs\\" + crosshairName + ".png";

    if (!std::filesystem::exists(filePath)) {
        return 0;
    }

    auto it = crosshairTextures.find(crosshairName);
    if (it != crosshairTextures.end()) {
        return it->second;
    }

    ID3D11ShaderResourceView* texture = nullptr;
    if (FlarialGUI::LoadImageFromFile(filePath, &texture)) {
        crosshairTextures[crosshairName] = (ImTextureID)texture;

        auto chIt = crosshairs.find(crosshairName);
        if (chIt != crosshairs.end() && chIt->second != nullptr) {
            crosshairSizes[crosshairName] = Vec2<int>(chIt->second->Size, chIt->second->Size);
        } else {
            crosshairSizes[crosshairName] = Vec2<int>(16, 16);
        }

        return (ImTextureID)texture;
    }

    return 0;
}

void CustomCrosshair::loadDefaultCrosshairTexture() {
    if (!SwapchainHook::d3d11Device) return;

    std::vector<unsigned char> blankData(16 * 16 * 4, 0);

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = 16;
    desc.Height = 16;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = blankData.data();
    initData.SysMemPitch = 16 * 4;

    winrt::com_ptr<ID3D11Texture2D> texture;
    if (SUCCEEDED(SwapchainHook::d3d11Device->CreateTexture2D(&desc, &initData, texture.put()))) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        ID3D11ShaderResourceView* srv = nullptr;
        if (SUCCEEDED(SwapchainHook::d3d11Device->CreateShaderResourceView(texture.get(), &srvDesc, &srv))) {
            defaultCrosshairTexture = (ImTextureID)srv;
            defaultCrosshairSize = Vec2<int>(16, 16);
        }
    }
}

void CustomCrosshair::cleanupTextures() {
    for (auto& pair : crosshairTextures) {
        if (pair.second) {
            ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)pair.second;
            srv->Release();
        }
    }
    crosshairTextures.clear();
    crosshairSizes.clear();

    if (defaultCrosshairTexture) {
        ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)defaultCrosshairTexture;
        srv->Release();
        defaultCrosshairTexture = 0;
    }
}

void CustomCrosshair::renderImGuiCrosshair() {
    if (!isRenderingSafe || !isValidPlayer || !isHudScreen) return;

    auto renderInThirdPerson = settings.getSettingByName<bool>("renderInThirdPerson")->value;
    if (!renderInThirdPerson && currentPerspective != Perspective::FirstPerson) return;

    bool useCustomCrosshair = settings.getSettingByName<bool>("CustomCrosshair")->value;
    if (!useCustomCrosshair) return;

    D2D1_COLOR_F color = isHoveringEntity && getOps<bool>("highlightOnEntity") ? getColor("enemy") : getColor("default");

    ImTextureID crosshairTexture = 0;
    Vec2<int> crosshairPixelSize;
    std::string currentCrosshair = settings.getSettingByName<std::string>("CurrentCrosshair")->value;

    if (!isRenderingSafe) return; // Early exit if disabling

    if (!currentCrosshair.empty()) {
        crosshairTexture = loadCrosshairTexture(currentCrosshair);
        if (crosshairTexture && isRenderingSafe) {
            crosshairPixelSize = crosshairSizes[currentCrosshair];
        }
    }

    if (!isRenderingSafe) return; // Check again before using default texture

    if (!crosshairTexture) {
        crosshairTexture = defaultCrosshairTexture;
        crosshairPixelSize = defaultCrosshairSize;
    }

    if (!crosshairTexture || !isRenderingSafe) return;

    float scale = settings.getSettingByName<float>("uiscale")->value;
    Vec2<float> crosshairSize = Vec2<float>(crosshairPixelSize.x * scale, crosshairPixelSize.y * scale);

    ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImVec2 crosshairPos = ImVec2(screenCenter.x - crosshairSize.x * 0.5f, screenCenter.y - crosshairSize.y * 0.5f);

    auto useSolidColor = settings.getSettingByName<bool>("solidColor")->value;
    auto useSolidColorWhenHighlighted = settings.getSettingByName<bool>("solidColorWhenHighlighted")->value;
    bool useSolid = (isHoveringEntity && getOps<bool>("highlightOnEntity") && useSolidColorWhenHighlighted) ||
                    (!isHoveringEntity && useSolidColor);

    ImU32 tintColor;
    if (useSolid) {
        tintColor = IM_COL32(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
    } else {
        tintColor = IM_COL32(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
    }

    static ID3D11SamplerState* pointSampler = nullptr;
    static ID3D11SamplerState* linearSampler = nullptr;

    if (!pointSampler) {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        SwapchainHook::d3d11Device->CreateSamplerState(&samplerDesc, &pointSampler);
    }

    if (!linearSampler) {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        SwapchainHook::d3d11Device->CreateSamplerState(&samplerDesc, &linearSampler);
    }

    if (!isRenderingSafe) return; // Final safety check before drawing

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    drawList->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
        static ID3D11SamplerState* pointSampler = nullptr;
        if (!pointSampler) {
            D3D11_SAMPLER_DESC samplerDesc = {};
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
            SwapchainHook::d3d11Device->CreateSamplerState(&samplerDesc, &pointSampler);
        }

        winrt::com_ptr<ID3D11DeviceContext> ctx = SwapchainHook::context;
        if (pointSampler && ctx) {
            ctx->PSSetSamplers(0, 1, &pointSampler);
        }
    }, nullptr);

    drawList->AddImage(
        crosshairTexture,
        crosshairPos,
        ImVec2(crosshairPos.x + crosshairSize.x, crosshairPos.y + crosshairSize.y),
        ImVec2(0, 0), ImVec2(1, 1),
        tintColor
    );

    drawList->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
        static ID3D11SamplerState* linearSampler = nullptr;
        if (!linearSampler) {
            D3D11_SAMPLER_DESC samplerDesc = {};
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
            SwapchainHook::d3d11Device->CreateSamplerState(&samplerDesc, &linearSampler);
        }

        winrt::com_ptr<ID3D11DeviceContext> ctx = SwapchainHook::context;
        if (linearSampler && ctx) {
            ctx->PSSetSamplers(0, 1, &linearSampler);
        }
    }, nullptr);
}

void CustomCrosshair::invalidateCrosshairTexture(const std::string& crosshairName) {
    auto it = crosshairTextures.find(crosshairName);
    if (it != crosshairTextures.end()) {
        ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)it->second;
        srv->Release();
        crosshairTextures.erase(it);
    }
    crosshairSizes.erase(crosshairName);
}

void CustomCrosshair::cleanupSamplerStates() {

}
