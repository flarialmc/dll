#include "PacketHandlerHook.h"

typedef void(*TextPacketHandler)(const float*, const float*, const float*, std::shared_ptr<Packet>);
TextPacketHandler oTextPacketHandler;

void PacketHandlerDetour(const float* a1, const float* networkIdentifier, const float* netEventCallback, std::shared_ptr<Packet> packet) {
	Packet* pPacket = packet.get();

	MinecraftPacketIds packetId = pPacket->getId();

	Manager::onPacketHandler(pPacket);

	switch (packetId)
	{
	case MinecraftPacketIds::KeepAlive:
		break;
	case MinecraftPacketIds::Login:
		break;
	case MinecraftPacketIds::PlayStatus:
		break;
	case MinecraftPacketIds::ServerToClientHandshake:
		break;
	case MinecraftPacketIds::ClientToServerHandshake:
		break;
	case MinecraftPacketIds::Disconnect:
		break;
	case MinecraftPacketIds::ResourcePacksInfo:
		break;
	case MinecraftPacketIds::ResourcePackStack:
		break;
	case MinecraftPacketIds::ResourcePackClientResponse:
		break;
	case MinecraftPacketIds::Text:
		return oTextPacketHandler(a1, networkIdentifier, netEventCallback, packet);
		break;
	case MinecraftPacketIds::SetTime:
		break;
	case MinecraftPacketIds::StartGame:
		break;
	case MinecraftPacketIds::AddPlayer:
		break;
	case MinecraftPacketIds::AddActor:
		break;
	case MinecraftPacketIds::RemoveActor:
		break;
	case MinecraftPacketIds::AddItemActor:
		break;
	case MinecraftPacketIds::TakeItemActor:
		break;
	case MinecraftPacketIds::MoveActorAbsolute:
		break;
	case MinecraftPacketIds::MovePlayer:
		break;
	case MinecraftPacketIds::RiderJump:
		break;
	case MinecraftPacketIds::UpdateBlock:
		break;
	case MinecraftPacketIds::AddPainting:
		break;
	case MinecraftPacketIds::TickSync:
		break;
	case MinecraftPacketIds::LevelSoundEventV1:
		break;
	case MinecraftPacketIds::LevelEvent:
		break;
	case MinecraftPacketIds::TileEvent:
		break;
	case MinecraftPacketIds::ActorEvent:
		break;
	case MinecraftPacketIds::MobEffect:
		break;
	case MinecraftPacketIds::UpdateAttributes:
		break;
	case MinecraftPacketIds::InventoryTransaction:
		break;
	case MinecraftPacketIds::MobEquipment:
		break;
	case MinecraftPacketIds::MobArmorEquipment:
		break;
	case MinecraftPacketIds::Interact:
		break;
	case MinecraftPacketIds::BlockPickRequest:
		break;
	case MinecraftPacketIds::ActorPickRequest:
		break;
	case MinecraftPacketIds::PlayerAction:
		break;
	case MinecraftPacketIds::ActorFall:
		break;
	case MinecraftPacketIds::HurtArmor:
		break;
	case MinecraftPacketIds::SetActorData:
		break;
	case MinecraftPacketIds::SetActorMotion:
		break;
	case MinecraftPacketIds::SetActorLink:
		break;
	case MinecraftPacketIds::SetHealth:
		break;
	case MinecraftPacketIds::SetSpawnPosition:
		break;
	case MinecraftPacketIds::Animate:
		break;
	case MinecraftPacketIds::Respawn:
		break;
	case MinecraftPacketIds::ContainerOpen:
		break;
	case MinecraftPacketIds::ContainerClose:
		break;
	case MinecraftPacketIds::PlayerHotbar:
		break;
	case MinecraftPacketIds::InventoryContent:
		break;
	case MinecraftPacketIds::InventorySlot:
		break;
	case MinecraftPacketIds::ContainerSetData:
		break;
	case MinecraftPacketIds::CraftingData:
		break;
	case MinecraftPacketIds::CraftingEvent:
		break;
	case MinecraftPacketIds::GuiDataPickItem:
		break;
	case MinecraftPacketIds::AdventureSettings:
		break;
	case MinecraftPacketIds::BlockActorData:
		break;
	case MinecraftPacketIds::PlayerInput:
		break;
	case MinecraftPacketIds::LevelChunk:
		break;
	case MinecraftPacketIds::SetCommandsEnabled:
		break;
	case MinecraftPacketIds::SetDifficulty:
		break;
	case MinecraftPacketIds::ChangeDimension:
		break;
	case MinecraftPacketIds::SetPlayerGameType:
		break;
	case MinecraftPacketIds::PlayerList:
		break;
	case MinecraftPacketIds::SimpleEvent:
		break;
	case MinecraftPacketIds::Event:
		break;
	case MinecraftPacketIds::SpawnExperienceOrb:
		break;
	case MinecraftPacketIds::ClientboundMapItemData:
		break;
	case MinecraftPacketIds::MapInfoRequest:
		break;
	case MinecraftPacketIds::RequestChunkRadius:
		break;
	case MinecraftPacketIds::ChunkRadiusUpdated:
		break;
	case MinecraftPacketIds::ItemFrameDropItem:
		break;
	case MinecraftPacketIds::GameRulesChanged:
		break;
	case MinecraftPacketIds::Camera:
		break;
	case MinecraftPacketIds::BossEvent:
		break;
	case MinecraftPacketIds::ShowCredits:
		break;
	case MinecraftPacketIds::AvailableCommands:
		break;
	case MinecraftPacketIds::CommandRequest:
		break;
	case MinecraftPacketIds::CommandBlockUpdate:
		break;
	case MinecraftPacketIds::CommandOutput:
		break;
	case MinecraftPacketIds::UpdateTrade:
		break;
	case MinecraftPacketIds::UpdateEquip:
		break;
	case MinecraftPacketIds::ResourcePackDataInfo:
		break;
	case MinecraftPacketIds::ResourcePackChunkData:
		break;
	case MinecraftPacketIds::ResourcePackChunkRequest:
		break;
	case MinecraftPacketIds::Transfer:
		break;
	case MinecraftPacketIds::PlaySoundW:
		break;
	case MinecraftPacketIds::StopSound:
		break;
	case MinecraftPacketIds::SetTitle:
		break;
	case MinecraftPacketIds::AddBehaviorTree:
		break;
	case MinecraftPacketIds::StructureBlockUpdate:
		break;
	case MinecraftPacketIds::ShowStoreOffer:
		break;
	case MinecraftPacketIds::PurchaseReceipt:
		break;
	case MinecraftPacketIds::PlayerSkin:
		break;
	case MinecraftPacketIds::SubClientLogin:
		break;
	case MinecraftPacketIds::AutomationClientConnect:
		break;
	case MinecraftPacketIds::SetLastHurtBy:
		break;
	case MinecraftPacketIds::BookEdit:
		break;
	case MinecraftPacketIds::NpcRequest:
		break;
	case MinecraftPacketIds::PhotoTransfer:
		break;
	case MinecraftPacketIds::ModalFormRequest:
		break;
	case MinecraftPacketIds::ModalFormResponse:
		break;
	case MinecraftPacketIds::ServerSettingsRequest:
		break;
	case MinecraftPacketIds::ServerSettingsResponse:
		break;
	case MinecraftPacketIds::ShowProfile:
		break;
	case MinecraftPacketIds::SetDefaultGameType:
		break;
	case MinecraftPacketIds::RemoveObjective:
		break;
	case MinecraftPacketIds::SetDisplayObjective:
		break;
	case MinecraftPacketIds::SetScore:
		break;
	case MinecraftPacketIds::LabTable:
		break;
	case MinecraftPacketIds::UpdateBlockSynced:
		break;
	case MinecraftPacketIds::MoveActorDelta:
		break;
	case MinecraftPacketIds::SetScoreboardIdentity:
		break;
	case MinecraftPacketIds::SetLocalPlayerAsInitialized:
		break;
	case MinecraftPacketIds::UpdateSoftEnum:
		break;
	case MinecraftPacketIds::NetworkStackLatency:
		break;
	case MinecraftPacketIds::BlockPalette:
		break;
	case MinecraftPacketIds::ScriptCustomEvent:
		break;
	case MinecraftPacketIds::SpawnParticleEffect:
		break;
	case MinecraftPacketIds::AvailableActorIdentifiers:
		break;
	case MinecraftPacketIds::LevelSoundEventV2:
		break;
	case MinecraftPacketIds::NetworkChunkPublisherUpdate:
		break;
	case MinecraftPacketIds::BiomeDefinitionList:
		break;
	case MinecraftPacketIds::LevelSoundEvent:
		break;
	case MinecraftPacketIds::LevelEventGeneric:
		break;
	case MinecraftPacketIds::LecternUpdate:
		break;
	case MinecraftPacketIds::VideoStreamConnect:
		break;
	case MinecraftPacketIds::AddEntity:
		break;
	case MinecraftPacketIds::RemoveEntity:
		break;
	case MinecraftPacketIds::ClientCacheStatus:
		break;
	case MinecraftPacketIds::OnScreenTextureAnimation:
		break;
	case MinecraftPacketIds::MapCreateLockedCopy:
		break;
	case MinecraftPacketIds::StructureTemplateDataRequest:
		break;
	case MinecraftPacketIds::StructureTemplateDataResponse:
		break;
	case MinecraftPacketIds::UpdateBlockProperties:
		break;
	case MinecraftPacketIds::ClientCacheBlobStatus:
		break;
	case MinecraftPacketIds::ClientCacheMissResponse:
		break;
	case MinecraftPacketIds::EducationSettings:
		break;
	case MinecraftPacketIds::Emote:
		break;
	case MinecraftPacketIds::MultiplayerSettings:
		break;
	case MinecraftPacketIds::SettingsCommand:
		break;
	case MinecraftPacketIds::AnvilDamage:
		break;
	case MinecraftPacketIds::CompletedUsingItem:
		break;
	case MinecraftPacketIds::NetworkSettings:
		break;
	case MinecraftPacketIds::PlayerAuthInput:
		break;
	case MinecraftPacketIds::CreativeContent:
		break;
	case MinecraftPacketIds::PlayerEnchantOptions:
		break;
	case MinecraftPacketIds::ItemStackRequest:
		break;
	case MinecraftPacketIds::ItemStackResponse:
		break;
	case MinecraftPacketIds::PlayerArmorDamage:
		break;
	case MinecraftPacketIds::CodeBuilder:
		break;
	case MinecraftPacketIds::UpdatePlayerGameType:
		break;
	case MinecraftPacketIds::EmoteList:
		break;
	case MinecraftPacketIds::PositionTrackingDBServerBroadcast:
		break;
	case MinecraftPacketIds::PositionTrackingDBClientRequest:
		break;
	case MinecraftPacketIds::DebugInfo:
		break;
	case MinecraftPacketIds::PacketViolationWarning:
		break;
	default:
		break;
	}
}

PacketHandlerHook::PacketHandlerHook() : Hook() {
	using _createPacket = std::shared_ptr<Packet>(*)(MinecraftPacketIds);
	uintptr_t hookAddr = Mem::findSig(Signatures::CreatePacket);
	if (hookAddr == NULL) {
		return;
	}
	_createPacket createPacket = Mem::FuncFromSigOffset<_createPacket>(hookAddr, 1);

	std::shared_ptr<Packet> textPacket = createPacket(MinecraftPacketIds::Text);

	Mem::hookFunc((void*)textPacket->packetHandler->vTable[1], (void*)PacketHandlerDetour, (void**)&oTextPacketHandler, "TextPacketHandler");
}