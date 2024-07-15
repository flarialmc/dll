#include "OffsetInit.hpp"

#include "../../../Logger/Logger.hpp"
#include "../SignatureAndOffsetManager.hpp"

void OffsetInit::init2100() {
    Logger::debug("[Offsets] Loading offsets for 1.21.0X");

    // Keystrokes
    ADD_OFFSET("MoveInputComponent::forward", 0x0A);
    ADD_OFFSET("MoveInputComponent::backward", 0x0B);
    ADD_OFFSET("MoveInputComponent::left", 0x0C);
    ADD_OFFSET("MoveInputComponent::right", 0x0D);
    ADD_OFFSET("MoveInputComponent::sneaking", 0x20);
    ADD_OFFSET("MoveInputComponent::jumping", 0x26);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x27);

    // Hitboxes and other +/- 1
    ADD_OFFSET("Actor::getActorFlag", 0);
    ADD_OFFSET("Actor::isAlive", 50);
    ADD_OFFSET("Actor::isValidTarget", 56);

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 7);

    ADD_OFFSET("ClientInstance::getBlockSource", 28);
    ADD_OFFSET("ClientInstance::getLocalPlayer", 29);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("Packet::getId", 1);

    ADD_OFFSET("Actor::hurtTime", 0x20C); // ?hurtEffects@Mob@@UEAAXAEBVActorDamageSource@@M_N1@Z Mob::hurtEffects 2nd after Actor::getHealth(void)
    ADD_OFFSET("Actor::level", 0x250);
    ADD_OFFSET("Actor::categories", 0x288);
    ADD_OFFSET("Actor::baseTickVft", 26);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x760);
    ADD_OFFSET("Player::playerName", 0x1D30);
    ADD_OFFSET("Player::gamemode", 0xEC8);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x10);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0xC8);
    ADD_OFFSET("ClientInstance::guiData", 0x558);
    ADD_OFFSET("ClientInstance::Matrix1", 0x330);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F0);
    ADD_OFFSET("ClientInstance::getFovY", 0x704);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x1A8);

    ADD_OFFSET("Option::optionInformation", 0x8);
    ADD_OFFSET("Option::value", 0x10);
    ADD_OFFSET("Option::value1", 0x70);
    ADD_OFFSET("OptionInfo::TranslateName", 0x158);

    ADD_OFFSET("Item::AtlasTextureFile", 0x8);
    ADD_OFFSET("Item::Namespace", 0xF8);
    ADD_OFFSET("Item::name",0xD0);

    ADD_OFFSET("ItemStack::tag", 0x10);
    ADD_OFFSET("ItemStack::count", 0x22);

    ADD_OFFSET("Biome::temparature", 0x38);

    ADD_OFFSET("Dimension::weather", 0x1B0);

    ADD_OFFSET("Weather::lightingLevel", 0x48);
    ADD_OFFSET("Weather::rainLevel", 0x3C);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x308);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5FC);

    ADD_OFFSET("LoopbackPacketSender::networkSystem", 0x20);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x80);

    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x60);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x458);
    ADD_OFFSET("RaknetConnector::getPeer", 0x298);
    ADD_OFFSET("RakPeer::GetAveragePing", 44);

    ADD_OFFSET("BaseActorRenderContext::itemRenderer", 0x58);

    ADD_OFFSET("GuiData::ScreenSize", 0x30);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x40);
    ADD_OFFSET("GuiData::GuiScale", 0x4C);
    ADD_OFFSET("GuiData::screenResRounded", 0x38);
    ADD_OFFSET("GuiData::sliderAmount", 0x4C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x50);

    ADD_OFFSET("MinecraftUIRenderContext::clientInstance", 0x8);
    ADD_OFFSET("MinecraftUIRenderContext::screenContext", 0x10);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xB38);
    ADD_OFFSET("Level::getPlayerMap", 0x1BC8);
}

