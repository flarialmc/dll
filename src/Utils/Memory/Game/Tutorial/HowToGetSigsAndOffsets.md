# How to get signatures/offsets needed for update

Requirements: IDA/Ghidra + CheatEngine

To get some initial understanding, I recommend reading this, even tho it's incomplete as of now: https://frederoxdev.github.io/Bedrock-Modding-Wiki/

How to check sigs:

1. Open CheatEngine
2. Open Minecraft and connect CheatEngine to it
3. In CheatEngine's search panel set them as follows: instead of "All", choose Minecraft.Windows.exe, Writable - uncheck, Executable - check, Scan Type - Array of byte
4. Now you're ready to scan!
5. Paste sig in "Array of byte" textbox and scan it, if 1 result found - sig is good

### Components:

tryGetPrefix - because we're not using entt, I made this "sig" that is only used to offset to start of tryGet function, that is basically bytes before "DA BA" and component hash "DE AB CB AF"
tryGetPrefix2 - same thing but later after a few updates tryGet on some components is a bit bigger, so that is for that case, also that sig is from start to "C8 25" and component hash "36 48 C4 71"

General idea on sigs used here for components:

1. its unlikely following sigs will change, but "tryGetPrefix"'s might
2. to "find" "tryGetPrefix" - search for any component's tryGet function scanning sigs below and copy bytes before them.
3. Component "sig" rn is 2 bytes from tryGet (usually DA BA) + component hash (DA BA + 2E CD 8B 46 hash)

Actor::getActorEquipmentComponent - tryGetPrefix2 + C8 25 36 48 C4 71
Actor::getActorDataFlagComponent - tryGetPrefix2 + C8 25 76 59 47 33

Actor::getActorGameTypeComponent - tryGetPrefix + DA BA DE AB CB AF
Actor::getMoveInputHandler - tryGetPrefix + DA BA 2E CD 8B 46
Actor::getAABBShapeComponent - tryGetPrefix + DA BA F2 C9 10 1B
Actor::getStateVectorComponent - tryGetPrefix + DA BA 91 3C C9 0E
Actor::getRuntimeIDComponent - tryGetPrefix + DA BA 14 14 A1 3C
Actor::getRenderPositionComponent - tryGetPrefix + DA BA 6E F3 E8 D4

## To get actual try\_get this could be used (.4X, might work on lower): 42 81 7C 09 08 + hash

### Actor::getActorDataFlagComponent

its inside Player::BaseTick, under while(aGameplayemote) loop, above \& 0x10000000
Actor::basicReg offset is offset in first arg

### GuiData::displayClientMessage:

You can try just searching for this sig that leads to the func call directly that worked from 1.20.3X to 1.21.2:
"E8 ?? ?? ?? ?? 90 48 85 DB 74 09 48 8B CB E8 ?? ?? ?? ?? 90 48 89 7D C7 48 8B 4D CF 48 89 7D CF 48 85 C9 74 09 E8 ?? ?? ?? ?? 48 8B 7D CF 48 85 FF 74 09"

If it didnt work:

1. Open IDA/Ghidra
2. Search for bytes and input "persona.emote.easter\_egg" or "persona.emote.chat\_message" there, search
3. When result found, xref to first function
4. Then switch to pseudocode
5. On lines 114-117 find 3 arg function, that if you go into starts with a bunch aXX = 0 at the start

### mce::TextureGroup::getTexture

Search for textures/ui/heart\_background or %sabsorption\_heart\_half - first ref, at the end - getTexture

### Player::playerName

First try those sigs (they lead directly to offset):

If didnt work, this one:
48 89 5C 24 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 C0 48 81 EC 40 01 00 00 41 (its sig to function described below)
if didnt work:

1. Open IDA/Ghidra
2. Search for bytes and input "book.defaultAuthor" there, search
3. When result found, xref to first function
4. Then switch to pseudocode
5. Approx on line 278 would be the required offset

