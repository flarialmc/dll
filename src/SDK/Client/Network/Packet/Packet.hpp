#pragma once

#include <cstdint>

enum InputMode {
    INPUT_MODE_UNKNOWN = 0, MOUSE = 1, TOUCH = 2, GAME_PAD = 3, MOTION_CONTROLLER = 4
};
enum PlayMode {
    PLAY_MODE_NORMAL = 0,
    TEASER = 1,
    SCREEN = 2,
    VIEWER = 3,
    REALITY = 4,
    PLACEMENT = 5,
    LIVING_ROOM = 6,
    EXIT_LEVEL = 7,
    EXIT_LEVEL_LIVING_ROOM = 8,
    NUM_MODES = 9
};

enum MovePacketMode {
    MOVE_PACKET_MODE_NORMAL = 0, RESET = 1, TELEPORT = 2, ROTATION = 3
};
enum MovePacketCause {
    MOVE_PACKET_CAUSE_UNKNOWN = 0, PROJECTILE = 1, CHORUS_FRUIT = 2, COMMAND = 3, BEHAVIOR = 4
};

/* namespace PacketVTables {
    uintptr_t TextPacketSig = NULL;
    uintptr_t CommandRequestPacketSig = NULL;
} */

enum class MinecraftPacketIds : int {
    KeepAlive = 0x0,
    Login = 0x1,
    PlayStatus = 0x2,
    ServerToClientHandshake = 0x3,
    ClientToServerHandshake = 0x4,
    Disconnect = 0x5,
    ResourcePacksInfo = 0x6,
    ResourcePackStack = 0x7,
    ResourcePackClientResponse = 0x8,
    Text = 0x9,
    SetTime = 0xA,
    StartGame = 0xB,
    AddPlayer = 0xC,
    AddActor = 0xD,
    RemoveActor = 0xE,
    AddItemActor = 0xF,
    UNUSED = 0x10,
    TakeItemActor = 0x11,
    MoveAbsoluteActor = 0x12,
    MovePlayer = 0x13,
    PassengerJump = 0x14,
    UpdateBlock = 0x15,
    AddPainting = 0x16,
    TickSync = 0x17,
    LevelSoundEventV1 = 0x18,
    LevelEvent = 0x19,
    TileEvent = 0x1A,
    ActorEvent = 0x1B,
    MobEffect = 0x1C,
    UpdateAttributes = 0x1D,
    InventoryTransaction = 0x1E,
    PlayerEquipment = 0x1F,
    MobArmorEquipment = 0x20,
    Interact = 0x21,
    BlockPickRequest = 0x22,
    ActorPickRequest = 0x23,
    PlayerAction = 0x24,
    ActorFall_deprecated = 0x25,
    HurtArmor = 0x26,
    SetActorData = 0x27,
    SetActorMotion = 0x28,
    SetActorLink = 0x29,
    SetHealth = 0x2A,
    SetSpawnPosition = 0x2B,
    Animate = 0x2C,
    Respawn = 0x2D,
    ContainerOpen = 0x2E,
    ContainerClose = 0x2F,
    PlayerHotbar = 0x30,
    InventoryContent = 0x31,
    InventorySlot = 0x32,
    ContainerSetData = 0x33,
    CraftingData = 0x34,
    CraftingEvent = 0x35,
    GuiDataPickItem = 0x36,
    AdventureSettings_Deprecated = 0x37,
    BlockActorData = 0x38,
    PlayerInput = 0x39,
    FullChunkData = 0x3A,
    SetCommandsEnabled = 0x3B,
    SetDifficulty = 0x3C,
    ChangeDimension = 0x3D,
    SetPlayerGameType = 0x3E,
    PlayerList = 0x3F,
    SimpleEvent = 0x40,
    Event = 0x41,
    SpawnExperienceOrb = 0x42,
    MapData = 0x43,
    MapInfoRequest = 0x44,
    RequestChunkRadius = 0x45,
    ChunkRadiusUpdated = 0x46,
    ItemFrameDropItem = 0x47,
    GameRulesChanged = 0x48,
    Camera = 0x49,
    BossEvent = 0x4A,
    ShowCredits = 0x4B,
    AvailableCommands = 0x4C,
    CommandRequest = 0x4D,
    CommandBlockUpdate = 0x4E,
    CommandOutput = 0x4F,
    UpdateTrade = 0x50,
    UpdateEquip = 0x51,
    ResourcePackDataInfo = 0x52,
    ResourcePackChunkData = 0x53,
    ResourcePackChunkRequest = 0x54,
    Transfer = 0x55,
    PlaySoundPacket = 0x56,
    StopSound = 0x57,
    SetTitle = 0x58,
    AddBehaviorTree = 0x59,
    StructureBlockUpdate = 0x5A,
    ShowStoreOffer = 0x5B,
    PurchaseReceipt = 0x5C,
    PlayerSkin = 0x5D,
    SubclientLogin = 0x5E,
    AutomationClientConnect = 0x5F,
    SetLastHurtBy = 0x60,
    BookEdit = 0x61,
    NPCRequest = 0x62,
    PhotoTransfer = 0x63,
    ShowModalForm = 0x64, // ModalFormRequest
    ModalFormResponse = 0x65,
    ServerSettingsRequest = 0x66,
    ServerSettingsResponse = 0x67,
    ShowProfile = 0x68,
    SetDefaultGameType = 0x69,
    RemoveObjective = 0x6A,
    SetDisplayObjective = 0x6B,
    SetScore = 0x6C,
    LabTable = 0x6D,
    UpdateBlockSynced = 0x6E,
    MoveDeltaActor = 0x6F,
    SetScoreboardIdentity = 0x70,
    SetLocalPlayerAsInit = 0x71,
    UpdateSoftEnum = 0x72,
    Ping = 0x73,
    BlockPalette = 0x74,
    ScriptCustomEvent = 0x75,
    SpawnParticleEffect = 0x76,
    AvailableActorIDList = 0x77,
    LevelSoundEventV2 = 0x78,
    NetworkChunkPublisherUpdate = 0x79,
    BiomeDefinitionList = 0x7A,
    LevelSoundEvent = 0x7B,
    LevelEventGeneric = 0x7C,
    LecternUpdate = 0x7D,
    VideoStreamConnect_DEPRECATED = 0x7E,
    AddEntity = 0x7F,
    RemoveEntity = 0x80,
    ClientCacheStatus = 0x81,
    OnScreenTextureAnimation = 0x82,
    MapCreateLockedCopy = 0x83,
    StructureTemplateDataExportRequest = 0x84,
    StructureTemplateDataExportResponse = 0x85,
    UNUSED_PLS_USE_ME2 = 0x86,
    ClientCacheBlobStatusPacket = 0x87,
    ClientCacheMissResponsePacket = 0x88,
    EducationSettingsPacket = 0x89,
    Emote = 0x8A,
    MultiplayerSettingsPacket = 0x8B,
    SettingsCommandPacket = 0x8C,
    AnvilDamage = 0x8D,
    CompletedUsingItem = 0x8E,
    NetworkSettings = 0x8F,
    PlayerAuthInputPacket = 0x90,
    CreativeContent = 0x91,
    PlayerEnchantOptions = 0x92,
    ItemStackRequest = 0x93,
    ItemStackResponse = 0x94,
    PlayerArmorDamage = 0x95,
    CodeBuilderPacket = 0x96,
    UpdatePlayerGameType = 0x97,
    EmoteList = 0x98,
    PositionTrackingDBServerBroadcast = 0x99,
    PositionTrackingDBClientRequest = 0x9A,
    DebugInfoPacket = 0x9B,
    PacketViolationWarning = 0x9C,
    MotionPredictionHints = 0x9D,
    TriggerAnimation = 0x9E,
    CameraShake = 0x9F,
    PlayerFogSetting = 0xA0,
    CorrectPlayerMovePredictionPacket = 0xA1,
    ItemComponentPacket = 0xA2,
    FilterTextPacket = 0xA3,
    ClientBoundDebugRendererPacket = 0xA4,
    SyncActorProperty = 0xA5,
    AddVolumeEntityPacket = 0xA6,
    RemoveVolumeEntityPacket = 0xA7,
    SimulationTypePacket = 0xA8,
    NpcDialoguePacket = 0xA9,
    EduUriResourcePacket = 0xAA,
    CreatePhotoPacket = 0xAB,
    UpdateSubChunkBlocks = 0xAC,
    PhotoInfoRequest_DEPRECATED = 0xAD,
    SubChunkPacket = 0xAE,
    SubChunkRequestPacket = 0xAF,
    PlayerStartItemCooldown = 0xB0,
    ScriptMessagePacket = 0xB1,
    CodeBuilderSourcePacket = 0xB2,
    TickingAreasLoadStatus = 0xB3,
    DimensionDataPacket = 0xB4,
    AgentAction = 0xB5,
    ChangeMobProperty = 0xB6,
    LessonProgressPacket = 0xB7,
    RequestAbilityPacket = 0xB8,
    RequestPermissionsPacket = 0xB9,
    ToastRequest = 0xBA,
    UpdateAbilitiesPacket = 0xBB,
    UpdateAdventureSettingsPacket = 0xBC,
    DeathInfo = 0xBD,
    EditorNetworkPacket = 0xBE,
    FeatureRegistryPacket = 0xBF,
    ServerStats = 0xC0,
    RequestNetworkSettings = 0xC1,
    GameTestRequestPacket = 0xC2,
    GameTestResultsPacket = 0xC3,
    PlayerClientInputPermissions = 0xC4,
    ClientCheatAbilityPacket = 0xC5,
    CameraPresets = 0xC6,
    UnlockedRecipes = 0xC7,
    CameraInstruction = 0x12C,
    CompressedBiomeDefinitionList = 0x12D,
    TrimData = 0x12E,
    OpenSign = 0x12F,
    AgentAnimation = 0x130,
    RefreshEntitlements = 0x131,
    EndId,
};

class PacketHandlerDispatcherInstance {
public:
    uintptr_t **vTable;
};

class Packet {
public:
    uintptr_t **vTable;
private:
    char pad_0000[0x18];
public:
    PacketHandlerDispatcherInstance *packetHandler;
private:
    char pad_0028[0x8];
public:
    template<typename P>
    void setVTable(uintptr_t sig) {
        if (sig == NULL)
            return;

        int offset = *(int *) (sig + 3);
        this->vTable = (uintptr_t **) (sig + offset + 7);
    }

public:
    MinecraftPacketIds getId();
};

static_assert(sizeof(Packet) == 0x30);