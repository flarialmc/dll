#include "OffsetInit.hpp"

#include "../../../Logger/Logger.hpp"
#include "../SignatureAndOffsetManager.hpp"

void OffsetInit::init2190() {
    Logger::debug("[Offsets] Loading offsets for 1.21.9X");

    ADD_OFFSET("Actor::baseTickVft", 25);

    ADD_OFFSET("Level::hitResult", 0x1E0);
    ADD_OFFSET("Level::getPlayerMap", 0x4E0);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x3F0);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x664);
}

void OffsetInit::init2180() {
    Logger::debug("[Offsets] Loading offsets for 1.21.8X");
    ADD_OFFSET("ClientInstance::getBlockSource", 31);

    ADD_OFFSET("ClientInstance::guiData", 0x5B8);
    ADD_OFFSET("ClientInstance::getFovX", 0x740);
    ADD_OFFSET("ClientInstance::getFovY", 0x754);

    ADD_OFFSET("RaknetConnector::getPeer", 0x48);
    ADD_OFFSET("RaknetConnector::JoinedIp", 0x3E8);

    ADD_OFFSET("Player::gamemode", 0xA88);
    ADD_OFFSET("Player::playerName", 0xC08);
    ADD_OFFSET("Player::playerInventory", 0x5C8);

    ADD_OFFSET("Level::hitResult", 0x250);
    ADD_OFFSET("Level::getPlayerMap", 0x960);
}

void OffsetInit::init2170() {
    Logger::debug("[Offsets] Loading offsets for 1.21.7X");

    ADD_OFFSET("ClientInstance::viewMatrix", 0x388);
    ADD_OFFSET("ClientInstance::guiData", 0x5B0);
    ADD_OFFSET("ClientInstance::getFovX", 0x748);
    ADD_OFFSET("ClientInstance::getFovY", 0x75C);

    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x65C);

    ADD_OFFSET("Player::gamemode", 0xAD0);
    ADD_OFFSET("Player::playerName", 0xC50);
    ADD_OFFSET("Player::playerInventory", 0x5C0);

    ADD_OFFSET("Level::hitResult", 0x240);
    ADD_OFFSET("Level::getPlayerMap", 0xB98);
}

void OffsetInit::init2160() {
    Logger::debug("[Offsets] Loading offsets for 1.21.6X");

    ADD_OFFSET("Player::gamemode", 0xAD8);
    ADD_OFFSET("Player::playerName", 0xC58);
    ADD_OFFSET("Player::playerInventory", 0x5C8);

    ADD_OFFSET("GuiData::ScreenSize", 0x40);
    ADD_OFFSET("GuiData::ScreenSizeScaled", 0x50);
    ADD_OFFSET("GuiData::GuiScale", 0x5C);
    ADD_OFFSET("GuiData::screenResRounded", 0x48);
    ADD_OFFSET("GuiData::sliderAmount", 0x5C);
    ADD_OFFSET("GuiData::scalingMultiplier", 0x60);

    ADD_OFFSET("Item::AtlasTextureFile", 0xB0);
    ADD_OFFSET("Item::Namespace", 0x100);
    ADD_OFFSET("Item::name",0xD8);

    ADD_OFFSET("Level::getPlayerMap", 0xB68);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x328);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x610);
}

void OffsetInit::init2150() {
    Logger::debug("[Offsets] Loading offsets for 1.21.5X");

    ADD_OFFSET("Actor::hurtTime", 0x19C);
    ADD_OFFSET("Actor::level", 0x1D8);
    ADD_OFFSET("Actor::categories", 0x210);

    ADD_OFFSET("Player::gamemode", 0xB18);
    ADD_OFFSET("Player::playerName", 0xCA0);

    ADD_OFFSET("Player::playerInventory", 0x5D0);

    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x6E4);

    ADD_OFFSET("MoveInputComponent::forward", 0xD);
    ADD_OFFSET("MoveInputComponent::backward", 0xE);
    ADD_OFFSET("MoveInputComponent::left", 0xF);
    ADD_OFFSET("MoveInputComponent::right", 0x10);

    ADD_OFFSET("MoveInputComponent::sneaking", 0x28);
    ADD_OFFSET("MoveInputComponent::jumping", 0x2F);
    ADD_OFFSET("MoveInputComponent::sprinting", 0x30);
}

