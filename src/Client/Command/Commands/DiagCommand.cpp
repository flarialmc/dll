#include "DiagCommand.hpp"

#include "../../Client.hpp"
#include "../../GUI/D2D.hpp"
#include "../../GUI/Engine/EngineCore.hpp"
#include "../../Hook/Hooks/Render/DirectX/DXGI/SwapchainHook.hpp"
#include "../../Module/Manager.hpp"
#include "../../Module/Modules/ClickGUI/ClickGUI.hpp"
#include "Utils/PlatformUtils.hpp"
#include <kiero/kiero.h>

// These are defined in SwapchainHook.cpp / SwapchainHook_DX12.cpp with external linkage
extern bool dx12DeviceRemoved;
extern bool dx11FallbackComplete;

static std::string kieroRenderTypeString(kiero::RenderType::Enum type) {
    switch (type) {
        case kiero::RenderType::None:  return "None";
        case kiero::RenderType::D3D9:  return "D3D9";
        case kiero::RenderType::D3D10: return "D3D10";
        case kiero::RenderType::D3D11: return "D3D11";
        case kiero::RenderType::D3D12: return "D3D12";
        default: return fmt::format("Unknown({})", static_cast<int>(type));
    }
}

void DiagCommand::execute(const std::vector<std::string>& args) {
    std::vector<std::string> lines;

    // Header
    lines.emplace_back("=== Flarial Diagnostic Dump ===");

    // Version / Platform
    lines.emplace_back(fmt::format("Flarial commit: {}", COMMIT_HASH));
    lines.emplace_back(fmt::format("MC version: {}", Client::version));
    lines.emplace_back(fmt::format("Platform: {}", PlatformUtils::getPlatformString()));
    lines.emplace_back(fmt::format("GPU: {}", MC::GPU));

    // DirectX state — this is the critical section for invisible GUI debugging
    lines.emplace_back("--- DirectX State ---");
    const bool kieroSaysDX12 = kiero::getRenderType() == kiero::RenderType::D3D12;
    lines.emplace_back(fmt::format("kiero render type: {}", kieroRenderTypeString(kiero::getRenderType())));
    lines.emplace_back(fmt::format("SwapchainHook::isDX12: {}", SwapchainHook::isDX12));
    if (kieroSaysDX12 != SwapchainHook::isDX12) lines.emplace_back("*** API MISMATCH: kiero and runtime disagree! Phase 2 rehook should have fixed this. ***");
    lines.emplace_back(fmt::format("SwapchainHook::init: {}", SwapchainHook::init.load()));
    lines.emplace_back(fmt::format("SwapchainHook::initImgui: {}", SwapchainHook::initImgui.load()));
    lines.emplace_back(fmt::format("dx12DeviceRemoved: {}", dx12DeviceRemoved));
    lines.emplace_back(fmt::format("dx11FallbackComplete: {}", dx11FallbackComplete));
    lines.emplace_back(fmt::format("D2D::context exists: {}", D2D::context != nullptr));
    lines.emplace_back(fmt::format("D3D11 device exists: {}", SwapchainHook::d3d11Device != nullptr));
    lines.emplace_back(fmt::format("D3D11 context exists: {}", SwapchainHook::context != nullptr));
    lines.emplace_back(fmt::format("Swapchain ptr exists: {}", SwapchainHook::swapchain != nullptr));

    bool killdx = false;
    if (const auto* setting = Client::settings.getSettingByName<bool>("killdx")) killdx = setting->value;
    lines.emplace_back(fmt::format("Better Frames (killdx): {}", killdx));

    bool recreateAtStart = false;
    if (const auto* setting = Client::settings.getSettingByName<bool>("recreateAtStart")) recreateAtStart = setting->value;
    lines.emplace_back(fmt::format("recreateAtStart: {}", recreateAtStart));

    // Window / Rendering
    lines.emplace_back("--- Window / Rendering ---");
    lines.emplace_back(fmt::format("windowSize: {}x{}", MC::windowSize.x, MC::windowSize.y));
    lines.emplace_back(fmt::format("FPS: {}", MC::fps));
    lines.emplace_back(fmt::format("frameTime: {:.2f}ms", MC::frameTime));
    lines.emplace_back(fmt::format("frameFactor: {:.4f}", FlarialGUI::frameFactor));

    // Screen state
    lines.emplace_back("--- Screen State ---");
    lines.emplace_back(fmt::format("currentScreen: {}", SDK::getCurrentScreen()));
    lines.emplace_back(fmt::format("Client::init: {}", Client::init));
    lines.emplace_back(fmt::format("Client::disable: {}", Client::disable));

    // ClickGUI state — key diagnostic for invisible GUI
    lines.emplace_back("--- ClickGUI State ---");
    // auto clickgui = ModuleManager::getModule("ClickGUI");
    if (clickgui) {
        lines.emplace_back(fmt::format("active: {}", clickgui->active));
        lines.emplace_back(fmt::format("enabledState: {}", clickgui->enabledState));
        lines.emplace_back(fmt::format("keybind: '{}'", clickgui->getKeybind()));
        lines.emplace_back(fmt::format("baseHeightActual: {:.6f}", ClickGUI::baseHeightActual));
        lines.emplace_back(fmt::format("editmenu: {}", ClickGUI::editmenu));
    } else lines.emplace_back("ClickGUI module NOT FOUND");

    // Module summary
    lines.emplace_back("--- Modules ---");
    lines.emplace_back(fmt::format("Total registered: {}", ModuleManager::moduleMap.size()));
    int enabledCount = 0;
    for (const auto& mod : ModuleManager::moduleMap | std::views::values) if (mod && mod->enabledState) enabledCount++;
    lines.emplace_back(fmt::format("Enabled: {}", enabledCount));

    // UIControl tree info
    lines.emplace_back("--- UIControl Tree ---");
    if (SDK::screenView) {
        lines.emplace_back("screenView: exists");
        try {
            if (const auto root = SDK::screenView->VisualTree->root) lines.emplace_back(fmt::format("root layer: {}", root->getLayerName()));
            else lines.emplace_back("root: null");
        } catch (...) {
            lines.emplace_back("root: access error");
        }
    } else lines.emplace_back("screenView: null");

    // Movable modules status
    lines.emplace_back("--- Movable Modules ---");
    for (const auto& name : {"Movable Hotbar", "Movable Scoreboard", "Movable Chat", "Paper Doll",
                              "Movable Actionbar", "Movable Boss Bar", "Movable Hunger",
                              "Movable Paperdoll", "Movable XP Bar"})
        if (const auto mod = ModuleManager::getModule(name)) lines.emplace_back(fmt::format("{}: enabled={}", name, mod->enabledState));

    lines.emplace_back("=== End Diagnostic Dump ===");

    // Write to log file
    Logger::info("--- DIAGNOSTIC DUMP START ---");
    for (const auto& line : lines) Logger::info("{}", line);
    Logger::info("--- DIAGNOSTIC DUMP END ---");

    // Write abbreviated info to chat (full dump is in log)
    addCommandMessage("§aDiagnostic dump written to log file.");
    addCommandMessage("§7Flarial §f{} §7| MC §f{} §7| §f{}", COMMIT_HASH, Client::version, PlatformUtils::getPlatformString());
    addCommandMessage("§7DX: kiero=§f{} §7isDX12=§f{} §7init=§f{} §7imgui=§f{}",
        kieroRenderTypeString(kiero::getRenderType()),
        SwapchainHook::isDX12,
        SwapchainHook::init.load(),
        SwapchainHook::initImgui.load());
    addCommandMessage("§7D2D=§f{} §7killdx=§f{} §7dxRemoved=§f{} §7dx11Fallback=§f{}",
        D2D::context != nullptr,
        killdx,
        dx12DeviceRemoved,
        dx11FallbackComplete);
    addCommandMessage("§7Screen: §f{} §7| FPS: §f{} §7| Window: §f{}x{}",
        SDK::getCurrentScreen(), MC::fps,
        static_cast<int>(MC::windowSize.x), static_cast<int>(MC::windowSize.y));

    if (clickgui)
        addCommandMessage("§7ClickGUI: active=§f{} §7keybind=§f'{}' §7baseHeight=§f{:.4f}",
            clickgui->active, clickgui->getKeybind(), ClickGUI::baseHeightActual);

    addCommandMessage("§7Modules: §f{} §7registered, §f{} §7enabled", ModuleManager::moduleMap.size(), enabledCount);
    addCommandMessage("§eLog path: §f%localappdata%\\Flarial\\Client\\logs\\latest.log");
    addCommandMessage("§cSend your log file when requested to.");
}
