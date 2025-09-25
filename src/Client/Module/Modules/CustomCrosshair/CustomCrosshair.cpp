#include "CustomCrosshair.hpp"
#include "Client.hpp"

#include "Utils/Render/PositionUtils.hpp"
#include "GUI/Engine/Engine.hpp"
#include "Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "GUI/Engine/Constraints.hpp"
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

    if (SwapchainHook::isDX12) {
        loadDefaultCrosshairTextureDX12();
    } else {
        loadDefaultCrosshairTexture();
    }
}

void CustomCrosshair::onDisable() {

    isRenderingSafe = false;

    Deafen(this, PerspectiveEvent, &CustomCrosshair::onGetViewPerspective)
    Deafen(this, HudCursorRendererRenderEvent, &CustomCrosshair::onHudCursorRendererRender)
    Deafen(this, TickEvent, &CustomCrosshair::onTick)
    Deafen(this, RenderEvent, &CustomCrosshair::onRender)

    std::this_thread::sleep_for(std::chrono::milliseconds(16));

    cleanupTextures();
    cleanupTexturesDX12();

    for (auto& pair : crosshairs) {
        delete pair.second;
    }
    crosshairs.clear();

    cleanupSamplerStates();

    Module::onDisable();

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
    isHudScreen = SDK::currentScreen == "hud_screen";
    if (actuallyRenderWindow)
        CrosshairEditorWindow();
    else {
        blankWindow = false;
    }
    actuallyRenderWindow = false;
    if (!blankWindow) CurrentSelectedCrosshair = settings.getSettingByName<std::string>("CurrentCrosshair")->value;

    // Handle crosshair reload
    if (CrosshairReloaded) {
        std::string currentCrosshair = settings.getSettingByName<std::string>("CurrentCrosshair")->value;
        if (!currentCrosshair.empty()) {
            reloadCrosshairFromFile(currentCrosshair);
        }
        CrosshairReloaded = false;
    }

    renderImGuiCrosshair();
}

ImTextureID CustomCrosshair::loadCrosshairTexture(const std::string& crosshairName) {
    if (!isRenderingSafe) return 0;

    std::string filePath = Utils::getClientPath() + "\\Crosshairs\\" + crosshairName + ".png";

    if (!std::filesystem::exists(filePath)) {
        return 0;
    }

    auto it = crosshairTextures.find(crosshairName);
    if (it != crosshairTextures.end() && it->second) {
        return reinterpret_cast<ImTextureID>(it->second.get());
    }

    ID3D11ShaderResourceView* texture = nullptr;
    if (FlarialGUI::LoadImageFromFile(filePath, &texture) && texture) {
        crosshairTextures[crosshairName].attach(texture);

        auto chIt = crosshairs.find(crosshairName);
        if (chIt != crosshairs.end() && chIt->second != nullptr && chIt->second->isValid()) {
            crosshairSizes[crosshairName] = Vec2<int>(chIt->second->Size, chIt->second->Size);
        } else {
            crosshairSizes[crosshairName] = Vec2<int>(16, 16);
        }

        return reinterpret_cast<ImTextureID>(texture);
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
            defaultCrosshairTexture.attach(srv);
            defaultCrosshairSize = Vec2<int>(16, 16);
        }
    }
}

void CustomCrosshair::cleanupTextures() {

    crosshairTextures.clear();
    crosshairSizes.clear();

    if (defaultCrosshairTexture) {
        defaultCrosshairTexture = nullptr;
    }
}

