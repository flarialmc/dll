#pragma once

enum TextureLoadState : int {
    UnloadedBit = 0x0000,
    PendingBit = 0x0001,
    LoadedBit = 0x0002,
    PendingMetadata = 0x0004,
    LoadedMetadata = 0x0008,
};