void OffsetInit::init2080() {
    Logger::debug("[Offsets] Loading offsets for 1.20.8X");

    // Keystrokes
    ADD_OFFSET("MoveInputComponent::forward", 0x0A);
    ADD_OFFSET("MoveInputComponent::backward", 0x0B);
    ADD_OFFSET("MoveInputComponent::left", 0x0C);
    ADD_OFFSET("MoveInputComponent::right", 0x0D);
    ADD_OFFSET("MoveInputComponent::sneaking", 0x20);
    ADD_OFFSET("MoveInputComponent::jumping", 0x26);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x27);

    // Hitboxes and other +/- 1
    ADD_OFFSET("Actor::getActorFlag", 0);
    ADD_OFFSET("Actor::isAlive", 51);
    ADD_OFFSET("Actor::isValidTarget", 57);

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 5);

    ADD_OFFSET("ClientInstance::getBlockSource", 28);
    ADD_OFFSET("ClientInstance::getLocalPlayer", 29);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("Packet::getId", 1);

    ADD_OFFSET("Actor::hurtTime", 0x214); // ?hurtEffects@Mob@@UEAAXAEBVActorDamageSource@@M_N1@Z Mob::hurtEffects 2nd after Actor::getHealth(void)
    ADD_OFFSET("Actor::level", 0x258);
    ADD_OFFSET("Actor::categories", 0x290);
    ADD_OFFSET("Actor::baseTickVft", 26);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x788);
    ADD_OFFSET("Player::playerName", 0x1D18);
    ADD_OFFSET("Player::gamemode", 0xEB0);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x10);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0xC8);
    ADD_OFFSET("ClientInstance::guiData", 0x558);
    ADD_OFFSET("ClientInstance::Matrix1", 0x330);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F0);
    ADD_OFFSET("ClientInstance::getFovY", 0x704);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x1A8);

    ADD_OFFSET("Option::optionInformation", 0x8);
    ADD_OFFSET("Option::value", 0x10);
    ADD_OFFSET("Option::value1", 0x70);
    ADD_OFFSET("OptionInfo::TranslateName", 0x158);

    ADD_OFFSET("Item::AtlasTextureFile", 0x8);
    ADD_OFFSET("Item::Namespace", 0xF8);
    ADD_OFFSET("Item::name",0xD0);

    ADD_OFFSET("ItemStack::tag", 0x10);
    ADD_OFFSET("ItemStack::count", 0x22);

    ADD_OFFSET("Biome::temparature", 0x38);

    ADD_OFFSET("Dimension::weather", 0x1B0);

    ADD_OFFSET("Weather::lightingLevel", 0x44);
    ADD_OFFSET("Weather::rainLevel", 0x3C);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x308);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5FC);

    ADD_OFFSET("LoopbackPacketSender::networkSystem", 0x20);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x80);

    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x60);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x458);
    ADD_OFFSET("RaknetConnector::getPeer", 0x298);
    ADD_OFFSET("RakPeer::GetAveragePing", 44);

    ADD_OFFSET("BaseActorRenderContext::itemRenderer", 0x58);

    ADD_OFFSET("GuiData::ScreenSize", 0x30);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x40);
    ADD_OFFSET("GuiData::GuiScale", 0x4C);
    ADD_OFFSET("GuiData::screenResRounded", 0x38);
    ADD_OFFSET("GuiData::sliderAmount", 0x4C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x50);

    ADD_OFFSET("MinecraftUIRenderContext::clientInstance", 0x8);
    ADD_OFFSET("MinecraftUIRenderContext::screenContext", 0x10);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xB30);
    ADD_OFFSET("Level::getPlayerMap", 0x1C88);
}