void OffsetInit::init2140() {
    Logger::debug("[Offsets] Loading offsets for 1.21.4X");
    ADD_OFFSET("Level::hitResult", 0x248);

    ADD_OFFSET("Player::gamemode", 0xB28);
    ADD_OFFSET("Player::playerName", 0xCB0);
    ADD_OFFSET("ClientInstance::getBlockSource", 29);

    ADD_OFFSET("ClientInstance::levelRenderer", 0xE8);

    //ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x0); No longer exists?
    ADD_OFFSET("Level::getPlayerMap", 0xBF0);

    ADD_OFFSET("ClientInstance::viewMatrix", 0x368);
    ADD_OFFSET("ClientInstance::guiData", 0x590);

    ADD_OFFSET("ClientInstance::getFovX", 0x728);
    ADD_OFFSET("ClientInstance::getFovY", 0x73C);

    ADD_OFFSET("RaknetConnector::getPeer", 0x2A0);
    ADD_OFFSET("RaknetConnector::JoinedIp", 0x398);

    ADD_OFFSET("LevelRender::getLevelRendererPlayer", 0x318);
    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x620);

    ADD_OFFSET("ClientInstance::getPacketSender", 0xF8);
    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x90);
    ADD_OFFSET("RemoteConnectorComposite::rakNetConnector", 0x70);
}

void OffsetInit::init2130() {
    Logger::debug("[Offsets] Loading offsets for 1.21.3X");

    ADD_OFFSET("Player::gamemode", 0xB18);
    ADD_OFFSET("Player::playerName", 0xCA0);

    ADD_OFFSET("Level::hitResult", 0x230);
    ADD_OFFSET("Level::getPlayerMap", 0xC08);

    ADD_OFFSET("ClientInstance::camera", 0x2A0);
    ADD_OFFSET("ClientInstance::viewMatrix", 0x360);
    ADD_OFFSET("ClientInstance::getFovX", 0x720);
    ADD_OFFSET("ClientInstance::getFovY", 0x734);

    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x614);

    ADD_OFFSET("ClientInstance::guiData", 0x588);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x1A0);
    ADD_OFFSET("MinecraftGame::textureGroup", 0x690);

    ADD_OFFSET("MoveInputComponent::forward", 0x2C);
    ADD_OFFSET("MoveInputComponent::backward", 0x2D);
    ADD_OFFSET("MoveInputComponent::left", 0x2E);
    ADD_OFFSET("MoveInputComponent::right", 0x2F);
}

void OffsetInit::init2120() {
    Logger::debug("[Offsets] Loading offsets for 1.21.2X");

    ADD_OFFSET("Actor::hurtTime", 0x1F4);
    ADD_OFFSET("Actor::level", 0x230);
    ADD_OFFSET("Actor::categories", 0x268);

    ADD_OFFSET("Player::playerInventory", 0x628);
    ADD_OFFSET("Player::playerName", 0xC88);
    ADD_OFFSET("Player::gamemode", 0xB00);

    ADD_OFFSET("Actor::baseTickVft", 24);

    ADD_OFFSET("PlayerInventory::inventory", 0xB8);

    ADD_OFFSET("UIControl::x", 0x10);
    ADD_OFFSET("UIControl::y", 0x14);
    ADD_OFFSET("UIControl::LayerName", 0x20);
    ADD_OFFSET("UIControl::scale", 0x48);

    ADD_OFFSET("UIControl::children", 0x90);
    ADD_OFFSET("UIControl::components", 0xB0);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x390);

    ADD_OFFSET("Level::hitResult", 0x220);
    ADD_OFFSET("Level::getPlayerMap", 0xBF8);

    ADD_OFFSET("ItemActor::stack", 0x408);

    ADD_OFFSET("MinecraftGame::textureGroup", 0x6D0);
}

void OffsetInit::init2100() {
    Logger::debug("[Offsets] Loading offsets for 1.21.0X");

    ADD_OFFSET("Inventory::getItem", 7);

    ADD_OFFSET("Actor::hurtTime", 0x20C);
    ADD_OFFSET("Actor::level", 0x250);
    ADD_OFFSET("Actor::categories", 0x288);

    ADD_OFFSET("Player::playerInventory", 0x760);
    ADD_OFFSET("Player::playerName", 0x1D30);
    ADD_OFFSET("Player::gamemode", 0xEC8);

    ADD_OFFSET("Weather::lightingLevel", 0x48);

    ADD_OFFSET("Level::hitResult", 0xB38);
    ADD_OFFSET("Level::getPlayerMap", 0x1BC8); // getRuntimeActorList offset + B8 || Level::getPlayerList

    ADD_OFFSET("ItemActor::stack", 0x448);
}