void CustomCrosshair::renderImGuiCrosshair() {
    if (!isRenderingSafe || !isValidPlayer || !isHudScreen || ModuleManager::getModule("ClickGUI")->isEnabled()) return;

    auto renderInThirdPerson = settings.getSettingByName<bool>("renderInThirdPerson")->value;
    if (!renderInThirdPerson && currentPerspective != Perspective::FirstPerson) return;

    bool useCustomCrosshair = settings.getSettingByName<bool>("CustomCrosshair")->value;
    if (!useCustomCrosshair) return;

    D2D1_COLOR_F color = isHoveringEntity && getOps<bool>("highlightOnEntity") ? getColor("enemy") : getColor("default");

    ImTextureID crosshairTexture = 0;
    Vec2<int> crosshairPixelSize;
    std::string currentCrosshair = settings.getSettingByName<std::string>("CurrentCrosshair")->value;

    if (!isRenderingSafe) return;

    if (!currentCrosshair.empty()) {
        if (SwapchainHook::isDX12) {
            crosshairTexture = loadCrosshairTextureDX12(currentCrosshair);
        } else {
            crosshairTexture = loadCrosshairTexture(currentCrosshair);
        }
        if (crosshairTexture && isRenderingSafe) {
            crosshairPixelSize = crosshairSizes[currentCrosshair];
        }
    }

    if (!isRenderingSafe) return;

    if (!crosshairTexture) {
        if (SwapchainHook::isDX12) {
            if (!defaultCrosshairTextureDX12 || defaultCrosshairTextureDX12Handle == 0) {
                loadDefaultCrosshairTextureDX12();
            }
            crosshairTexture = defaultCrosshairTextureDX12Handle;
        } else {
            if (!defaultCrosshairTexture) {
                loadDefaultCrosshairTexture();
            }
            crosshairTexture = reinterpret_cast<ImTextureID>(defaultCrosshairTexture.get());
        }
        crosshairPixelSize = defaultCrosshairSize;
    }

    if (!crosshairTexture || !isRenderingSafe) return;

    float scale = settings.getSettingByName<float>("uiscale")->value;
    Vec2<float> crosshairSize = Vec2<float>(crosshairPixelSize.x * scale, crosshairPixelSize.y * scale);

    ImVec2 screenCenter = ImVec2(Constraints::PercentageConstraint(1.f, "left"), Constraints::PercentageConstraint(1.f, "top"));
    // Round to integer pixel coordinates for crisp rendering
    ImVec2 crosshairPos = ImVec2(floorf((screenCenter.x - crosshairSize.x) * 0.5f), floorf((screenCenter.y - crosshairSize.y) * 0.5f));

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

    if (!pointSampler) {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        SwapchainHook::d3d11Device->CreateSamplerState(&samplerDesc, pointSampler.put());
    }

    if (!isRenderingSafe) return;

    if (!isRenderingSafe) return; // Final safety check before drawing

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    if (!SwapchainHook::isDX12) {
        // DX11: Use class-level sampler in callback for proper resource management
        drawList->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
            ID3D11SamplerState* sampler = CustomCrosshair::pointSampler.get();
            SwapchainHook::context->PSSetSamplers(0, 1, &sampler);

        }, nullptr);
    } else {
        // DX12: Use the new dynamic sampler system from ImGui backend
        drawList->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
            auto cmdlist = SwapchainHook::d3d12CommandList.get();
            auto pointSamplerHandle = ImGui_ImplDX12_GetPointSamplerHandle();
            cmdlist->SetGraphicsRootDescriptorTable(2, pointSamplerHandle);  // Slot 2 is now the sampler table
        }, nullptr);

    }

    drawList->AddImage(
        crosshairTexture,
        crosshairPos,
        ImVec2(crosshairPos.x + crosshairSize.x, crosshairPos.y + crosshairSize.y),
        ImVec2(0, 0), ImVec2(1, 1),
        tintColor
    );

    // Reset to default rendering state
    if (!SwapchainHook::isDX12) {
        drawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
    } else {
        // DX12: Restore linear sampler for subsequent draws (e.g., fonts)
        drawList->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
            auto cmdlist = SwapchainHook::d3d12CommandList.get();
            auto linearSamplerHandle = ImGui_ImplDX12_GetLinearSamplerHandle();
            cmdlist->SetGraphicsRootDescriptorTable(2, linearSamplerHandle);  // Restore linear sampler
        }, nullptr);
    }


}

void CustomCrosshair::invalidateCrosshairTexture(const std::string& crosshairName) {
    // Clean up DX11 textures
    auto it = crosshairTextures.find(crosshairName);
    if (it != crosshairTextures.end()) {
        it->second = nullptr;
        crosshairTextures.erase(it);
    }

    // Clean up DX12 textures
    auto dx12It = crosshairTexturesDX12.find(crosshairName);
    if (dx12It != crosshairTexturesDX12.end()) {
        if (dx12It->second.second) {
            dx12It->second.second = nullptr;
        }
        crosshairTexturesDX12.erase(dx12It);
    }

    // Remove size information
    crosshairSizes.erase(crosshairName);
}