### Player::playerInventory \& PlayerInventory::inventory

48 8b 91 (? ? ? ?) Player::playerInventory 80 ba ? ? ? ? ? 75 ? 48 8b 8a (? ? ? ?) PlayerInventory::inventory 8b 52 ? 48 8b 01 48 8b 40 ? 48 ff 25 ? ? ? ? 48 8d 05
48 8b 91 ? ? ? ? 80 ba ? ? ? ? ? 75 ? 48 8b 8a ? ? ? ? 8b 52 ? 48 8b 01 48 8b 40 ? 48 ff 25 ? ? ? ? 48 8d 05
Could also be used:
48 8b ? (? ? ? ?) 80 ba ? ? ? ? ? 75 ? 48 8b 8a (? ? ? ?) ?

Gives a million results but 99% of what it has is correct offsets

### Player::gamemode

48 8b b5 ? ? ? ? 48 8b 06 48 8b 78 ? 48 8b 01 // 1.20.3X+
49 8b 9e ? ? ? ? 48 8b 03 4c 8d 4c 24 // 1.20.3X+
48 8b 8b ? ? ? ? 45 84 f6 74 ? 45 84 e4
4c 8b b0 ? ? ? ? 49 8b 06 4c 8b b8
48 8b 88 ? ? ? ? 48 8d 54 24 ? c6 80

### Actor::hurtTime

8b 86 ? ? ? ? 85 c0 7e ? ff c8 89 86 ? ? ? ? 48 8b 96 // 1.20.3X+
44 8b 87 ? ? ? ? b8 ? ? ? ? 41 f7 e8 // 1.20.3X+
89 86 ? ? ? ? 48 8b 96 ? ? ? ? 48 83 fa // 1.20.3X+
c7 81 ? ? ? ? 0A 00 00 00 48 8b 81 // 2 results, 1st one is the one, 1.20.3X+
c7 81 ? ? ? ? 0A 00 00 00 48 8b 81 ? ? ? ? 48 85 c0 0f 84 ? ? ? ? c7 00 // more direct one



### Actor::level

48 8b 8f ? ? ? ? 48 8b 11 48 8b 82 ? ? ? ? ff 15 ? ? ? ? 48 8b 8f // 1.20.3X+
48 8b 8e ? ? ? ? 48 8b 01 48 8b 80 ? ? ? ? ff 15 ? ? ? ? 48 8b 10 // 1.20.3X+
48 8b 8e ? ? ? ? 48 8b 01 48 8b 80 ? ? ? ? ff 15 ? ? ? ? 48 8b 08 48 89 8b // 1.20.3X+
49 8b 8f ? ? ? ? 48 8b 01 48 8b 80 ? ? ? ? ff 15 ? ? ? ? 84 c0 75 // 1.20.3X+

4d 8b 81 ? ? ? ? 48 8b 42 ? 48 85 c0
49 83 be ? ? ? ? ? 0f 84 ? ? ? ? 48 8b 86

### Actor::categories

f6 87 ? ? ? ? ? 74 ? 41 0f 28 c2 // 1.20.3X+
f6 83 ? ? ? ? ? 74 ? 48 8b cb // 1.20.3X+
8b 81 ? ? ? ? d1 e8 a8 ? 48 0f 45 f9 // 1.20.3X+
8b 87 ? ? ? ? d1 e8 a8 ? 0f 84 ? ? ? ? 48 8b 17 // 1.20.3X+
8b 83 ? ? ? ? d1 e8 a8 ? 74 ? 80 bb // 1.20.3X+

f6 87 ? ? ? ? ? 0f 84 ? ? ? ? 4c 8b b7
8b 88 ? ? ? ? 0f 29 b4 24 ? ? ? ? 0f 28 35

### Level::getPlayerMap

aka Level::getPlayerList
Find getRuntimeActorList using method in sigs section
Add 0xB8 to 1st arg getRuntimeActorList gets called with and you get the offset