void OffsetInit::init2070() {
    Logger::debug("[Offsets] Loading offsets for 1.20.7X");

    // Keystrokes
    ADD_OFFSET("MoveInputComponent::forward", 0x0A);
    ADD_OFFSET("MoveInputComponent::backward", 0x0B);
    ADD_OFFSET("MoveInputComponent::left", 0x0C);
    ADD_OFFSET("MoveInputComponent::right", 0x0D);
    ADD_OFFSET("MoveInputComponent::sneaking", 0x20);
    ADD_OFFSET("MoveInputComponent::jumping", 0x26);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x27);

    // Hitboxes and other
    ADD_OFFSET("Actor::getActorFlag", 0);
    ADD_OFFSET("Actor::isAlive", 53);
    ADD_OFFSET("Actor::isValidTarget", 61);

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 5);

    ADD_OFFSET("ClientInstance::getBlockSource", 28);
    ADD_OFFSET("ClientInstance::getLocalPlayer", 29);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("Packet::getId", 1);
    
    ADD_OFFSET("Actor::hurtTime", 0x214); // ?hurtEffects@Mob@@UEAAXAEBVActorDamageSource@@M_N1@Z Mob::hurtEffects 2nd after Actor::getHealth(void)
    ADD_OFFSET("Actor::level", 0x258);
    ADD_OFFSET("Actor::categories", 0x290);
    ADD_OFFSET("Actor::baseTickVft", 29);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x7B0);
    ADD_OFFSET("Player::playerName", 0x1D70);
    ADD_OFFSET("Player::gamemode", 0xED8);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x010);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0xC8);
    ADD_OFFSET("ClientInstance::guiData", 0x558);
    ADD_OFFSET("ClientInstance::Matrix1", 0x330);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F0);
    ADD_OFFSET("ClientInstance::getFovY", 0x704);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x1A8);

    ADD_OFFSET("Option::optionInformation", 0x8);
    ADD_OFFSET("Option::value", 0x10);
    ADD_OFFSET("Option::value1", 0x70);
    ADD_OFFSET("OptionInfo::TranslateName", 0x158);

    ADD_OFFSET("Item::AtlasTextureFile", 0x8);
    ADD_OFFSET("Item::Namespace", 0xF8);
    ADD_OFFSET("Item::name",0xD0);

    ADD_OFFSET("ItemStack::tag", 0x10);
    ADD_OFFSET("ItemStack::count", 0x22);

    ADD_OFFSET("Biome::temparature", 0x40);

    ADD_OFFSET("Dimension::weather", 0x1B0);

    ADD_OFFSET("Weather::lightingLevel", 0x44);
    ADD_OFFSET("Weather::rainLevel", 0x38);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x308);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5FC);

    ADD_OFFSET("LoopbackPacketSender::networkSystem", 0x20);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x80);

    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x60);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x438);
    ADD_OFFSET("RaknetConnector::getPeer", 0x278);
    ADD_OFFSET("RakPeer::GetAveragePing", 44);

    ADD_OFFSET("BaseActorRenderContext::itemRenderer", 0x58);

    ADD_OFFSET("GuiData::ScreenSize", 0x30);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x40);
    ADD_OFFSET("GuiData::GuiScale", 0x4C);
    ADD_OFFSET("GuiData::screenResRounded", 0x38);
    ADD_OFFSET("GuiData::sliderAmount", 0x4C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x50);

    ADD_OFFSET("MinecraftUIRenderContext::clientInstance", 0x8);
    ADD_OFFSET("MinecraftUIRenderContext::screenContext", 0x10);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xB18);
    ADD_OFFSET("Level::getPlayerMap", 0x1E98);
}

