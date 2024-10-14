#pragma once

enum class ContainerID : char {
    None           = -1,
    Inventory      = 0,
    First          = 1,
    Last           = 100,
    Offhand        = 119,
    Armor          = 120,
    SelectionSlots = 122,
    Chest          = 123,
    PlayerUIOnly   = 124
};