void OffsetInit::init2080() {
    Logger::debug("[Offsets] Loading offsets for 1.20.8X");

    ADD_OFFSET("Actor::baseTickVft", 26);

    ADD_OFFSET("Player::playerInventory",  0x788);
    ADD_OFFSET("Player::playerName", 0x1D18);
    ADD_OFFSET("Player::gamemode", 0xEB0);

    ADD_OFFSET("Biome::temparature", 0x38);

    ADD_OFFSET("Weather::rainLevel", 0x3C);

    ADD_OFFSET("RaknetConnector::JoinedIp", 0x458);
    ADD_OFFSET("RaknetConnector::getPeer", 0x298);

    ADD_OFFSET("Level::hitResult", 0xB30);
    ADD_OFFSET("Level::getPlayerMap", 0x1C88);

    ADD_OFFSET("ItemActor::stack", 0x470);
}

void OffsetInit::init2070() {
    Logger::debug("[Offsets] Loading offsets for 1.20.7X");

    ADD_OFFSET("ClientInstance::getBlockSource", 28);

    ADD_OFFSET("Actor::hurtTime", 0x214);
    ADD_OFFSET("Actor::level", 0x258);
    ADD_OFFSET("Actor::categories", 0x290);

    ADD_OFFSET("Actor::baseTickVft", 29);

    ADD_OFFSET("Player::playerInventory", 0x7B0);
    ADD_OFFSET("Player::playerName", 0x1D70);
    ADD_OFFSET("Player::gamemode", 0xED8);

    ADD_OFFSET("Dimension::weather", 0x1B0);

    ADD_OFFSET("LevelRendererPlayer::cameraPos", 0x5FC);

    ADD_OFFSET("Level::hitResult", 0xB18);

    ADD_OFFSET("ItemActor::stack", 0x498);
}

void OffsetInit::init2060() {
    Logger::debug("[Offsets] Loading offsets for 1.20.6X");

    ADD_OFFSET("Actor::hurtTime", 0x234);
    ADD_OFFSET("Actor::level", 0x290);
    ADD_OFFSET("Actor::categories", 0x2C8);

    ADD_OFFSET("Player::playerInventory", 0x7F0);
    ADD_OFFSET("Player::playerName", 0x1D40);
    ADD_OFFSET("Player::gamemode", 0xF18);

    ADD_OFFSET("ClientInstance::guiData", 0x558);
    ADD_OFFSET("ClientInstance::getFovX", 0x6F0);
    ADD_OFFSET("ClientInstance::getFovY", 0x704);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x1A8);
    ADD_OFFSET("MinecraftGame::textureGroup", 0x828);

    ADD_OFFSET("RakPeer::GetAveragePing", 44);

    ADD_OFFSET("Level::hitResult", 0xA98);
    ADD_OFFSET("Level::getPlayerMap", 0x1E98);

    ADD_OFFSET("ItemActor::stack", 0x4D0);
}

void OffsetInit::init2050() {
    Logger::debug("[Offsets] Loading offsets for 1.20.5X");

    ADD_OFFSET("ClientInstance::getBlockSource", 27);

    ADD_OFFSET("Actor::hurtTime", 0x22C);
    ADD_OFFSET("Actor::level", 0x288);
    ADD_OFFSET("Actor::categories", 0x2C0);

    ADD_OFFSET("Player::playerInventory", 0x7E8);
    ADD_OFFSET("Player::playerName", 0x1D28);
    ADD_OFFSET("Player::gamemode", 0xF10);

    ADD_OFFSET("ItemStack::count", 0x22);

    ADD_OFFSET("Weather::lightingLevel", 0x44);
    ADD_OFFSET("Weather::rainLevel", 0x38);

    ADD_OFFSET("NetworkSystem::remoteConnectorComposite", 0x80);

    ADD_OFFSET("Level::hitResult", 0xA48);
    ADD_OFFSET("Level::getPlayerMap", 0x1EA8);

    ADD_OFFSET("ItemActor::stack", 0x4C8);

    ADD_OFFSET("OptionInfo::TranslateName", 0x158);
}