ImTextureID CustomCrosshair::loadCrosshairTextureDX12(const std::string& crosshairName) {
    if (!isRenderingSafe) return 0;

    std::string filePath = Utils::getClientPath() + "\\Crosshairs\\" + crosshairName + ".png";

    if (!std::filesystem::exists(filePath)) {
        return 0;
    }

    auto it = crosshairTexturesDX12.find(crosshairName);
    if (it != crosshairTexturesDX12.end() && it->second.first != 0) {
        return it->second.first;
    }

    if (!SwapchainHook::d3d12Device5 || !SwapchainHook::d3d12DescriptorHeapImGuiRender) {
        return 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu;

    if (!SwapchainHook::AllocateImageDescriptor(SwapchainHook::nextAvailableDescriptorIndex, &cpu, &gpu)) {
        return 0;
    }

    ID3D12Resource* texture = nullptr;
    if (FlarialGUI::LoadImageFromFile(filePath, cpu, &texture) && texture) {
        crosshairTexturesDX12[crosshairName].first = (ImTextureID)gpu.ptr;
        crosshairTexturesDX12[crosshairName].second.attach(texture);

        auto chIt = crosshairs.find(crosshairName);
        if (chIt != crosshairs.end() && chIt->second != nullptr && chIt->second->isValid()) {
            crosshairSizes[crosshairName] = Vec2<int>(chIt->second->Size, chIt->second->Size);
        } else {
            crosshairSizes[crosshairName] = Vec2<int>(16, 16);
        }

        return (ImTextureID)gpu.ptr;
    }

    return 0;
}

void CustomCrosshair::loadDefaultCrosshairTextureDX12() {
    if (!SwapchainHook::d3d12Device5 || !SwapchainHook::d3d12DescriptorHeapImGuiRender) return;

    // Create a temporary 16x16 transparent PNG in memory and use FlarialGUI::LoadImageFromFile
    D3D12_CPU_DESCRIPTOR_HANDLE cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu;

    if (!SwapchainHook::AllocateImageDescriptor(SwapchainHook::nextAvailableDescriptorIndex, &cpu, &gpu)) {
        return;
    }

    // Create a simple 1x1 transparent texture instead of complex manual creation
    std::vector<uint8_t> transparentPixel = {0, 0, 0, 0}; // RGBA transparent

    // Create texture resource directly in PIXEL_SHADER_RESOURCE state (no copy needed)
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    winrt::com_ptr<ID3D12Resource> texture;
    HRESULT hr = SwapchainHook::d3d12Device5->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(texture.put())
    );

    if (FAILED(hr)) return;

    // Create SRV directly since texture is already in correct state
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    SwapchainHook::d3d12Device5->CreateShaderResourceView(texture.get(), &srvDesc, cpu);

    defaultCrosshairTextureDX12 = texture;
    defaultCrosshairTextureDX12Handle = (ImTextureID)gpu.ptr;
    defaultCrosshairSize = Vec2<int>(16, 16);
}

void CustomCrosshair::cleanupTexturesDX12() {

    for (auto& pair : crosshairTexturesDX12) {
        if (pair.second.second) {
            pair.second.second = nullptr;
        }
    }
    crosshairTexturesDX12.clear();
    defaultCrosshairTextureDX12 = nullptr;
    defaultCrosshairTextureDX12Handle = 0;
}

void CustomCrosshair::reinitializeAfterResize() {
    if (!this->isEnabled()) return;

    crosshairTextures.clear();
    crosshairSizes.clear();
    crosshairTexturesDX12.clear();

    defaultCrosshairTexture = nullptr;
    defaultCrosshairTextureDX12 = nullptr;
    defaultCrosshairTextureDX12Handle = 0;

    pointSampler = nullptr;
}


void CustomCrosshair::cleanupSamplerStates() {

}

void CustomCrosshair::reloadCrosshairFromFile(const std::string& crosshairName) {
    if (!isRenderingSafe || crosshairName.empty()) return;

    std::string filePath = Utils::getClientPath() + "\\Crosshairs\\" + crosshairName + ".png";
    if (!std::filesystem::exists(filePath)) return;

    // Clean up existing crosshair
    auto it = crosshairs.find(crosshairName);
    if (it != crosshairs.end() && it->second != nullptr) {
        delete it->second;
        it->second = nullptr;
    }

    // Invalidate cached textures
    invalidateCrosshairTexture(crosshairName);

    // Load new crosshair from file
    crosshairs[crosshairName] = new CrosshairImage(filePath);
}