void OffsetInit::init2060() {
    Logger::debug("[Offsets] Loading offsets for 1.20.6X");

    // Keystrokes
    ADD_OFFSET("MoveInputComponent::forward", 0x0A);
    ADD_OFFSET("MoveInputComponent::backward", 0x0B);
    ADD_OFFSET("MoveInputComponent::left", 0x0C);
    ADD_OFFSET("MoveInputComponent::right", 0x0D);
    ADD_OFFSET("MoveInputComponent::sneaking", 0x20);
    ADD_OFFSET("MoveInputComponent::jumping", 0x26);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x27);

    // Hitboxes and other
    ADD_OFFSET("Actor::getActorFlag", 0);
    ADD_OFFSET("Actor::isAlive", 54);
    ADD_OFFSET("Actor::isValidTarget", 62);

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 5);

    ADD_OFFSET("ClientInstance::getBlockSource", 27);
    ADD_OFFSET("ClientInstance::getLocalPlayer", 28);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("Packet::getId", 1);

    ADD_OFFSET("Actor::hurtTime", 0x234);
    ADD_OFFSET("Actor::level", 0x290);
    ADD_OFFSET("Actor::categories", 0x2C8);
    ADD_OFFSET("Actor::baseTickVft", 30);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x7F0);
    ADD_OFFSET("Player::playerName", 0x1D40);
    ADD_OFFSET("Player::gamemode", 0xF18);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x010);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0x0C8);
    ADD_OFFSET("ClientInstance::guiData", 0x558);
    ADD_OFFSET("ClientInstance::Matrix1", 0x0330);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F0);
    ADD_OFFSET("ClientInstance::getFovY", 0x704);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x1A8);

    ADD_OFFSET("Option::optionInformation", 0x8);
    ADD_OFFSET("Option::value", 0x10);
    ADD_OFFSET("Option::value1", 0x70);
    ADD_OFFSET("OptionInfo::TranslateName", 0x158);

    ADD_OFFSET("Item::AtlasTextureFile", 0x8);
    ADD_OFFSET("Item::Namespace", 0xF8);
    ADD_OFFSET("Item::name",0xD0);

    ADD_OFFSET("ItemStack::tag", 0x10);
    ADD_OFFSET("ItemStack::count", 0x22);

    ADD_OFFSET("Biome::temparature", 0x40);

    ADD_OFFSET("Dimension::weather", 0x1A8);

    ADD_OFFSET("Weather::lightingLevel", 0x44);
    ADD_OFFSET("Weather::rainLevel", 0x38);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x308);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5E4);

    ADD_OFFSET("LoopbackPacketSender::networkSystem", 0x20);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x80);

    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x60);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x438);
    ADD_OFFSET("RaknetConnector::getPeer", 0x278);
    ADD_OFFSET("RakPeer::GetAveragePing", 44);

    ADD_OFFSET("BaseActorRenderContext::itemRenderer", 0x58);

    ADD_OFFSET("GuiData::ScreenSize", 0x30);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x40);
    ADD_OFFSET("GuiData::GuiScale", 0x4C);
    ADD_OFFSET("GuiData::screenResRounded", 0x38);
    ADD_OFFSET("GuiData::sliderAmount", 0x4C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x50);

    ADD_OFFSET("MinecraftUIRenderContext::clientInstance", 0x8);
    ADD_OFFSET("MinecraftUIRenderContext::screenContext", 0x10);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xA98);
    ADD_OFFSET("Level::getPlayerMap", 0x1E98);
}

void OffsetInit::init2050() {
    Logger::debug("[Offsets] Loading offsets for 1.20.5X");

    ADD_OFFSET("MoveInputComponent::forward", 0x0A);
    ADD_OFFSET("MoveInputComponent::backward", 0x0B);
    ADD_OFFSET("MoveInputComponent::left", 0x0C);
    ADD_OFFSET("MoveInputComponent::right", 0x0D);
    ADD_OFFSET("MoveInputComponent::sneaking", 0x20);
    ADD_OFFSET("MoveInputComponent::jumping", 0x26);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x27);

    // Hitboxes and other
    ADD_OFFSET("Actor::getActorFlag", 0);
    ADD_OFFSET("Actor::isAlive", 57);
    ADD_OFFSET("Actor::isValidTarget", 65);

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 5);

    ADD_OFFSET("ClientInstance::getBlockSource", 27);
    ADD_OFFSET("ClientInstance::getLocalPlayer", 28);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F8);
    ADD_OFFSET("ClientInstance::getFovY", 0x70C);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Packet::getId", 1);

    ADD_OFFSET("Actor::hurtTime", 0x22C);
    ADD_OFFSET("Actor::level", 0x288);
    ADD_OFFSET("Actor::categories", 0x2C0);
    ADD_OFFSET("Actor::baseTickVft", 30);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x7E8);
    ADD_OFFSET("Player::playerName", 0x1D28);
    ADD_OFFSET("Player::gamemode", 0xF10);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x010);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0x0C8);
    ADD_OFFSET("ClientInstance::guiData", 0x560);
    ADD_OFFSET("ClientInstance::Matrix1", 0x0330);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x190);

    ADD_OFFSET("Option::optionInformation", 0x8);
    ADD_OFFSET("Option::value", 0x10);
    ADD_OFFSET("Option::value1", 0x70);
    ADD_OFFSET("OptionInfo::TranslateName", 0x158);

    ADD_OFFSET("Item::AtlasTextureFile", 0x8);
    ADD_OFFSET("Item::Namespace", 0xF8);
    ADD_OFFSET("Item::name",0xD0);

    ADD_OFFSET("ItemStack::tag", 0x10);
    ADD_OFFSET("ItemStack::count", 0x22);

    ADD_OFFSET("Biome::temparature", 0x40);

    ADD_OFFSET("Dimension::weather", 0x1A8);

    ADD_OFFSET("Weather::lightingLevel", 0x44);
    ADD_OFFSET("Weather::rainLevel", 0x38);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x308);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5E4);

    ADD_OFFSET("LoopbackPacketSender::networkSystem", 0x20);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x80);

    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x60);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x438);
    ADD_OFFSET("RaknetConnector::getPeer", 0x278);
    ADD_OFFSET("RakPeer::GetAveragePing", 42);

    ADD_OFFSET("BaseActorRenderContext::itemRenderer", 0x58);

    ADD_OFFSET("GuiData::ScreenSize", 0x30);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x40);
    ADD_OFFSET("GuiData::GuiScale", 0x4C);
    ADD_OFFSET("GuiData::screenResRounded", 0x38);
    ADD_OFFSET("GuiData::sliderAmount", 0x4C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x50);

    ADD_OFFSET("MinecraftUIRenderContext::clientInstance", 0x8);
    ADD_OFFSET("MinecraftUIRenderContext::screenContext", 0x10);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xA48);
    ADD_OFFSET("Level::getPlayerMap", 0x1EA8);
}