void OffsetInit::init2040() {
    // getGamma, combo &
    Logger::debug("[Offsets] Loading offsets for 1.20.4X");

    ADD_OFFSET("Actor::hurtTime", 0x204);
    ADD_OFFSET("Actor::baseTickVft", 30);

    ADD_OFFSET("Player::playerName", 0x1CB8);
    ADD_OFFSET("Player::gamemode", 0xEB0);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x190);
    ADD_OFFSET("MinecraftGame::textureGroup", 0x818);

    ADD_OFFSET("Weather::lightingLevel",  0x48);
    ADD_OFFSET("Weather::rainLevel", 0x3C);

    ADD_OFFSET("Level::hitResult", 0xA68);
    ADD_OFFSET("Level::getPlayerMap", 0x25F0);

    ADD_OFFSET("OptionInfo::TranslateName", 0x168);
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

    // Armour HUD, Inventory HUD
    ADD_OFFSET("Inventory::getItem", 5);

    ADD_OFFSET("ClientInstance::getBlockSource", 26);
    ADD_OFFSET("ClientInstance::levelRenderer", 0xE0);

    ADD_OFFSET("ClientInstance::getFovX", 0x6F8);
    ADD_OFFSET("ClientInstance::getFovY", 0x70C);
    ADD_OFFSET("ClientInstance::getPacketSender", 0xF0);

    ADD_OFFSET("Packet::getId", 1);

    ADD_OFFSET("Actor::hurtTime", 0x204); // ?hurtEffects@Mob@@UEAAXAEBVActorDamageSource@@M_N1@Z Mob::hurtEffects 2nd after Actor::getHealth(void)
    ADD_OFFSET("Actor::level", 0x260);
    ADD_OFFSET("Actor::categories", 0x298);
    ADD_OFFSET("Actor::baseTickVft", 44);

    ADD_OFFSET("Gamemode::player", 0x8);
    ADD_OFFSET("Gamemode::lastBreakProgress", 0x20);
    ADD_OFFSET("Gamemode::attackVft", 14);

    ADD_OFFSET("Player::playerInventory", 0x7C0);
    ADD_OFFSET("Player::playerName", 0x1C78); // 48 89 5C 24 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 C0 48 81 EC 40 01 00 00 41 | line 278 | book.defaultAuthor
    ADD_OFFSET("Player::gamemode", 0xE70);

    ADD_OFFSET("BlockSource::dimension", 0x30);

    ADD_OFFSET("PlayerInventory::SelectedSlot", 0x010);
    ADD_OFFSET("PlayerInventory::inventory", 0xC0);

    ADD_OFFSET("ClientInstance::minecraftGame", 0xC8);
    ADD_OFFSET("ClientInstance::guiData", 0x560);
    ADD_OFFSET("ClientInstance::camera", 0x270);
    ADD_OFFSET("ClientInstance::viewMatrix", 0x330);

    ADD_OFFSET("Minecraft::timer", 0xD8);

    ADD_OFFSET("MinecraftGame::mouseGrabbed", 0x188);
    ADD_OFFSET("MinecraftGame::textureGroup", 0x810);

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
    ADD_OFFSET("MinecraftUIRenderContext::textures", 0x48);

    ADD_OFFSET("ScreenContext::colorHolder", 0x30);
    ADD_OFFSET("ScreenContext::tessellator", 0xC0);

    ADD_OFFSET("createMaterial", 1);

    ADD_OFFSET("TextureGroup::base", 0x18);
    ADD_OFFSET("TextureGroup::loadedTextures", 0x178);

    ADD_OFFSET("ScreenView::VisualTree", 0x48);

    ADD_OFFSET("MinecraftCustomUIRenderer::state", 0x10);

    ADD_OFFSET("CustomRenderComponent::renderer", 0x18);

    ADD_OFFSET("UIControl::LayerName", 0x18);
    ADD_OFFSET("UIControl::scale", 0x40);
    ADD_OFFSET("UIControl::x", 0x78);
    ADD_OFFSET("UIControl::y", 0x7C);
    ADD_OFFSET("UIControl::children", 0xA0);
    ADD_OFFSET("UIControl::components", 0xC0);

    ADD_OFFSET("VisualTree::root", 0x8);

    ADD_OFFSET("Level::hitResult", 0xA48);
    ADD_OFFSET("Level::getPlayerMap", 0x2608);

    ADD_OFFSET("ItemActor::stack", 0x4A0);

    ADD_OFFSET("NameTagRenderObject::nameTag", 0x0);
    ADD_OFFSET("NameTagRenderObject::pos", 0x60);

    ADD_OFFSET("ViewRenderData::cameraPos", 0x0);
    ADD_OFFSET("ViewRenderData::cameraTargetPos", 0xC);

    ADD_OFFSET("Font::getLineLength", 6);
    ADD_OFFSET("Font::getLineHeight", 7);

    ADD_OFFSET("mce::Camera::worldMatrixStack", 0x40);
}