### Level::hitResult

// for newer versions
48 8b 81 ? ? ? ? 48 85 c0 74 ? 48 83 c0 ? 48 83 c4 - gives multiple results, last one is the correct one, as of .40 2nd from end is it
48 8b 81 ? ? ? ? 48 85 c0 74 ? 48 83 c0 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc 48 8d 81 ? ? ? ? c3 cc cc cc cc cc cc cc cc 40 53
48 8b 81 ? ? ? ? 48 85 c0 74 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc cc cc cc cc 48 83 ec ? 48 8b 81 ? ? ? ? 48 85 c0 74 ? 48 83 c0 ? 48 83 c4 ? c3 e8 ? ? ? ? cc cc 48 8d 81

### Gamemode::player

### Gamemode::lastBreakProgress

4d 89 66 ? 49 8b 8e // 1.20.3X+
89 46 ? 80 be

### ScreenView::VisualTree

4c 8b 43 ? 49 8b 08 48 8b 41
48 8b 49 ? 48 8b 01 48 8b 40 ? ff 15 ? ? ? ? 84 c0 0f 84 ? ? ? ? 45 32 ff
48 8b 4f ? 48 8b 01 ba ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 84 c0 74 ? 48 8b 4f ? 83 61 ? ? 48 8b 4f ? 48 8b 01 48 8b 40 ? ff 15 ? ? ? ? 66 0f 1f 84 00
48 8b 4f ? 48 8b 01 ba ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 84 c0 74 ? 48 8b 4f ? 83 61 ? ? 4c 8b 87
48 8b 4f ? 48 8b 01 ba ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 84 c0 74 ? 48 8b 4f ? 83 61 ? ? 48 8b 4f ? 48 8b 01 48 8b 40 ? ff 15 ? ? ? ? 48 8b 4f
48 8b 4f ? 48 8b 01 ba ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 84 c0 74 ? 48 8b b7
4c 8b 47 ? 49 8b 08 48 8b 41 ? ba
48 8b 4f ? 48 8b 01 ba ? ? ? ? 48 8b 40 ? ff 15 ? ? ? ? 84 c0 75 ? 48 8b 4f ? 48 8b 41 ? 48 39 41 ? 75 ? 48 8b 81 ? ? ? ? 48 39 81 ? ? ? ? 75 ? 48 8b 81 ? ? ? ? 48 39 81 ? ? ? ? 75 ? 80 b9

### VisualTree::root



### UIControl::LayerName



### Actor::runtimeID / EntityContext::id (EntityId) + EntityContext::basicReg

Found near getActorFlag first param is basicReg, second is runtimeID



# VTable Indexes