void OffsetInit::init2040() {
    // getGamma, combo &
    Logger::debug("[Offsets] Loading offsets for 1.20.4X"); // quite some offsets are wrong

    ADD_OFFSET("MoveInputComponent::forward", 0x0A);
    ADD_OFFSET("MoveInputComponent::backward", 0x0B);
    ADD_OFFSET("MoveInputComponent::left", 0x0C);
    ADD_OFFSET("MoveInputComponent::right", 0x0D);
    ADD_OFFSET("MoveInputComponent::sneaking", 0x20);
    ADD_OFFSET("MoveInputComponent::jumping", 0x26);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x27);

    // Hitboxes and other
    ADD_OFFSET("Actor::getActorFlag", 0);
    ADD_OFFSET("Actor::isAlive", 58);
    ADD_OFFSET("Actor::isValidTarget", 66);

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 5);

    ADD_OFFSET("ClientInstance::getBlockSource", 26);
    ADD_OFFSET("ClientInstance::getLocalPlayer", 27);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F8);
    ADD_OFFSET("ClientInstance::getFovY", 0x70C);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Packet::getId", 1);

    ADD_OFFSET("Actor::hurtTime", 0x204);
    ADD_OFFSET("Actor::level", 0x260);
    ADD_OFFSET("Actor::categories", 0x298);
    ADD_OFFSET("Actor::baseTickVft", 30);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x7C0);
    ADD_OFFSET("Player::playerName", 0x1CB8);
    ADD_OFFSET("Player::gamemode", 0xEB0);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x10);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0xC8);
    ADD_OFFSET("ClientInstance::guiData", 0x560);
    ADD_OFFSET("ClientInstance::Matrix1", 0x330);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x190);

    ADD_OFFSET("Option::optionInformation", 0x8);
    ADD_OFFSET("Option::value", 0x10);
    ADD_OFFSET("Option::value1", 0x70);
    ADD_OFFSET("OptionInfo::TranslateName", 0x158);

    ADD_OFFSET("Item::AtlasTextureFile", 0x8);
    ADD_OFFSET("Item::Namespace", 0xF8);
    ADD_OFFSET("Item::name",0xD0);

    ADD_OFFSET("ItemStack::tag", 0x10);
    ADD_OFFSET("ItemStack::count", 0x20);

    ADD_OFFSET("Biome::temparature", 0x40);

    ADD_OFFSET("Dimension::weather", 0x1A8);

    ADD_OFFSET("Weather::lightingLevel", 0x48);
    ADD_OFFSET("Weather::rainLevel", 0x3C);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x308);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5E4);

    ADD_OFFSET("LoopbackPacketSender::networkSystem", 0x20);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x60);

    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x60);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x438);
    ADD_OFFSET("RaknetConnector::getPeer", 0x278);
    ADD_OFFSET("RakPeer::GetAveragePing", 42);

    ADD_OFFSET("BaseActorRenderContext::itemRenderer", 0x58);

    ADD_OFFSET("GuiData::ScreenSize", 0x30);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x40);
    ADD_OFFSET("GuiData::GuiScale", 0x4C);
    ADD_OFFSET("GuiData::screenResRounded", 0x38);
    ADD_OFFSET("GuiData::sliderAmount", 0x4C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x50);

    ADD_OFFSET("MinecraftUIRenderContext::clientInstance", 0x8);
    ADD_OFFSET("MinecraftUIRenderContext::screenContext", 0x10);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xA68);
    ADD_OFFSET("Level::getPlayerMap", 0x25F0);
}

