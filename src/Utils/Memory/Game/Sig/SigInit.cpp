#include "SigInit.hpp"
#include "../../../Logger/Logger.hpp"

#include "../SignatureAndOffsetManager.hpp"

void SigInit::init2140() {
    Logger::debug("[Signatures] Loading sigs for 1.21.40");

    ADD_SIG("tryGetPrefix", "48 89 5C 24 ? 57 48 83 EC 30 8B ?");
    ADD_SIG("tryGetPrefix2", "4c 8b 41 ? 4c 8b d1 48 8b 41 ? 4c 8b 49 ? 49 2b c0 8b 12 48 c1 f8 ? 48 ff");

    ADD_SIG("mce::Color::BLACK", "? ? ? ? 0F 11 00 C6 40 10 ? 0F 57 C9");

    ADD_SIG("Font::drawTransformed", "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 41 54 41 55 41 56 41 57 48 81 EC B0 01");

    ADD_SIG("CameraYaw", "F3 0F 11 00 F3 0F 11 70 04"); // prevent render angle change 00 00 00 00 00 F3 41 0F 5C C1
    ADD_SIG("CameraYaw2", "F3 0F 11 10 48 8B 8F 30 02 00 00"); // head rot
    ADD_SIG("_updatePlayer", "48 89 5C 24 08 57 48 83 EC 70 0F 29 74 24 60 49"); // sets angles F3 44 0F 11 52 04 8B 01 | F3 0F 11 10 48 8B 8F 30 02 00 00 | f3 41 0f 58 c0 89 02 | F3 0F 11 37 0F 28 74 24 60

    DEPRECATE_SIG("CameraMovement");
    DEPRECATE_SIG("CameraPitch"); // F3 0F 11 37 0F 28 74 24 60

    ADD_SIG("ClientInstance::grabMouse", "48 8B 80 ? ? ? ? FF 15 ? ? ? ? 90 48 85 DB 74 37 8B C6 F0 0F C1 43 08 83 F8 01 75 2B 48 8B 03 48 8B CB 48 8B 00 FF 15 ? ? ? ? 8B C6 F0 0F C1 43 0C 83 F8 01 75 10 48 8B 03 48 8B CB 48 8B 40 08 FF 15 ? ? ? ? 48 8B 8F E8 0C 00 00");

    ADD_SIG("RaknetTick", "4C 8B DC 49 89 5B 10 49 89 6B 18 56 57 41 56 48 81 EC 10");

    ADD_SIG("Actor::getActorGameTypeComponent", "10 BA DE AB CB AF");
    ADD_SIG("Actor::getAABBShapeComponent", "C8 25 F2 C9 10 1B"); // 42 81 7C 09 08 + hash
    ADD_SIG("Actor::getStateVectorComponent", "C8 25 91 3C C9 0E");

    ADD_SIG("Actor::getMobEffectsComponent", "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 4C 89 6C 24 20 55 41 56 41 57 48 8D 6C 24 B9 48 81 EC E0"); // 10 BA 2F B4 D6 F7
    DEPRECATE_SIG("Actor::getRuntimeIDComponent"); // 10 BA 14 14 A1 3C ??
    ADD_SIG("Actor::getMoveInputHandler", "C8 25 2E CD 8B 46");
    // 49 2B C0 8B 12 48 C1 F8 03 48 FF C8 49 23 C3 49 8B 0C C0 48 83 F9 FF 74 1A 66 90 48 C1 E1 05 46 39 5C 09 08 4A 8D 04 09 74 0D 48 8B 08 48 83 F9 FF 75 E8
    ADD_SIG("Actor::getRenderPositionComponent", "4C 8B 41 48 41 BB 6E F3 E8 D4"); // 6E F3 E8 D4

    ADD_SIG("ThirdPersonNametag", "0F 84 B5 05 00 00 49 8B 07 49 8B CF 48 8B 80 00 01 00 00");

    ADD_SIG("Tessellator::begin", "40 53 55 48 83 EC 28 80 B9");
    ADD_SIG("MeshHelpers::renderMeshImmediately", "40 53 55 56 57 41 56 48 81 EC 20 04 00 00 49 8B F1 4D 8B F0 48 8B FA 48 8B E9 80 BA 0D 02 00 00 00 0F 85 2E");
    ADD_SIG("MeshHelpers::renderMeshImmediately2", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 98 FC FF FF 48 81 EC 68 04 00 00 49"); // bruh it changed

    // entity_alphatest_change_color_glint to vtable then to ItemRenderer::render
    DEPRECATE_SIG("glm_rotate");
    ADD_SIG("glm_rotateRef", "E8 ? ? ? ? 48 8B D0 8B 08 89 0B 8B 48 04 89 4B 04 8B 48 08 89 4B 08 8B 48 0C 89 4B 0C 8B 48 10 89 4B 10 8B 48 14 89 4B 14 8B 40 18 89 43 18 8B 42 1C 89 43 1C 8B 42 20 89 43 20 8B 42 24 89 43 24 8B 42 28 89 43 28 8B 42 2C 89 43 2C 8B 42 30 89 43 30 8B 42 34 89 43 34 8B 42 38 89 43 38 8B 42 3C 89 43 3C 0F B6 47 22");
    ADD_SIG("glm_translateRef", "E8 ? ? ? ? 48 8D 15 ? ? ? ? 0F 28 35");
    ADD_SIG("glm_translateRef2", "E8 ? ? ? ? C6 46 38 01 F3 0F 11 74 24 20 F3 0F 10 1D");

    ADD_SIG("ItemPositionConst", "F3 0F 10 15 ? ? ? ? F3 0F 59 C2 F3 41 0F 58 C1");

    ADD_SIG("MinecraftUIRenderContext::getTexture", "");

    ADD_SIG("ItemStack::getDamageValue", "40 53 48 83 EC 30 48 8B 51 08 33");

    ADD_SIG("ItemStack::isEnchanted", "48 83 EC 38 48 8B 49 10 48 85 C9 74 60"); // variable.is_enchanted + 48 83 EC 38 48 8B 49 10 48 85 C9 74 ? (last)

    ADD_SIG("Dimension::getBrightnessDependentFogColor", "41 0F 10 00 48 8B C2 0F"); // "A dimension task group" (vtable ref)

    ADD_SIG("Actor::setNametag", "48 89 5C 24 18 48 89 74 24 20 57 48 83 EC 60 48 8B F2 48 8B F9 48 8B");

    ADD_SIG("BlockSource::getBiome", "48 89 5C 24 18 57 48 83 EC 60 48 8B DA 48 8B F9 48 8B"); // world_loading_progress_screen sub_14XXXXXX(arg) + 16;

    ADD_SIG("MinecraftPackets::createPacket", "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 48 81 EC A0 00 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 98 00 00 00 48 8B F9");

    ADD_SIG("Options::getSensitivity", "48 83 EC 18 48 8B 41 08 4C 8B C9"); // "AutoUpdate" / "gameLocale" / "Options" has ref to it

    ADD_SIG("blockHighlightColor", "? ? ? ? 0F 11 85 78 02 00 00 0F 11 00");

    ADD_SIG("HurtColor", "E8 ?? ?? ?? ?? 0F 28 05 ?? ?? ?? ?? 0F 11 85 68 02 00 00");

    ADD_SIG("Level::getRuntimeActorList", "48 89 5C 24 18 55 56 57 48 83 EC 40 48 8B FA 48 89 54 24 30");

    ADD_SIG("Actor::baseTick", "48 8D 05 ? ? ? ? 48 89 01 ? ? ? ? 00 44 8D 4A 04 44 8D 42 02 66 C7 44 24 20 37 00 E8 ? ? ? ? 48 8B 8F 80 11 00 00"); // 8D 4A 04 44 8D 42 02 66

    ADD_SIG("BaseActorRenderer::renderText", "E8 ? ? ? ? 48 83 C3 ? 48 3B DF 75 ? 48 8B 74 24 ? 48 8B 5C 24 ? 48 8B 6C 24");

    DEPRECATE_SIG("HitResult::getEntity");

    ADD_SIG("HitResult::_getEntity", "40 55 57 41 54 41 56 41 57 48 83"); // params - weak ent ref at hitres+0x38 and 1 (true)

    ADD_SIG("ScreenView::setupAndRender", "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 98 FD");
}

void SigInit::init2130() {
    Logger::debug("[Signatures] Loading sigs for 1.21.30");
    ADD_SIG("Actor::baseTick", "48 8D 05 ? ? ? ? 48 89 01 BA 31 00 00 00 44 8D 4A 04 44 8D 42 02 66 C7 44 24 20 37 00 E8 ? ? ? ? 48 8B 8B 70 11 00 00");
    ADD_SIG("ItemRenderer::renderGuiItemNew", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 D8 48 81 EC 28 01 00 00 0F 29 B4 24 10 01 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 45 00 44");

    ADD_SIG("MinecraftGame::onAppSuspended", "48 89 5c 24 ? 48 89 74 24 ? 48 89 7c 24 ? 55 41 54 41 55 41 56 41 57 48 8d ac 24 ? ? ? ? 48 81 ec ? ? ? ? ? 8b ? 45 33 f6");

    ADD_SIG("ThirdPersonNametag", "0F 84 ? ? ? ? 49 8B D4 49 8B CD");

    ADD_SIG("tryGetPrefix3", "40 53 48 83 EC 40 48 8B");

    ADD_SIG("Actor::canSee", "E8 ? ? ? ? 84 C0 74 1C 48 8B 4B 48");

    ADD_SIG("HurtColor", "E8 ? ? ? ? E9 FD 02 00 00 8B 46 18");

    ADD_SIG("CameraAssignAngle", "F3 0F 11 43 08 C6 43 0C 01");

    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 50");

    ADD_SIG("ItemStack::getMaxDamage", "48 83 EC 28 48 8B 41 08 33 D2 48 85 C0 74 08 48 39 10 0F 94 C1 EB 05 B9 01 00 00 00 84 C9 75 20");

    ADD_SIG("Tessellator::begin", "48 89 5C 24 ? 57 48 83 EC ? 80 B9 ? ? ? ? ? 45 0F B6");

    ADD_SIG("MeshHelpers::renderMeshImmediately", "E8 ? ? ? ? C6 47 38 ? F3 0F 10 ? ? ? ? ? 0F 57 D2");

    ADD_SIG("glm_rotateRef", "E8 ? ? ? ? 41 BC ? ? ? ? 0F B6 47");
    ADD_SIG("glm_translateRef", "E8 ? ? ? ? 48 8D 15 ? ? ? ? 44 0F 28 05");

    ADD_SIG("mce::TextureGroup::getTexture", "40 55 53 56 57 41 54 41 56 41 57 48 8D 6C 24 E9 48 81 EC 00");

    DEPRECATE_SIG("ActorCollision::isOnGround");
}

void SigInit::init2120() {
    Logger::debug("[Signatures] Loading sigs for 1.21.20");

    ADD_SIG("Options::getGamma", "48 83 EC 28 80 B9 ? 17 00 00 00 48 8D 54 24 30 48 8B 01 48 8B 40 60 74 38 41 B8 19");
    ADD_SIG("mce::TextureGroup::getTexture", "40 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24 E9 48 81 EC 00");
    ADD_SIG("RaknetTick", "4C 8B DC 49 89 5B 10 49 89 73 18 57 48 81 EC 10 02");
    ADD_SIG("ItemRenderer::renderGuiItemNew", "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 E8 48 81 EC 18 01 00 00 0F 29 B4 24 00 01 00 00 48");

    ADD_SIG("Actor::baseTick", "48 8D 05 ? ? ? ? 48 89 01 B8 37 00 00 00 8D 50 FA 44 8D 48 FE 44 8D 40 FC 66 89 44 24 20 E8 ? ? ? ? 48 8B 8B C0 10 00 00");

    ADD_SIG("MinecraftPackets::createPacket", "40 53 48 83 EC 30 45 33 C0 48 8B D9 81");

    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 40");

    ADD_SIG("ActorCollision::isOnGround", "E8 ? ? ? ? 84 C0 49 8B 87"); // depricated

    ADD_SIG("HurtColor", "E8 ? ? ? ? E9 ? ? ? ? 8B 43 ? 48 8D 54 24 ? 48 8B 4B ? 89 44 24 ? E8 ? ? ? ? 4C 8B D8");

    ADD_SIG("GuiData::displayClientMessage", "40 55 53 56 57 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 0F B6 F1 49 8B D8");

    ADD_SIG("MeshHelpers::renderMeshImmediately", "E8 ? ? ? ? C6 43 ? ? F3 0F 10 1D ? ? ? ? 0F 57 D2 0F 57 C9");

    ADD_SIG("HudMobEffectsRenderer::render", "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 0F 29 B4 24 ? ? ? ? 0F 29 BC 24 ? ? ? ? 44 0F 29 84 24 ? ? ? ? 44 0F 29 8C 24 ? ? ? ? 44 0F 29 94 24 ? ? ? ? 44 0F 29 9C 24 ? ? ? ? 44 0F 29 A4 24 ? ? ? ? 44 0F 29 AC 24 ? ? ? ? 44 0F 29 B4 24 ? ? ? ? 44 0F 29 BC 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B F9");

    ADD_SIG("MeshHelpers::renderMeshImmediately2", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 49 8B F1 49 8B E8 48 8B FA 4C 8B F1");
}

void SigInit::init2102() {
    Logger::debug("[Signatures] Loading sigs for 1.21.02");

    ADD_SIG("MinecraftPackets::createPacket", "40 53 48 83 EC ? 45 33 C0 48 8B D9 81 FA");
}

void SigInit::init2100() {
    Logger::debug("[Signatures] Loading sigs for 1.21.0X");

    ADD_SIG("BlockSource::getBiome", "40 53 48 83 EC ? 48 8B D9 E8 ? ? ? ? 48 85 C0 75 ? 48 8B 03");
    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 10");

    ADD_SIG("ActorCollision::isOnGround", "E8 ? ? ? ? 88 45 FF");

    ADD_SIG("ItemPositionConst", "F3 0F 10 ? ? ? ? ? F3 0F 59 ? F3 0F 58 C7");

    ADD_SIG("glm_rotateRef", "E8 ? ? ? ? 41 BD ? ? ? ? 41 0F B6 46");

    ADD_SIG("Actor::getOnGroundFlagComponent", "DA BA A3 99 E2 C3");

    ADD_SIG("glm_translateRef", "? ? ? ? ? 4C 8D 05 ? ? ? ? 44 0F 28 05");
    ADD_SIG("glm_translateRef2", "? ? ? ? ? C6 46 ? ? F3 0F 11 74 24 ? F3 0F 10 1D");

    ADD_SIG("ItemRenderer::render", "48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 49 8B D8");
}

void SigInit::init2080() {
    Logger::debug("[Signatures] Loading sigs for 1.20.8X");

    ADD_SIG("tryGetPrefix2", "4C 8B 41 48 4C 8B C9 48 8B 41 50 4C 8B 51 68 49 2B C0 48 C1 F8 03 48 FF");

    // 36 48 C4 71
    ADD_SIG("Actor::getActorEquipmentComponent", "C8 25 36 48 C4 71");
    // Actor::getActorFlag replacment
    ADD_SIG("Actor::getActorDataFlagComponent", "C8 25 76 59 47 33");

    ADD_SIG("ClientInstance::grabMouse", "48 8B 80 ? ? ? ? FF 15 ? ? ? ? 90 48 85 DB 74 08 48 8B CB E8 ? ? ? ? 48 8B 8D ? ? ? ? E8");

    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 C0");

    ADD_SIG("GuiData::displayClientMessage", "40 55 53 56 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 41 0F B6 F0");

    ADD_SIG("glm_rotateRef", "E8 ? ? ? ? BA ? ? ? ? 0F B6 43");

    ADD_SIG("ItemRenderer::render", "48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 ? ? 44 0F 29 ? ? 44 0F 29 ? ? ? ? ? 44 0F 29 ? ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B F8");

    DEPRECATE_SIG("Actor::getArmor");
    DEPRECATE_SIG("Actor::getOffhandSlot");
}

void SigInit::init2070() {
    Logger::debug("[Signatures] Loading sigs for 1.20.7X");

    ADD_SIG("Dimension::getBrightnessDependentFogColor", "? ? ? ? ? ? C2 0F 11 02 C3 CC CC CC CC CC 48 89 54");

    ADD_SIG("OverworldDimension::getBrightnessDependentFogColor", "? ? ? ? ? ? C2 0F 28 D3 F3 0F 59 1D");

    ADD_SIG("LevelRendererPlayer::getFov", "? ? ? ? ? ? ? 48 89 ? ? 57 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 41 0F");

    ADD_SIG("TimeChanger", "? ? ? ? ? ? 76 05 F7 EA C1 FA 09 8B C2");

    ADD_SIG("Actor::getArmor", "48 8B 89 20 04 00 00 48 8B 01 48 8B 40 28 48"); // depricated

    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 E0 1C 00 00");
}

void SigInit::init2060() {
    Logger::debug("[Signatures] Loading sigs for 1.20.6X");

    ADD_SIG("MinecraftGame::onAppSuspended", "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B E9 45 33 F6");

    ADD_SIG("HurtColor", "E8 ? ? ? ? E9 ? ? ? ? 8B 43 ? 48 8D 54 24");

    ADD_SIG("ThirdPersonNametag", "0F 84 ? ? ? ? 49 8B D6 48 8B CE E8 ? ? ? ? 84 C0 0F 84");

    ADD_SIG("Actor::getArmor", "48 8B 89 58 04 00 00 48 8B 01 48 8B 40 28 48");

    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 C8 1C 00 00");

    ADD_SIG("ActorCollision::isOnGround", "E8 ? ? ? ? 84 C0 49 8B 87");
}

void SigInit::init2050() {
    Logger::debug("[Signatures] Loading sigs for 1.20.5X");

    ADD_SIG("tryGetPrefix", "48 89 5C 24 ? 57 48 83 EC ? 48 8B");

    ADD_SIG("tryGetPrefix2", "40 53 48 83 EC 20 48 8B");

    ADD_SIG("MinecraftGame::onAppSuspended", "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B F9 4C 8D B1");

    ADD_SIG("HurtColor", "E8 ? ? ? ? E9 ? ? ? ? 8B 43 18 89 44 24 ? 48 8D 54 24 ? 48 8B 4B ? E8 ? ? ? ? 48 8B F8");

    ADD_SIG("mce::Color::BLACK", "? ? ? ? 0F 11 00 C6 40 10 ? 0F 57 C9 0F 11 4D");

    ADD_SIG("BlockSource::getBiome", "40 53 48 83 EC ? 48 8B D9 E8 ? ? ? ? 48 85 C0 75 5C");

    ADD_SIG("HitResult::getEntity", "48 83 EC 58 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 48 48 8D 51 38 48 8D 4C 24 28 E8 ? ? ? ? 80 7C 24 40 00 74 31");

    ADD_SIG("CameraYaw", "F3 0F 11 ? F3 0F 11 ? ? 48 8B CE");

    ADD_SIG("ThirdPersonNametag", "? ? ? ? ? ? 49 8B D6 48 8B CE E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 97");

    ADD_SIG("Actor::getArmor", "48 8B 89 50 04 00 00 48 8B 01 48 8B 40 28 48");
    ADD_SIG("Actor::getOffhandSlot", "48 8B 89 ? ? ? ? BA ? ? ? ? 48 8B 01 48 8B 40 ? 48 FF 25"); // depricated

    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 D8 1C 00 00");

    ADD_SIG("ItemPositionConst", "F3 0F 59 ? ? ? ? ? F3 41 0F 58 ? ? ? ? ? ? F3 0F 59 ? ? ? ? ? F3 0F 2C"); // Yes this is the same sig as 1.20.30

    ADD_SIG("ItemRenderer::render", "48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B D8");

    ADD_SIG("Tessellator::vertexUV", "48 83 EC ? 80 B9 ? ? ? ? ? 0F 57 E4");
}

void SigInit::init2040() {
    Logger::debug("[Signatures] Loading sigs for 1.20.4X");

    ADD_SIG("LoopbackPacketSender::sendPacket", "48 83 EC ? 48 0F BE ? ? 48 83 C0 ? 74 27");

    ADD_SIG("ActorCollision::isOnGround", "?"); // TODO: wrong

    ADD_SIG("Options::getGamma", "? ? ? ? ? ? ? ? ? ? ? 48 8D 54 24 ? 48 8B 01 48 8B 40 ? 74 ? 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 74 ? 48 8B 42 ? 48 8B 88 ? ? ? ? 48 85 C9 74 ? E8 ? ? ? ? 48 83 C4 ? C3 F3 0F 10 42 ? 48 83 C4 ? C3 41 B8 ? ? ? ? FF 15 ? ? ? ? 48 8B 10 48 85 D2 75 ? E8 ? ? ? ? CC E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC F3 0F 11 4C 24");

    ADD_SIG("Actor::canSee", "E8 ? ? ? ? 84 C0 74 1C 48 8B 4F 48");

    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 78");

    ADD_SIG("ItemPositionConst", "F3 0F 59 05 ? ? ? ? F3 41 0F 58 87");

    ADD_SIG("HudMobEffectsRenderer::render", "48 89 5C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? EE");
}

void SigInit::init2030() {
    Logger::debug("[Signatures] Loading sigs for 1.20.3X");

    ADD_SIG("tryGetPrefix", "40 53 48 83 EC 20 48 8B");

    ADD_SIG("Keyboard::feed", "? ? ? ? ? ? ? 4C 8D 05 ? ? ? ? 89 54 24 20 88");
    ADD_SIG("MouseDevice::feed", "? ? ? ? ? ? ? ? 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 44 0F B7 BC 24 ? ? ? ? 48 8B D9");

    ADD_SIG("ClientInstance::grabMouse", "48 8B 80 ? ? ? ? FF 15 ? ? ? ? 90 48 85 DB 74 08 48 8B CB E8 ? ? ? ? 48 8B 8F ? ? ? ? E8 ? ? ? ? 33 D2");

    ADD_SIG("MinecraftGame::onAppSuspended", "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B F1 48 8D B1 ? ? ? ? 48 83 3E 00 0F 85 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 3B F0 74 1A 48 8B 08 48 C7 00 ? ? ? ? 48 8B 16 48 89 0E");

    ADD_SIG("ScreenView::setupAndRender", "? ? ? ? ? ? ? 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B FA 48 89");
    ADD_SIG("Font::drawTransformed", "48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 48 89 78 ? 41 54 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78");

    ADD_SIG("Actor::getActorGameTypeComponent", "DA BA DE AB CB AF");
    ADD_SIG("Actor::getAABBShapeComponent", "DA BA F2 C9 10 1B");
    ADD_SIG("Actor::getStateVectorComponent", "DA BA 91 3C C9 0E");
    ADD_SIG("Actor::getMobEffectsComponent", "DA BA 2F B4 D6 F7");

    ADD_SIG("ActorCollision::isOnGround", "40 53 48 83 EC ? 48 8B D9 BA E1 2D 1F 21"); // TODO: wrong

    // AutoGG, AutoRQ, DiscordRPC ...
    ADD_SIG("LoopbackPacketSender::sendPacket", "48 89 5C 24 ? 57 48 83 EC 20 48 8B D9 48 8B FA 48 8B 49 20 E8 ? ? ? ? 4C 8B 03 48 8B D7");
    ADD_SIG("MinecraftPackets::createPacket", "40 53 48 83 EC 30 45 33 C0 48 8B D9 FF CA 81 FA ? ? ? ? 0F 87 ? ? ? ? 48 63 C2 48 8D 0D ? ? ? ? 0F B6 84 01 ? ? ? ? 8B 94 81 ? ? ? ? 48 03 D1");
    ADD_SIG("RaknetTick", "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F9 45 33 FF 4C 89 BD ? ? ? ? 48 8D 95 ? ? ? ?");

    // Reach display
    ADD_SIG("GameMode::attack", "48 8D 05 ? ? ? ? 48 89 01 48 89 51 08 48 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ?");

    ADD_SIG("Actor::getRuntimeIDComponent", "DA BA 14 14 A1 3C");

    // Fog color
    ADD_SIG("Dimension::getBrightnessDependentFogColor", "41 0F 10 00 48 8B C2 0F 11 02");
    ADD_SIG("OverworldDimension::getBrightnessDependentFogColor", "41 0F 10 08 48 8B C2 0F");

    // Zoom, UpsideDown and FOV Changer
    ADD_SIG("LevelRendererPlayer::getFov", "48 8B ? 48 89 ? ? 48 89 ? ? 57 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 44 0F ? ? ? 44 0F ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? 41 0F");

    // Zoom
    ADD_SIG("Options::getSensitivity", "4C 8B 51 20 49 B9");

    // Fullbright
    ADD_SIG("Options::getGamma", "48 83 EC 28 80 B9 88 14 00 00 00 48 8D 54 24 30 48 8B 01 48 8B 40 60 74 38 41 B8 17 01 00 00");

    // HurtColor
    ADD_SIG("HurtColor", "E8 ? ? ? ? E9 ? ? ? ? 48 8B ? ? 48 8B ? 8B 43 ? 89 44 ? ? 48 8D ? ? ? E8 ? ? ? ? 48 8B ? 48 85 ? 0F 84 ? ? ? ? 48 8B");

    // Block outline
    ADD_SIG("blockHighlightColor", "? ? ? ? 0F 11 45 ? 0F 11 00 C6 40 10 ? 45 38 96");
    ADD_SIG("mce::Color::BLACK", "? ? ? ? 0F 11 00 C6 40 10 ? 0F 57 C9 0F 11");

    // Coordinates
    ADD_SIG("ForceCoordsOption", "80 78 ? ? 0F 95 C0 48 8B 5C 24");

    //TimeChanger
    ADD_SIG("TimeChanger", "44 8B C2 B8 F1 19 76 05 F7 EA");

    // Weather
    ADD_SIG("BlockSource::getBiome", "40 57 48 83 EC 50 48 8B F9 E8");

    // FreeLook
    ADD_SIG("CameraYaw", "F3 0F 11 30 F3 ? ? 78 ? 49 8B CE");
    ADD_SIG("CameraYaw2", "F3 0F 11 ? F3 0F 11 ? ? 48 8B 4D");
    ADD_SIG("CameraPitch", "F3 0F 11 0E 48 89 9C");
    ADD_SIG("CameraMovement", "F3 0F 11 01 48 8D 56");
    ADD_SIG("getViewPerspective", "48 83 EC ? 48 8B 01 48 8D 54 ? ? 41 B8 03 00 00 00");

    // No Hurt Cam
    ADD_SIG("CameraAssignAngle", "8B 44 24 48 89 43 08 C6");

    // Nametag
    ADD_SIG("ThirdPersonNametag", "? ? ? ? ? ? 49 8B ? 48 8B ? E8 ? ? ? ? 84 C0 ? ? ? ? ? ? 49 8B ? 49 8b");

    // Keystrokes
    ADD_SIG("Actor::getMoveInputHandler", "DA BA 2E CD 8B 46");

    // Hitboxes
    ADD_SIG("Actor::getRenderPositionComponent", "DA BA 6E F3 E8 D4");
    ADD_SIG("Actor::canSee", "48 89 5C 24 20 56 48 81 EC 00"); // Hitboxes

    // onTick event
    ADD_SIG("Actor::baseTick", "48 8D 05 ? ? ? ? 48 89 01 B8 ? ? ? ? 8D 50 FA 44 8D 48 ? 44 8D 40 ? 66 89 44 ? ? E8 ? ? ? ? 48 8B 8B");

    // Nick
    ADD_SIG("Actor::setNametag", "48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 48 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84");

    // Armour HUD
    ADD_SIG("Actor::getArmor", "48 8B 89 28 04 00 00 48 8B 01 48 8B 40 28 48 FF 25");
    ADD_SIG("Actor::getOffhandSlot", "48 8B 89 30 04 00 00 BA 01 00 00 00");
    ADD_SIG("Item::getDamageValue", "48 89 5C 24 ? 57 48 83 EC ? 48 8B DA 48 85 D2");
    ADD_SIG("ItemStack::getMaxDamage", "48 8B 41 08 48 85 C0 74 19");
    ADD_SIG("ItemStack::isEnchanted", "48 83 EC 38 48 8B 49 10 48 85 C9 74 4B");
    ADD_SIG("ItemRenderer::renderGuiItemNew", "48 8B ? 53 55 56 57 41 ? 41 ? 41 ? 41 ? 48 81 EC ? ? ? ? 0F 29 ? ? 0F 29 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? ? ? ? ? 45 8B");

    // Tablist
    ADD_SIG("Actor::getNametag", "48 83 EC ? 48 8B 81 ? ? ? ? 48 85 C0 74 3B 48 8B 08 BA ? ? ? ? 48 8B 40 ? 48 2B C1 48 C1 F8 ? 66 3B D0 73 17");
    ADD_SIG("Level::getRuntimeActorList", "40 53 48 83 EC 30 48 81 C1 D8");

    ADD_SIG("baseActorRenderContext", "48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 49 8B ? 48 8B ? 48 8B ? 48 8D ? ? ? ? ? 48 89 ? 33 ED");

    ADD_SIG("mce::RenderMaterialGroup::ui", "48 8B 05 ? ? ? ? 48 8D 55 ? 48 8D 0D ? ? ? ? 48 8B 40 ? FF 15 ? ? ? ? 48 8B F8");
    ADD_SIG("Tessellator::begin", "48 89 5C 24 ? 56 48 83 EC ? 80 B9 ? ? ? ? ? 45 0F B6");
    ADD_SIG("Tessellator::vertex", "40 57 48 81 EC ? ? ? ? 0F 29 7C ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 8B 81");
    ADD_SIG("Tessellator::vertexUV", "48 83 EC ? 48 8B 94");
    ADD_SIG("Tessellator::colorF", "80 B9 ? ? ? ? ? 4C 8B C1 75");
    ADD_SIG("Tessellator::setRotation", "48 89 5C 24 ? 57 48 81 EC ? ? ? ? F3 41 0F 10");
    ADD_SIG("Tessellator::resetTransform", "40 53 48 81 EC ? ? ? ? C6 81 ? ? ? ? ? 48 8B D9 84 D2");
    ADD_SIG("MeshHelpers::renderMeshImmediately", "48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 49 8B F8 48 8B DA 48 8B F1 80 BA");
    ADD_SIG("MeshHelpers::renderMeshImmediately2", "40 55 53 56 57 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4D 8B F9");

    ADD_SIG("MinecraftUIRenderContext::getTexture", "40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 60 48 8B DA 48 89");

    ADD_SIG("mce::TextureGroup::getTexture", "40 55 53 56 57 41 54 41 56 41 57 48 8D 6C 24 E9 48 81 EC 00 01 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 07");

    ADD_SIG("GuiData::displayClientMessage", "40 55 53 56 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 41 0F B6 F0");

    ADD_SIG("HitResult::getEntity", "E8 ? ? ? ? EB 1A 48 8B CE");

    ADD_SIG("ClientInstance::getLocalPlayerIndex", "49 8B 00 49 8B C8 48 8B 80 ? ? ? ? FF 15 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 0F");

    // ItemPhysics
    ADD_SIG("ItemPositionConst", "F3 0F 59 ? ? ? ? ? F3 41 0F 58 ? ? ? ? ? ? F3 0F 59 ? ? ? ? ? F3 0F 2C");

    ADD_SIG("glm_rotate", "40 53 48 83 EC ? F3 0F 59 ? ? ? ? ? 4C 8D 4C 24");
    ADD_SIG("glm_rotateRef", "E8 ? ? ? ? B8 ? ? ? ? 41 0F B6");

    ADD_SIG("glm_translateRef", "? ? ? ? ? 41 C6 ? ? ? F3 0F 11 7C 24 ? 41 0F 28 DA");

    ADD_SIG("ItemRenderer::render", "48 8B C4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B D8");
    ADD_SIG("HudMobEffectsRenderer::render", "48 8B C4 48 89 58 08 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 28 F5");

    ADD_SIG("BaseActorRenderer::renderText", "E8 ? ? ? ? 48 83 C3 ? 48 3B DF 75 ? 48 8B 74 24 ? 48 8B 5C 24 ? 48 8B 6C 24");

    ADD_SIG("mce::MathUtility::getRotationFromNegZToVector", "48 8B C4 48 81 EC ? ? ? ? F3 0F 10 2A");
}