Memory::GetAddressByIndex(vtableAddr, function

### ContainerScreenController::\_handlePlaceAll

"#is\_creative\_and\_recipe\_book\_layout" (CraftingScreenController::\_registerBindings) xref out -> CraftingScreenController::CraftingScreenController -> CraftingScreenController::`vftable' -> \_handlePlaceAll

### ClientInstance::getScreenName

"content\_area" x3 + "focus\_reset\_input\_panel" + "ScreenVersion"
at the end 2 arg func calls it by index:
vxx1 = \*(\_QWORD *)(a1 + xxxx);
v34 = sub\_140xxxxx(*(\_QWORD \*)(a1 + xxxx), (\_\_int64)\&vxx2);
Inside second virtual call is it, sig at .4X - 48 8B 80 58 08 00 00 FF 15 ?? ?? ?? ?? 90 BE FF FF FF FF

also 48 8B ? ? ? ? ? FF 15 ? ? ? ? 90 41 8B ? 89 5D

### LevelRendererCamera\_\_onDeviceLost

It calls j\_LevelRendererCamera\_\_\_releaseResources second func from it is onDeviceLost aka \_releaseResources + 2 = onDeviceLost
Info on how to find \_releaseResource is in sigs setion



# Bobbing function

0.017453292 find this and compare refs
48 89 5C 24 10 48 89 74 24 20 4C 89 44 24 18 57 48 83 EC 60 0F 29 74 24 50 0F 29 1.21.30
40 53 56 57 48 81 EC 80 00 00 00 0F 29 74 24 70 0F 29 7C 24 60 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 44 24 50 1.21.20 and below a bit



# Sigs to functions

### Font::drawTransformed

splashes.json

### ItemRenderer::renderGuiItemNew

textures/entity/shulker/shulker\_
xref out 1 time, then xref to last func and here you go

### Level::getRuntimeActorList

48 89 5C 24 18 55 56 57 48 83 EC 40 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 44 24 38 48 8B F2 48 8B E9 48 89
xref out and here u go
or scan 40 53 48 83 EC 30 48 81 (1.21.30+) first res should be it

### Actor::baseTick

48 83 FA 0F 76 34 48 8B 8B 80 10 00 00 should lead you to the func u need. At the start, you will see:
*a1 = off_xxxxxxx. Make a sig here.

### Player::BaseTick

gamePlayEmote, gamePlayFlying, /gameplay/death
xref out, count to off\_, that -1 is func index

### RaknetTick

Search for this function (RaknetTick calls this), that almost never changes:
4C 8B DC 49 89 5B 10 49 89 6B 18 56 57 41 56 48 81 EC 40 02 00 00
There are a few xrefs, but the one you need should be calling RaknetTick in an if statement that looks like this:
***if ( \*(\_WORD \*)(a1 + 256) != 0xFFFF )***

    {

      v2 = \*(void (\_\_fastcall \*\*)(\_\_int64, \_\_int64, \_BYTE \*))(\*(\_QWORD \*)a1 + 80LL);

      sub\_142BFF380((\_\_int64)v14);

      v2(a1, a1 + 256, v14);

      sub\_14089FBC0(v14);

    }



### HudCursorRenderer::render

look for textures/ui/cross\_hair or ui\_crosshair, first function should be it

### ItemRenderer::render

Look for this number 0x80100240C00

### HudMobEffectsRenderer::render

textures/ui/hud\_mob\_effect\_background

### ActorCollision::isOnGround

minecraft:arrow\_spell\_emitter

### ItemStack::isEnchanted

variable.is\_enchanted (above that)
48 83 EC 38 48 8B 49 10 48 85 C9 74 ?

### Options::getSensitivity

"AutoUpdate" / "gameLocale" / "Options" (used in the func that has those strings)

### ClientInstance::grabMouse

accessibility.game.playerView (somewhere above that)

### Actor::setNametag

"CustomNameVisible" 2nd ref above that is the func (2 arg)

### BlockSource::getBiome

world\_loading\_progress\_screen sub\_14XXXXXX(arg) + 16; last ref (has credits\_screen too)

### ItemRenderer::render (for glm\_rotateRef etc in ItemPhysics)

entity\_alphatest\_change\_color\_glint to vtable then to ItemRenderer::render (like 3rd func in vtable)

### Dimension::getBrightnessDependentFogColor

"A dimension task group" (vtable ref to it)

### ItemStack::getDamageValue

used in a lot of func, could be found by looking through "Damage" string refs, should have something like v1 = \*(\_QWORD \*\*)(a1 + 8) at the start

### GeneralSettingsScreenController::GeneralSettingsScreenController

"GeneralSettingsScreenController", tho on some lower versions this string is not present

### SettingsScreenOnExit

search for "create\_world\_button" / "navigation\_tab" first ref, go to "button.menu\_exit", were in SettingsScreenController::\_registerEventHandlers, above it is vtable, 3rd func is it (if lower then 21.2X its not inlined so go 1 layer deeper)

LevelRendererCamera::\_releaseResources

"Queueing renderchunk for build\&sort" -> LevelBuilder::scheduleChunkBuild -> RenderChunkShared::startRebuild -> RenderChunkGeometry::RenderChunkGeometry -> RenderChunkGeometry::reset -> 4 refs to LevelRendererCamera::\_releaseResources
Or from its vtable at runtime, its near start
Also OP sig to it: 48 89 ? ? ? 48 89 ? ? ? 57 48 83 EC ? 48 8B ? ? ? ? ? 48 8B ? 48 8B ? ? ? ? ? 48 8B ? ? ? ? ? 48 3B ? 74 ? 0F 1F

### MinecraftPackets::createPacket

"UnknownPacket" - above it

### RenderChunkCoordinator::preRenderTick

"renderChunkShared marked dirty"

### LevelRenderer::preRenderUpdate

1 ref out from RenderChunkCoordinator::preRenderTick

### ContainerScreenController::\_handleTakeAll

"button.anvil\_take\_all\_place\_all" ->
v81 = off\_144EA79E0; <- the vtable
v82 = v74;
v84 = \&v81;
sub\_1403E13C0(v56, 2 <- make sure its 2, (\_\_int64)\&v81, (\_\_int64)\&v77, 0);

Then to 3rd func in vtable -> first 3arg func there (or 1 func deeper if below 1.21.40)

## CameraYaw2

4C 8B 49 08 41 BA 9F 0D AF DF
xref of this function contains CameraYaw2

Near end of this function, you will see something like:
\*(float \*)v17 = v13;

or

\*(float \*)v37 = v32;

Make a sig here.



## FluxSwing

func contains 2 variable.is\_first\_person \& 1 "trying to set variable '%s' to #nan/#inf!  setting to zero instea"
Above the first variable.is\_first\_person is the func.

Inside of that func, sub\_140CC2260(a4\[1]) looking func, outside of if (!a6)





## ItemInHandRenderer::renderItem

This one's hard to reverse. This function contains the string "Error: accessing a variable that doesn't currently exist: %s"
But this appears in 10 different places.
Find a function that:

* Has this string showing up in the first few lines of code in the function
* Function that has exactly 8 parameters.

## LocalPlayer::applyTurnDelta

Search for this function sig: 48 83 EC 28 F3 0F 10 11 0F 57 E4
Usually never changes.
Xrefs to one of these is applyTurnDelta.
The one you need should have following params: \_\_int64 \_\_fastcall sub\_14144C220(\_\_int64 a1, float \*a2)

## RakPeer::GetAveragePing

Search for this sig:
74 0E 44 03 C0
There should only be one xref. That function is it.

## Tessellator::vertex

Search seq. of bytes ``"Feedback Circle"`` then go into the func, scroll up until u see a func like ``sub_141132570(a1, 0.0, 0.0, 0.0);`` with a bunch of zeros as an argument, that's the func

## LevelRenderer::renderLevel

Search text ``"Untagged Tessellator Mesh"`` it's gonna show a list of occurences, now go through each one until you come across a function with 7 parameters and then the string gets used like 2 times idk check the structure using previous versions, the function is just an xref to the LevelRenderer::renderLevel. ok enough yapping, once ur in the func with 7 params, get the list of xrefs, go to the first one, then scroll up until you see a function with 3 parameters e.g., ``sub_141E63C30(v8, a2, a3);`` that's the func

## ActorShaderManager::setupShaderParameter

Open hurt color sig, go down, open the func with 9 args under the "else", open 10 arg func at bottom, the 15 arg func call (with 0.00026666667 and -0.00072727271) should be what you're looking for

## SoundEngine::play
Search for this sig: 72 0C F3 0F 5C C8 0F 2F C8 73 03 49 8B C1
This should lead you to the function you need.

## ChatScreenController::tick
Search for this sig: 49 83 F9 0F 76 03 4C 8B 01
This func xrefs to ChatScreenController::tick. 