void OffsetInit::init2030() {
    Logger::debug("[Offsets] Loading offsets for 1.20.3X"); // quite some offsets are wrong

    ADD_OFFSET("MoveInputComponent::forward", 0x0A);
    ADD_OFFSET("MoveInputComponent::backward", 0x0B);
    ADD_OFFSET("MoveInputComponent::left", 0x0C);
    ADD_OFFSET("MoveInputComponent::right", 0x0D);
    ADD_OFFSET("MoveInputComponent::sneaking", 0x20);
    ADD_OFFSET("MoveInputComponent::jumping", 0x26);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x27);

    // Hitboxes and other
    ADD_OFFSET("Actor::getActorFlag", 0);
    ADD_OFFSET("Actor::isAlive", 89);
    ADD_OFFSET("Actor::isValidTarget", 65);

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 5);

    ADD_OFFSET("ClientInstance::getBlockSource", 26);
    ADD_OFFSET("ClientInstance::getLocalPlayer", 27);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F8);
    ADD_OFFSET("ClientInstance::getFovY", 0x70C);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Packet::getId", 1);

    ADD_OFFSET("Actor::hurtTime", 0x22C);
    ADD_OFFSET("Actor::level", 0x260);
    ADD_OFFSET("Actor::categories", 0x298);
    ADD_OFFSET("Actor::baseTickVft", 44);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x7C0);
    ADD_OFFSET("Player::playerName", 0x1C78);
    ADD_OFFSET("Player::gamemode", 0xE70);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x010);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0xC8);
    ADD_OFFSET("ClientInstance::guiData", 0x560);
    ADD_OFFSET("ClientInstance::Matrix1", 0x330);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x188);

    ADD_OFFSET("Option::optionInformation", 0x8);
    ADD_OFFSET("Option::value", 0x10);
    ADD_OFFSET("Option::value1", 0x70);
    ADD_OFFSET("OptionInfo::TranslateName", 0x158);

    ADD_OFFSET("Item::AtlasTextureFile", 0x8);
    ADD_OFFSET("Item::Namespace", 0xF8);
    ADD_OFFSET("Item::name",0xD0);

    ADD_OFFSET("ItemStack::tag", 0x10);
    ADD_OFFSET("ItemStack::count", 0x20);

    ADD_OFFSET("Biome::temparature", 0x40);

    ADD_OFFSET("Dimension::weather", 0x1A8);

    ADD_OFFSET("Weather::lightingLevel", 0x44);
    ADD_OFFSET("Weather::rainLevel", 0x38);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x308);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5E4);

    ADD_OFFSET("LoopbackPacketSender::networkSystem", 0x20);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x60);

    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x60);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x438);
    ADD_OFFSET("RaknetConnector::getPeer", 0x278);
    ADD_OFFSET("RakPeer::GetAveragePing", 42);

    ADD_OFFSET("BaseActorRenderContext::itemRenderer", 0x58);

    ADD_OFFSET("GuiData::ScreenSize", 0x30);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x40);
    ADD_OFFSET("GuiData::GuiScale", 0x4C);
    ADD_OFFSET("GuiData::screenResRounded", 0x38);
    ADD_OFFSET("GuiData::sliderAmount", 0x4C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x50);

    ADD_OFFSET("MinecraftUIRenderContext::clientInstance", 0x8);
    ADD_OFFSET("MinecraftUIRenderContext::screenContext", 0x10);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xA48);
    ADD_OFFSET("Level::getPlayerMap", 0x2608);
}
