#include "Tags.hpp"
#include <unordered_map>
#include <vector>
#include <string>

static std::vector<std::string> acacia_logs = {
    "acacia_log",
    "acacia_wood",
    "stripped_acacia_log",
    "stripped_acacia_wood"
};

static std::vector<std::string> air = {
    "air",
    "void_air",
    "cave_air"
};

static std::vector<std::string> all_hanging_signs = {
    "oak_hanging_sign",
    "spruce_hanging_sign",
    "birch_hanging_sign",
    "acacia_hanging_sign",
    "cherry_hanging_sign",
    "jungle_hanging_sign",
    "dark_oak_hanging_sign",
    "pale_oak_hanging_sign",
    "crimson_hanging_sign",
    "warped_hanging_sign",
    "mangrove_hanging_sign",
    "bamboo_hanging_sign",
    "oak_wall_hanging_sign",
    "spruce_wall_hanging_sign",
    "birch_wall_hanging_sign",
    "acacia_wall_hanging_sign",
    "cherry_wall_hanging_sign",
    "jungle_wall_hanging_sign",
    "dark_oak_wall_hanging_sign",
    "pale_oak_wall_hanging_sign",
    "crimson_wall_hanging_sign",
    "warped_wall_hanging_sign",
    "mangrove_wall_hanging_sign",
    "bamboo_wall_hanging_sign"
};

static std::vector<std::string> all_signs = {
    "oak_sign",
    "spruce_sign",
    "birch_sign",
    "acacia_sign",
    "jungle_sign",
    "dark_oak_sign",
    "pale_oak_sign",
    "crimson_sign",
    "warped_sign",
    "mangrove_sign",
    "bamboo_sign",
    "cherry_sign",
    "oak_wall_sign",
    "spruce_wall_sign",
    "birch_wall_sign",
    "acacia_wall_sign",
    "jungle_wall_sign",
    "dark_oak_wall_sign",
    "pale_oak_wall_sign",
    "crimson_wall_sign",
    "warped_wall_sign",
    "mangrove_wall_sign",
    "bamboo_wall_sign",
    "cherry_wall_sign",
    "oak_hanging_sign",
    "spruce_hanging_sign",
    "birch_hanging_sign",
    "acacia_hanging_sign",
    "cherry_hanging_sign",
    "jungle_hanging_sign",
    "dark_oak_hanging_sign",
    "pale_oak_hanging_sign",
    "crimson_hanging_sign",
    "warped_hanging_sign",
    "mangrove_hanging_sign",
    "bamboo_hanging_sign",
    "oak_wall_hanging_sign",
    "spruce_wall_hanging_sign",
    "birch_wall_hanging_sign",
    "acacia_wall_hanging_sign",
    "cherry_wall_hanging_sign",
    "jungle_wall_hanging_sign",
    "dark_oak_wall_hanging_sign",
    "pale_oak_wall_hanging_sign",
    "crimson_wall_hanging_sign",
    "warped_wall_hanging_sign",
    "mangrove_wall_hanging_sign",
    "bamboo_wall_hanging_sign"
};

static std::vector<std::string> ancient_city_replaceable = {
    "deepslate",
    "deepslate_bricks",
    "deepslate_tiles",
    "deepslate_brick_slab",
    "deepslate_tile_slab",
    "cobbled_deepslate",
    "cobbled_deepslate_slab",
    "cobbled_deepslate_stairs",
    "cobbled_deepslate_wall",
    "polished_deepslate",
    "polished_deepslate_slab",
    "polished_deepslate_stairs",
    "polished_deepslate_wall",
    "deepslate_brick_stairs",
    "deepslate_brick_wall",
    "deepslate_tile_stairs",
    "deepslate_tile_wall",
    "cracked_deepslate_bricks",
    "cracked_deepslate_tiles"
};

static std::vector<std::string> anvil = {
    "anvil",
    "chipped_anvil",
    "damaged_anvil"
};

static std::vector<std::string> axolotl_always_hostiles = {
    "tropical_fish",
    "pufferfish",
    "salmon",
    "cod",
    "squid",
    "glow_squid"
};

static std::vector<std::string> azalea_grows_on = {
    "clay",
    "coarse_dirt",
    "dirt",
    "farmland",
    "grass_block",
    "moss_block",
    "mud",
    "muddy_mangrove_roots",
    "mycelium",
    "podzol",
    "rooted_dirt",
    "sand",
    "red_sand"
};

static std::vector<std::string> azalea_root_replaceable = {
    "dirt",
    "grass_block",
    "podzol",
    "coarse_dirt",
    "mycelium",
    "rooted_dirt",
    "moss_block",
    "mud",
    "clay",
    "sand",
    "red_sand",
    "gravel",
    "stone",
    "granite",
    "diorite",
    "andesite",
    "deepslate",
    "tuff",
    "calcite",
    "smooth_basalt"
};

static std::vector<std::string> bamboo_blocks = {
    "bamboo_block",
    "stripped_bamboo_block"
};

static std::vector<std::string> bamboo_plantable_on = {
    "bamboo",
    "bamboo_sapling",
    "dirt",
    "grass_block",
    "gravel",
    "mycelium",
    "podzol",
    "coarse_dirt",
    "sand",
    "red_sand",
    "moss_block",
    "mud",
    "muddy_mangrove_roots"
};

static std::vector<std::string> banners = {
    "white_banner",
    "orange_banner",
    "magenta_banner",
    "light_blue_banner",
    "yellow_banner",
    "lime_banner",
    "pink_banner",
    "gray_banner",
    "light_gray_banner",
    "cyan_banner",
    "purple_banner",
    "blue_banner",
    "brown_banner",
    "green_banner",
    "red_banner",
    "black_banner"
};

static std::vector<std::string> base_stone_nether = {
    "netherrack",
    "basalt",
    "blackstone"
};

static std::vector<std::string> base_stone_overworld = {
    "stone",
    "granite",
    "diorite",
    "andesite",
    "deepslate",
    "tuff"
};

static std::vector<std::string> beacon_base_blocks = {
    "emerald_block",
    "diamond_block",
    "gold_block",
    "iron_block",
    "netherite_block"
};

static std::vector<std::string> beds = {
    "red_bed",
    "black_bed",
    "blue_bed",
    "brown_bed",
    "cyan_bed",
    "gray_bed",
    "green_bed",
    "light_blue_bed",
    "light_gray_bed",
    "lime_bed",
    "magenta_bed",
    "orange_bed",
    "pink_bed",
    "purple_bed",
    "white_bed",
    "yellow_bed"
};

static std::vector<std::string> bee_growables = {
    "sweet_berry_bush",
    "cave_vines",
    "cave_vines_plant",
    "crops",
    "pumpkin_stem",
    "attached_pumpkin_stem",
    "melon_stem",
    "attached_melon_stem",
    "beetroots",
    "carrots",
    "potatoes",
    "torchflower_crop",
    "pitcher_crop"
};

static std::vector<std::string> beehives = {
    "bee_nest",
    "beehive"
};

static std::vector<std::string> big_dripleaf_placeable = {
    "clay",
    "coarse_dirt",
    "dirt",
    "farmland",
    "grass_block",
    "moss_block",
    "mud",
    "muddy_mangrove_roots",
    "mycelium",
    "podzol",
    "rooted_dirt"
};

static std::vector<std::string> birch_logs = {
    "birch_log",
    "birch_wood",
    "stripped_birch_log",
    "stripped_birch_wood"
};

static std::vector<std::string> buttons = {
    "stone_button",
    "oak_button",
    "spruce_button",
    "birch_button",
    "jungle_button",
    "acacia_button",
    "cherry_button",
    "dark_oak_button",
    "pale_oak_button",
    "mangrove_button",
    "bamboo_button",
    "crimson_button",
    "warped_button",
    "polished_blackstone_button"
};

static std::vector<std::string> campfires = {
    "campfire",
    "soul_campfire"
};

static std::vector<std::string> candle_cakes = {
    "candle_cake",
    "white_candle_cake",
    "orange_candle_cake",
    "magenta_candle_cake",
    "light_blue_candle_cake",
    "yellow_candle_cake",
    "lime_candle_cake",
    "pink_candle_cake",
    "gray_candle_cake",
    "light_gray_candle_cake",
    "cyan_candle_cake",
    "purple_candle_cake",
    "blue_candle_cake",
    "brown_candle_cake",
    "green_candle_cake",
    "red_candle_cake",
    "black_candle_cake"
};

static std::vector<std::string> candles = {
    "candle",
    "white_candle",
    "orange_candle",
    "magenta_candle",
    "light_blue_candle",
    "yellow_candle",
    "lime_candle",
    "pink_candle",
    "gray_candle",
    "light_gray_candle",
    "cyan_candle",
    "purple_candle",
    "blue_candle",
    "brown_candle",
    "green_candle",
    "red_candle",
    "black_candle"
};

static std::vector<std::string> cauldrons = {
    "cauldron",
    "water_cauldron",
    "lava_cauldron",
    "powder_snow_cauldron"
};

static std::vector<std::string> cave_vines = {
    "cave_vines",
    "cave_vines_plant"
};

static std::vector<std::string> cherry_logs = {
    "cherry_log",
    "cherry_wood",
    "stripped_cherry_log",
    "stripped_cherry_wood"
};

static std::vector<std::string> climbable = {
    "ladder",
    "vine",
    "scaffolding",
    "weeping_vines",
    "weeping_vines_plant",
    "twisting_vines",
    "twisting_vines_plant",
    "cave_vines",
    "cave_vines_plant"
};

static std::vector<std::string> coal_ores = {
    "coal_ore",
    "deepslate_coal_ore"
};

static std::vector<std::string> completes_find_tree_tutorial = {
    "oak_log",
    "spruce_log",
    "birch_log",
    "jungle_log",
    "acacia_log",
    "cherry_log",
    "dark_oak_log",
    "pale_oak_log",
    "mangrove_log",
    "crimson_stem",
    "warped_stem",
    "bamboo_block"
};

static std::vector<std::string> concrete_powder = {
    "white_concrete_powder",
    "orange_concrete_powder",
    "magenta_concrete_powder",
    "light_blue_concrete_powder",
    "yellow_concrete_powder",
    "lime_concrete_powder",
    "pink_concrete_powder",
    "gray_concrete_powder",
    "light_gray_concrete_powder",
    "cyan_concrete_powder",
    "purple_concrete_powder",
    "blue_concrete_powder",
    "brown_concrete_powder",
    "green_concrete_powder",
    "red_concrete_powder",
    "black_concrete_powder"
};

static std::vector<std::string> copper_ores = {
    "copper_ore",
    "deepslate_copper_ore"
};

static std::vector<std::string> coral_blocks = {
    "tube_coral_block",
    "brain_coral_block",
    "bubble_coral_block",
    "fire_coral_block",
    "horn_coral_block",
    "dead_tube_coral_block",
    "dead_brain_coral_block",
    "dead_bubble_coral_block",
    "dead_fire_coral_block",
    "dead_horn_coral_block"
};

static std::vector<std::string> coral_plants = {
    "tube_coral",
    "brain_coral",
    "bubble_coral",
    "fire_coral",
    "horn_coral",
    "dead_tube_coral",
    "dead_brain_coral",
    "dead_bubble_coral",
    "dead_fire_coral",
    "dead_horn_coral"
};

static std::vector<std::string> corals = {
    "tube_coral",
    "brain_coral",
    "bubble_coral",
    "fire_coral",
    "horn_coral",
    "tube_coral_fan",
    "brain_coral_fan",
    "bubble_coral_fan",
    "fire_coral_fan",
    "horn_coral_fan",
    "tube_coral_wall_fan",
    "brain_coral_wall_fan",
    "bubble_coral_wall_fan",
    "fire_coral_wall_fan",
    "horn_coral_wall_fan"
};

static std::vector<std::string> crimson_stems = {
    "crimson_stem",
    "crimson_hyphae",
    "stripped_crimson_stem",
    "stripped_crimson_hyphae"
};

static std::vector<std::string> crops = {
    "beetroots",
    "carrots",
    "potatoes",
    "wheat"
};

static std::vector<std::string> crystal_sound_blocks = {
    "amethyst_block",
    "budding_amethyst"
};

static std::vector<std::string> dark_oak_logs = {
    "dark_oak_log",
    "dark_oak_wood",
    "stripped_dark_oak_log",
    "stripped_dark_oak_wood"
};

static std::vector<std::string> dead_bush_may_place_on = {
    "sand",
    "red_sand",
    "terracotta",
    "white_terracotta",
    "orange_terracotta",
    "magenta_terracotta",
    "light_blue_terracotta",
    "yellow_terracotta",
    "lime_terracotta",
    "pink_terracotta",
    "gray_terracotta",
    "light_gray_terracotta",
    "cyan_terracotta",
    "purple_terracotta",
    "blue_terracotta",
    "brown_terracotta",
    "green_terracotta",
    "red_terracotta",
    "black_terracotta",
    "dirt",
    "coarse_dirt",
    "podzol"
};

static std::vector<std::string> deepslate_ore_replaceables = {
    "deepslate"
};

static std::vector<std::string> diamond_ores = {
    "diamond_ore",
    "deepslate_diamond_ore"
};

static std::vector<std::string> dirt = {
    "dirt",
    "grass_block",
    "podzol",
    "coarse_dirt",
    "mycelium",
    "rooted_dirt",
    "mud",
    "muddy_mangrove_roots"
};

static std::vector<std::string> doors = {
    "oak_door",
    "spruce_door",
    "birch_door",
    "jungle_door",
    "acacia_door",
    "cherry_door",
    "dark_oak_door",
    "pale_oak_door",
    "mangrove_door",
    "bamboo_door",
    "crimson_door",
    "warped_door",
    "iron_door",
    "copper_door",
    "exposed_copper_door",
    "weathered_copper_door",
    "oxidized_copper_door",
    "waxed_copper_door",
    "waxed_exposed_copper_door",
    "waxed_weathered_copper_door",
    "waxed_oxidized_copper_door"
};

static std::vector<std::string> dragon_immune = {
    "bedrock",
    "obsidian",
    "crying_obsidian",
    "end_stone",
    "iron_bars",
    "respawn_anchor"
};

static std::vector<std::string> dragon_transparent = {
    "light",
    "fire",
    "soul_fire",
    "water",
    "lava",
    "bubble_column"
};

static std::vector<std::string> dripstone_replaceable_blocks = {
    "stone",
    "deepslate",
    "dripstone_block",
    "pointed_dripstone"
};

static std::vector<std::string> emerald_ores = {
    "emerald_ore",
    "deepslate_emerald_ore"
};

static std::vector<std::string> enchantment_power_provider = {
    "bookshelf",
    "chiseled_bookshelf"
};

static std::vector<std::string> enchantment_power_transmitter = {
    "air",
    "tall_grass",
    "fern",
    "large_fern",
    "tall_grass"
};

static std::vector<std::string> enderman_holdable = {
    "grass_block",
    "dirt",
    "coarse_dirt",
    "rooted_dirt",
    "podzol",
    "mycelium",
    "sand",
    "red_sand",
    "gravel",
    "brown_mushroom",
    "red_mushroom",
    "tnt",
    "cactus",
    "clay",
    "pumpkin",
    "carved_pumpkin",
    "melon",
    "crimson_fungus",
    "crimson_nylium",
    "crimson_roots",
    "warped_fungus",
    "warped_nylium",
    "warped_roots"
};

static std::vector<std::string> fall_damage_resetting = {
    "hay_block",
    "dripstone_block",
    "pointed_dripstone",
    "big_dripleaf",
    "water",
    "powder_snow",
    "cobweb",
    "sweet_berry_bush"
};

static std::vector<std::string> features_cannot_replace = {
    "bedrock",
    "spawner",
    "chest",
    "end_portal_frame"
};

static std::vector<std::string> fence_gates = {
    "oak_fence_gate",
    "spruce_fence_gate",
    "birch_fence_gate",
    "jungle_fence_gate",
    "acacia_fence_gate",
    "cherry_fence_gate",
    "dark_oak_fence_gate",
    "pale_oak_fence_gate",
    "mangrove_fence_gate",
    "bamboo_fence_gate",
    "crimson_fence_gate",
    "warped_fence_gate"
};

static std::vector<std::string> fences = {
    "oak_fence",
    "spruce_fence",
    "birch_fence",
    "jungle_fence",
    "acacia_fence",
    "cherry_fence",
    "dark_oak_fence",
    "pale_oak_fence",
    "mangrove_fence",
    "bamboo_fence",
    "crimson_fence",
    "warped_fence",
    "nether_brick_fence"
};

static std::vector<std::string> fire = {
    "fire",
    "soul_fire"
};

static std::vector<std::string> flowers = {
    "dandelion",
    "poppy",
    "blue_orchid",
    "allium",
    "azure_bluet",
    "red_tulip",
    "orange_tulip",
    "white_tulip",
    "pink_tulip",
    "oxeye_daisy",
    "cornflower",
    "lily_of_the_valley",
    "torchflower",
    "wither_rose"
};

static std::vector<std::string> fox_food = {
    "sweet_berries",
    "glow_berries"
};

static std::vector<std::string> frog_prefer_jump_to = {
    "water",
    "lily_pad"
};

static std::vector<std::string> geode_invalid_blocks = {
    "bedrock",
    "water",
    "lava",
    "ice",
    "packed_ice",
    "blue_ice"
};

static std::vector<std::string> gold_ores = {
    "gold_ore",
    "deepslate_gold_ore",
    "nether_gold_ore"
};

static std::vector<std::string> guarded_by_piglins = {
    "gold_block",
    "barrel",
    "chest",
    "ender_chest",
    "gilded_blackstone",
    "trapped_chest",
    "shulker_box",
    "black_shulker_box",
    "blue_shulker_box",
    "brown_shulker_box",
    "cyan_shulker_box",
    "gray_shulker_box",
    "green_shulker_box",
    "light_blue_shulker_box",
    "light_gray_shulker_box",
    "lime_shulker_box",
    "magenta_shulker_box",
    "orange_shulker_box",
    "pink_shulker_box",
    "purple_shulker_box",
    "red_shulker_box",
    "white_shulker_box",
    "yellow_shulker_box"
};

static std::vector<std::string> hoglin_repellents = {
    "warped_fungus",
    "nether_portal",
    "respawn_anchor"
};

static std::vector<std::string> ice = {
    "ice",
    "packed_ice",
    "blue_ice",
    "frosted_ice"
};

static std::vector<std::string> ignored_by_piglin_babies = {
    "leather"
};

static std::vector<std::string> impermeable = {
    "glass",
    "white_stained_glass",
    "orange_stained_glass",
    "magenta_stained_glass",
    "light_blue_stained_glass",
    "yellow_stained_glass",
    "lime_stained_glass",
    "pink_stained_glass",
    "gray_stained_glass",
    "light_gray_stained_glass",
    "cyan_stained_glass",
    "purple_stained_glass",
    "blue_stained_glass",
    "brown_stained_glass",
    "green_stained_glass",
    "red_stained_glass",
    "black_stained_glass",
    "tinted_glass"
};

static std::vector<std::string> infiniburn_end = {
    "bedrock",
    "netherrack",
    "magma_block"
};

static std::vector<std::string> infiniburn_nether = {
    "netherrack",
    "magma_block"
};

static std::vector<std::string> infiniburn_overworld = {
    "netherrack",
    "magma_block"
};

static std::vector<std::string> inside_step_sound_blocks = {
    "lily_pad",
    "big_dripleaf",
    "big_dripleaf_stem",
    "small_dripleaf"
};

static std::vector<std::string> iron_ores = {
    "iron_ore",
    "deepslate_iron_ore"
};

static std::vector<std::string> jungle_logs = {
    "jungle_log",
    "jungle_wood",
    "stripped_jungle_log",
    "stripped_jungle_wood"
};

static std::vector<std::string> lapis_ores = {
    "lapis_ore",
    "deepslate_lapis_ore"
};

static std::vector<std::string> lava_pool_stone_cannot_replace = {
    "bedrock",
    "nether_bricks",
    "nether_brick_fence",
    "nether_brick_stairs",
    "nether_wart",
    "chest",
    "spawner"
};

static std::vector<std::string> leaves = {
    "oak_leaves",
    "spruce_leaves",
    "birch_leaves",
    "jungle_leaves",
    "acacia_leaves",
    "cherry_leaves",
    "dark_oak_leaves",
    "pale_oak_leaves",
    "mangrove_leaves",
    "azalea_leaves",
    "flowering_azalea_leaves"
};

static std::vector<std::string> lectern_books = {
    "written_book",
    "writable_book"
};

static std::vector<std::string> logs = {
    "oak_log",
    "spruce_log",
    "birch_log",
    "jungle_log",
    "acacia_log",
    "cherry_log",
    "dark_oak_log",
    "pale_oak_log",
    "mangrove_log",
    "crimson_stem",
    "warped_stem",
    "bamboo_block"
};

static std::vector<std::string> logs_that_burn = {
    "oak_log",
    "spruce_log",
    "birch_log",
    "jungle_log",
    "acacia_log",
    "cherry_log",
    "dark_oak_log",
    "pale_oak_log",
    "mangrove_log",
    "bamboo_block",
    "oak_wood",
    "spruce_wood",
    "birch_wood",
    "jungle_wood",
    "acacia_wood",
    "cherry_wood",
    "dark_oak_wood",
    "pale_oak_wood",
    "mangrove_wood",
    "stripped_oak_log",
    "stripped_spruce_log",
    "stripped_birch_log",
    "stripped_jungle_log",
    "stripped_acacia_log",
    "stripped_cherry_log",
    "stripped_dark_oak_log",
    "stripped_pale_oak_log",
    "stripped_mangrove_log",
    "stripped_oak_wood",
    "stripped_spruce_wood",
    "stripped_birch_wood",
    "stripped_jungle_wood",
    "stripped_acacia_wood",
    "stripped_cherry_wood",
    "stripped_dark_oak_wood",
    "stripped_pale_oak_wood",
    "stripped_mangrove_wood",
    "stripped_bamboo_block"
};

static std::vector<std::string> lush_ground_replaceable = {
    "clay",
    "coarse_dirt",
    "dirt",
    "farmland",
    "grass_block",
    "gravel",
    "moss_block",
    "mud",
    "muddy_mangrove_roots",
    "mycelium",
    "podzol",
    "rooted_dirt",
    "sand",
    "red_sand",
    "stone",
    "andesite",
    "diorite",
    "granite",
    "deepslate",
    "tuff",
    "calcite",
    "smooth_basalt"
};

static std::vector<std::string> maintain_farmland = {
    "pumpkin_stem",
    "attached_pumpkin_stem",
    "melon_stem",
    "attached_melon_stem",
    "beetroots",
    "carrots",
    "potatoes",
    "wheat",
    "torchflower_crop",
    "pitcher_crop"
};

static std::vector<std::string> mangrove_logs = {
    "mangrove_log",
    "mangrove_wood",
    "stripped_mangrove_log",
    "stripped_mangrove_wood"
};

static std::vector<std::string> mangrove_logs_can_grow_through = {
    "air",
    "dirt",
    "grass_block",
    "podzol",
    "coarse_dirt",
    "mycelium",
    "rooted_dirt",
    "moss_block",
    "mud",
    "muddy_mangrove_roots",
    "water",
    "flowing_water",
    "vine",
    "small_dripleaf",
    "big_dripleaf",
    "big_dripleaf_stem",
    "mangrove_roots",
    "mangrove_leaves"
};

static std::vector<std::string> mangrove_roots_can_grow_through = {
    "air",
    "water",
    "flowing_water",
    "lava",
    "flowing_lava"
};

static std::vector<std::string> mineable_axe = {
    "oak_log",
    "spruce_log",
    "birch_log",
    "jungle_log",
    "acacia_log",
    "cherry_log",
    "dark_oak_log",
    "pale_oak_log",
    "mangrove_log",
    "oak_wood",
    "spruce_wood",
    "birch_wood",
    "jungle_wood",
    "acacia_wood",
    "cherry_wood",
    "dark_oak_wood",
    "pale_oak_wood",
    "mangrove_wood",
    "oak_leaves",
    "spruce_leaves",
    "birch_leaves",
    "jungle_leaves",
    "acacia_leaves",
    "cherry_leaves",
    "dark_oak_leaves",
    "pale_oak_leaves",
    "mangrove_leaves",
    "azalea_leaves",
    "flowering_azalea_leaves",
    "stripped_oak_log",
    "stripped_spruce_log",
    "stripped_birch_log",
    "stripped_jungle_log",
    "stripped_acacia_log",
    "stripped_cherry_log",
    "stripped_dark_oak_log",
    "stripped_pale_oak_log",
    "stripped_mangrove_log",
    "stripped_oak_wood",
    "stripped_spruce_wood",
    "stripped_birch_wood",
    "stripped_jungle_wood",
    "stripped_acacia_wood",
    "stripped_cherry_wood",
    "stripped_dark_oak_wood",
    "stripped_pale_oak_wood",
    "stripped_mangrove_wood",
    "oak_planks",
    "spruce_planks",
    "birch_planks",
    "jungle_planks",
    "acacia_planks",
    "cherry_planks",
    "dark_oak_planks",
    "pale_oak_planks",
    "mangrove_planks",
    "bamboo_planks",
    "crimson_planks",
    "warped_planks",
    "bamboo_mosaic",
    "bookshelf",
    "chiseled_bookshelf",
    "chest",
    "trapped_chest",
    "crafting_table",
    "jukebox",
    "note_block",
    "brown_mushroom_block",
    "red_mushroom_block",
    "mushroom_stem",
    "jack_o_lantern",
    "melon",
    "pumpkin",
    "carved_pumpkin",
    "banner",
    "white_banner",
    "orange_banner",
    "magenta_banner",
    "light_blue_banner",
    "yellow_banner",
    "lime_banner",
    "pink_banner",
    "gray_banner",
    "light_gray_banner",
    "cyan_banner",
    "purple_banner",
    "blue_banner",
    "brown_banner",
    "green_banner",
    "red_banner",
    "black_banner",
    "wall_banner",
    "white_wall_banner",
    "orange_wall_banner",
    "magenta_wall_banner",
    "light_blue_wall_banner",
    "yellow_wall_banner",
    "lime_wall_banner",
    "pink_wall_banner",
    "gray_wall_banner",
    "light_gray_wall_banner",
    "cyan_wall_banner",
    "purple_wall_banner",
    "blue_wall_banner",
    "brown_wall_banner",
    "green_wall_banner",
    "red_wall_banner",
    "black_wall_banner",
    "bamboo",
    "bamboo_sapling",
    "scaffolding",
    "composter",
    "barrel",
    "cartography_table",
    "fletching_table",
    "smithing_table",
    "loom",
    "lectern",
    "bamboo_block",
    "stripped_bamboo_block",
    "crimson_stem",
    "warped_stem",
    "stripped_crimson_stem",
    "stripped_warped_stem",
    "crimson_hyphae",
    "warped_hyphae",
    "stripped_crimson_hyphae",
    "stripped_warped_hyphae",
    "crimson_fungus",
    "warped_fungus",
    "nether_wart_block",
    "warped_wart_block",
    "shroomlight",
    "vine",
    "ladder",
    "bee_nest",
    "beehive",
    "daylight_detector",
    "campfire",
    "soul_campfire",
    "mangrove_roots",
    "muddy_mangrove_roots"
};

static std::vector<std::string> mineable_hoe = {
    "hay_block",
    "dried_kelp_block",
    "target",
    "shroomlight",
    "nether_wart_block",
    "warped_wart_block",
    "sponge",
    "wet_sponge",
    "jungle_leaves",
    "oak_leaves",
    "spruce_leaves",
    "dark_oak_leaves",
    "acacia_leaves",
    "cherry_leaves",
    "birch_leaves",
    "pale_oak_leaves",
    "azalea_leaves",
    "mangrove_leaves",
    "flowering_azalea_leaves",
    "sculk_sensor",
    "calibrated_sculk_sensor"
};

static std::vector<std::string> mineable_pickaxe = {
    "stone",
    "granite",
    "polished_granite",
    "diorite",
    "polished_diorite",
    "andesite",
    "polished_andesite",
    "deepslate",
    "cobbled_deepslate",
    "polished_deepslate",
    "calcite",
    "tuff",
    "polished_tuff",
    "tuff_bricks",
    "chiseled_tuff",
    "chiseled_tuff_bricks",
    "dripstone_block",
    "pointed_dripstone",
    "cobblestone",
    "mossy_cobblestone",
    "smooth_stone",
    "stone_bricks",
    "cracked_stone_bricks",
    "chiseled_stone_bricks",
    "mossy_stone_bricks",
    "infested_stone",
    "infested_cobblestone",
    "infested_stone_bricks",
    "infested_mossy_stone_bricks",
    "infested_cracked_stone_bricks",
    "infested_chiseled_stone_bricks",
    "infested_deepslate",
    "deepslate_bricks",
    "cracked_deepslate_bricks",
    "deepslate_tiles",
    "cracked_deepslate_tiles",
    "chiseled_deepslate",
    "reinforced_deepslate",
    "sandstone",
    "chiseled_sandstone",
    "cut_sandstone",
    "smooth_sandstone",
    "red_sandstone",
    "chiseled_red_sandstone",
    "cut_red_sandstone",
    "smooth_red_sandstone",
    "sea_lantern",
    "prismarine",
    "prismarine_bricks",
    "dark_prismarine",
    "netherrack",
    "nether_bricks",
    "cracked_nether_bricks",
    "chiseled_nether_bricks",
    "nether_brick_fence",
    "nether_brick_stairs",
    "nether_brick_slab",
    "nether_brick_wall",
    "red_nether_bricks",
    "red_nether_brick_stairs",
    "red_nether_brick_slab",
    "red_nether_brick_wall",
    "basalt",
    "smooth_basalt",
    "polished_basalt",
    "blackstone",
    "gilded_blackstone",
    "polished_blackstone",
    "polished_blackstone_bricks",
    "cracked_polished_blackstone_bricks",
    "chiseled_polished_blackstone",
    "end_stone",
    "end_stone_bricks",
    "iron_ore",
    "deepslate_iron_ore",
    "coal_ore",
    "deepslate_coal_ore",
    "nether_gold_ore",
    "gold_ore",
    "deepslate_gold_ore",
    "diamond_ore",
    "deepslate_diamond_ore",
    "emerald_ore",
    "deepslate_emerald_ore",
    "lapis_ore",
    "deepslate_lapis_ore",
    "redstone_ore",
    "deepslate_redstone_ore",
    "nether_quartz_ore",
    "ancient_debris",
    "copper_ore",
    "deepslate_copper_ore",
    "iron_block",
    "copper_block",
    "exposed_copper",
    "weathered_copper",
    "oxidized_copper",
    "cut_copper",
    "exposed_cut_copper",
    "weathered_cut_copper",
    "oxidized_cut_copper",
    "cut_copper_stairs",
    "exposed_cut_copper_stairs",
    "weathered_cut_copper_stairs",
    "oxidized_cut_copper_stairs",
    "cut_copper_slab",
    "exposed_cut_copper_slab",
    "weathered_cut_copper_slab",
    "oxidized_cut_copper_slab",
    "waxed_copper_block",
    "waxed_exposed_copper",
    "waxed_weathered_copper",
    "waxed_oxidized_copper",
    "waxed_cut_copper",
    "waxed_exposed_cut_copper",
    "waxed_weathered_cut_copper",
    "waxed_oxidized_cut_copper",
    "waxed_cut_copper_stairs",
    "waxed_exposed_cut_copper_stairs",
    "waxed_weathered_cut_copper_stairs",
    "waxed_oxidized_cut_copper_stairs",
    "waxed_cut_copper_slab",
    "waxed_exposed_cut_copper_slab",
    "waxed_weathered_cut_copper_slab",
    "waxed_oxidized_cut_copper_slab",
    "gold_block",
    "diamond_block",
    "emerald_block",
    "lapis_block",
    "redstone_block",
    "quartz_block",
    "smooth_quartz",
    "quartz_bricks",
    "chiseled_quartz_block",
    "quartz_pillar",
    "amethyst_block",
    "budding_amethyst",
    "iron_door",
    "iron_trapdoor",
    "iron_bars",
    "chain",
    "brewing_stand",
    "cauldron",
    "water_cauldron",
    "lava_cauldron",
    "powder_snow_cauldron",
    "stonecutter",
    "bell",
    "lantern",
    "soul_lantern",
    "shulker_box",
    "white_shulker_box",
    "orange_shulker_box",
    "magenta_shulker_box",
    "light_blue_shulker_box",
    "yellow_shulker_box",
    "lime_shulker_box",
    "pink_shulker_box",
    "gray_shulker_box",
    "light_gray_shulker_box",
    "cyan_shulker_box",
    "purple_shulker_box",
    "blue_shulker_box",
    "brown_shulker_box",
    "green_shulker_box",
    "red_shulker_box",
    "black_shulker_box",
    "observer",
    "smoker",
    "blast_furnace",
    "furnace",
    "dispenser",
    "dropper",
    "spawner",
    "diamond_ore",
    "deepslate_diamond_ore",
    "emerald_ore",
    "deepslate_emerald_ore",
    "gold_ore",
    "deepslate_gold_ore",
    "iron_ore",
    "deepslate_iron_ore",
    "nether_gold_ore",
    "nether_quartz_ore",
    "redstone_ore",
    "deepslate_redstone_ore",
    "coal_ore",
    "deepslate_coal_ore",
    "copper_ore",
    "deepslate_copper_ore",
    "lapis_ore",
    "deepslate_lapis_ore",
    "ancient_debris",
    "raw_iron_block",
    "raw_copper_block",
    "raw_gold_block",
    "glowstone",
    "redstone_lamp",
    "conduit",
    "beacon",
    "end_gateway",
    "end_portal_frame",
    "ender_chest",
    "enchanting_table",
    "anvil",
    "damaged_anvil",
    "chipped_anvil",
    "grindstone",
    "dragon_egg",
    "turtle_egg",
    "sniffer_egg",
    "respawn_anchor",
    "crying_obsidian",
    "obsidian",
    "block_of_netherite",
    "lodestone",
    "rail",
    "powered_rail",
    "detector_rail",
    "activator_rail",
    "lightning_rod",
    "pointed_dripstone",
    "dripstone_block",
    "copper_door",
    "exposed_copper_door",
    "oxidized_copper_door",
    "weathered_copper_door",
    "waxed_copper_door",
    "waxed_exposed_copper_door",
    "waxed_oxidized_copper_door",
    "waxed_weathered_copper_door",
    "copper_trapdoor",
    "exposed_copper_trapdoor",
    "oxidized_copper_trapdoor",
    "weathered_copper_trapdoor",
    "waxed_copper_trapdoor",
    "waxed_exposed_copper_trapdoor",
    "waxed_oxidized_copper_trapdoor",
    "waxed_weathered_copper_trapdoor",
    "copper_grate",
    "exposed_copper_grate",
    "weathered_copper_grate",
    "oxidized_copper_grate",
    "waxed_copper_grate",
    "waxed_exposed_copper_grate",
    "waxed_weathered_copper_grate",
    "waxed_oxidized_copper_grate",
    "copper_bulb",
    "exposed_copper_bulb",
    "weathered_copper_bulb",
    "oxidized_copper_bulb",
    "waxed_copper_bulb",
    "waxed_exposed_copper_bulb",
    "waxed_weathered_copper_bulb",
    "waxed_oxidized_copper_bulb",
    "trial_spawner",
    "vault",
    "heavy_core"
};

static std::vector<std::string> mineable_shovel = {
    "clay",
    "dirt",
    "coarse_dirt",
    "podzol",
    "farmland",
    "grass_block",
    "gravel",
    "mycelium",
    "sand",
    "red_sand",
    "snow_block",
    "snow",
    "soul_sand",
    "soul_soil",
    "concrete_powder",
    "white_concrete_powder",
    "orange_concrete_powder",
    "magenta_concrete_powder",
    "light_blue_concrete_powder",
    "yellow_concrete_powder",
    "lime_concrete_powder",
    "pink_concrete_powder",
    "gray_concrete_powder",
    "light_gray_concrete_powder",
    "cyan_concrete_powder",
    "purple_concrete_powder",
    "blue_concrete_powder",
    "brown_concrete_powder",
    "green_concrete_powder",
    "red_concrete_powder",
    "black_concrete_powder",
    "suspicious_sand",
    "suspicious_gravel",
    "rooted_dirt",
    "mud",
    "muddy_mangrove_roots"
};

static std::vector<std::string> moss_replaceable = {
    "stone",
    "granite",
    "diorite",
    "andesite",
    "deepslate",
    "tuff",
    "calcite",
    "smooth_basalt",
    "cave_air",
    "dirt",
    "coarse_dirt",
    "rooted_dirt",
    "podzol",
    "moss_block"
};

static std::vector<std::string> mushroom_grow_block = {
    "mycelium",
    "podzol",
    "crimson_nylium",
    "warped_nylium"
};

static std::vector<std::string> needs_diamond_tool = {
    "obsidian",
    "crying_obsidian",
    "netherite_block",
    "respawn_anchor",
    "ancient_debris"
};

static std::vector<std::string> needs_iron_tool = {
    "diamond_ore",
    "deepslate_diamond_ore",
    "emerald_ore",
    "deepslate_emerald_ore",
    "gold_ore",
    "deepslate_gold_ore",
    "redstone_ore",
    "deepslate_redstone_ore"
};

static std::vector<std::string> needs_stone_tool = {
    "iron_ore",
    "deepslate_iron_ore",
    "copper_ore",
    "deepslate_copper_ore",
    "lapis_ore",
    "deepslate_lapis_ore"
};

static std::vector<std::string> nether_carver_replaceables = {
    "netherrack",
    "basalt",
    "blackstone",
    "nether_bricks",
    "nether_brick_fence",
    "nether_brick_stairs",
    "nether_wart",
    "chest",
    "spawner"
};

static std::vector<std::string> non_flammable_wood = {
    "crimson_stem",
    "stripped_crimson_stem",
    "crimson_hyphae",
    "stripped_crimson_hyphae",
    "crimson_planks",
    "crimson_slab",
    "crimson_pressure_plate",
    "crimson_fence",
    "crimson_trapdoor",
    "crimson_fence_gate",
    "crimson_stairs",
    "crimson_button",
    "crimson_door",
    "crimson_sign",
    "crimson_wall_sign",
    "crimson_hanging_sign",
    "crimson_wall_hanging_sign",
    "warped_stem",
    "stripped_warped_stem",
    "warped_hyphae",
    "stripped_warped_hyphae",
    "warped_planks",
    "warped_slab",
    "warped_pressure_plate",
    "warped_fence",
    "warped_trapdoor",
    "warped_fence_gate",
    "warped_stairs",
    "warped_button",
    "warped_door",
    "warped_sign",
    "warped_wall_sign",
    "warped_hanging_sign",
    "warped_wall_hanging_sign"
};

static std::vector<std::string> nylium = {
    "crimson_nylium",
    "warped_nylium"
};

static std::vector<std::string> oak_logs = {
    "oak_log",
    "oak_wood",
    "stripped_oak_log",
    "stripped_oak_wood"
};

static std::vector<std::string> occludes_vibration_signals = {
    "wool",
    "white_wool",
    "orange_wool",
    "magenta_wool",
    "light_blue_wool",
    "yellow_wool",
    "lime_wool",
    "pink_wool",
    "gray_wool",
    "light_gray_wool",
    "cyan_wool",
    "purple_wool",
    "blue_wool",
    "brown_wool",
    "green_wool",
    "red_wool",
    "black_wool",
    "wool_carpets",
    "white_carpet",
    "orange_carpet",
    "magenta_carpet",
    "light_blue_carpet",
    "yellow_carpet",
    "lime_carpet",
    "pink_carpet",
    "gray_carpet",
    "light_gray_carpet",
    "cyan_carpet",
    "purple_carpet",
    "blue_carpet",
    "brown_carpet",
    "green_carpet",
    "red_carpet",
    "black_carpet"
};

static std::vector<std::string> overworld_carver_replaceables = {
    "stone",
    "granite",
    "diorite",
    "andesite",
    "deepslate",
    "tuff",
    "calcite",
    "smooth_basalt",
    "dirt",
    "coarse_dirt",
    "podzol",
    "rooted_dirt",
    "grass_block",
    "mycelium",
    "sand",
    "red_sand",
    "gravel",
    "sandstone",
    "red_sandstone",
    "packed_ice",
    "snow_block"
};

static std::vector<std::string> overworld_natural_logs = {
    "oak_log",
    "spruce_log",
    "birch_log",
    "jungle_log",
    "acacia_log",
    "cherry_log",
    "dark_oak_log",
    "pale_oak_log",
    "mangrove_log"
};

static std::vector<std::string> pale_oak_logs = {
    "pale_oak_log",
    "pale_oak_wood",
    "stripped_pale_oak_log",
    "stripped_pale_oak_wood"
};

static std::vector<std::string> parrots_spawnable_on = {
    "grass_block",
    "air",
    "oak_leaves",
    "spruce_leaves",
    "birch_leaves",
    "jungle_leaves",
    "acacia_leaves",
    "cherry_leaves",
    "dark_oak_leaves",
    "pale_oak_leaves",
    "azalea_leaves",
    "flowering_azalea_leaves"
};

static std::vector<std::string> piglin_loved = {
    "gold_block",
    "gilded_blackstone",
    "gold_ore",
    "nether_gold_ore",
    "deepslate_gold_ore",
    "raw_gold_block",
    "gold_ingot",
    "raw_gold",
    "gold_nugget",
    "golden_helmet",
    "golden_chestplate",
    "golden_leggings",
    "golden_boots",
    "golden_sword",
    "golden_pickaxe",
    "golden_axe",
    "golden_shovel",
    "golden_hoe",
    "light_weighted_pressure_plate",
    "golden_horse_armor",
    "golden_apple",
    "enchanted_golden_apple",
    "golden_carrot",
    "gold_ingot",
    "raw_gold",
    "bell",
    "clock",
    "powered_rail",
    "detector_rail"
};

static std::vector<std::string> piglin_repellents = {
    "soul_fire",
    "soul_torch",
    "soul_wall_torch",
    "soul_lantern",
    "soul_campfire"
};

static std::vector<std::string> planks = {
    "oak_planks",
    "spruce_planks",
    "birch_planks",
    "jungle_planks",
    "acacia_planks",
    "cherry_planks",
    "dark_oak_planks",
    "pale_oak_planks",
    "mangrove_planks",
    "bamboo_planks",
    "crimson_planks",
    "warped_planks"
};

static std::vector<std::string> polar_bears_spawnable_on_alternate = {
    "grass_block",
    "ice",
    "packed_ice",
    "snow_block"
};

static std::vector<std::string> portals = {
    "nether_portal",
    "end_portal",
    "end_gateway"
};

static std::vector<std::string> pressure_plates = {
    "light_weighted_pressure_plate",
    "heavy_weighted_pressure_plate",
    "stone_pressure_plate",
    "oak_pressure_plate",
    "spruce_pressure_plate",
    "birch_pressure_plate",
    "jungle_pressure_plate",
    "acacia_pressure_plate",
    "cherry_pressure_plate",
    "dark_oak_pressure_plate",
    "pale_oak_pressure_plate",
    "mangrove_pressure_plate",
    "bamboo_pressure_plate",
    "crimson_pressure_plate",
    "warped_pressure_plate",
    "polished_blackstone_pressure_plate"
};

static std::vector<std::string> prevent_mob_spawning_inside = {
    "rail",
    "powered_rail",
    "detector_rail",
    "activator_rail"
};

static std::vector<std::string> rabbits_spawnable_on = {
    "grass_block",
    "snow",
    "snow_block",
    "sand",
    "desert_sand"
};

static std::vector<std::string> redstone_ores = {
    "redstone_ore",
    "deepslate_redstone_ore"
};

static std::vector<std::string> replaceable = {
    "air",
    "water",
    "lava",
    "tall_grass",
    "large_fern",
    "dead_bush",
    "vine",
    "snow",
    "light",
    "cave_air",
    "void_air",
    "bubble_column",
    "structure_void"
};

static std::vector<std::string> replaceable_by_trees = {
    "air",
    "water",
    "grass",
    "fern",
    "dead_bush",
    "vine",
    "snow",
    "oak_leaves",
    "spruce_leaves",
    "birch_leaves",
    "jungle_leaves",
    "acacia_leaves",
    "cherry_leaves",
    "dark_oak_leaves",
    "pale_oak_leaves",
    "azalea_leaves",
    "flowering_azalea_leaves"
};

static std::vector<std::string> sand = {
    "sand",
    "red_sand",
    "suspicious_sand"
};

static std::vector<std::string> saplings = {
    "oak_sapling",
    "spruce_sapling",
    "birch_sapling",
    "jungle_sapling",
    "acacia_sapling",
    "cherry_sapling",
    "dark_oak_sapling",
    "pale_oak_sapling",
    "mangrove_propagule",
    "azalea",
    "flowering_azalea"
};

static std::vector<std::string> sculk_replaceable = {
    "dirt",
    "coarse_dirt",
    "rooted_dirt",
    "podzol",
    "grass_block",
    "mycelium",
    "sand",
    "red_sand",
    "gravel",
    "clay",
    "stone",
    "granite",
    "diorite",
    "andesite",
    "deepslate",
    "tuff",
    "calcite",
    "smooth_basalt",
    "sandstone",
    "red_sandstone",
    "netherrack",
    "soul_sand",
    "soul_soil",
    "nether_bricks",
    "basalt",
    "blackstone",
    "end_stone"
};

static std::vector<std::string> sculk_replaceable_world_gen = {
    "dirt",
    "coarse_dirt",
    "rooted_dirt",
    "podzol",
    "grass_block",
    "mycelium",
    "sand",
    "red_sand",
    "gravel",
    "clay",
    "stone",
    "granite",
    "diorite",
    "andesite",
    "deepslate",
    "tuff",
    "calcite",
    "smooth_basalt"
};

static std::vector<std::string> shulker_boxes = {
    "shulker_box",
    "white_shulker_box",
    "orange_shulker_box",
    "magenta_shulker_box",
    "light_blue_shulker_box",
    "yellow_shulker_box",
    "lime_shulker_box",
    "pink_shulker_box",
    "gray_shulker_box",
    "light_gray_shulker_box",
    "cyan_shulker_box",
    "purple_shulker_box",
    "blue_shulker_box",
    "brown_shulker_box",
    "green_shulker_box",
    "red_shulker_box",
    "black_shulker_box"
};

static std::vector<std::string> signs = {
    "oak_sign",
    "spruce_sign",
    "birch_sign",
    "jungle_sign",
    "acacia_sign",
    "cherry_sign",
    "dark_oak_sign",
    "pale_oak_sign",
    "mangrove_sign",
    "bamboo_sign",
    "crimson_sign",
    "warped_sign"
};

static std::vector<std::string> slabs = {
    "stone_slab",
    "smooth_stone_slab",
    "stone_brick_slab",
    "mossy_stone_brick_slab",
    "andesite_slab",
    "polished_andesite_slab",
    "diorite_slab",
    "polished_diorite_slab",
    "granite_slab",
    "polished_granite_slab",
    "cobblestone_slab",
    "mossy_cobblestone_slab",
    "sandstone_slab",
    "smooth_sandstone_slab",
    "cut_sandstone_slab",
    "red_sandstone_slab",
    "smooth_red_sandstone_slab",
    "cut_red_sandstone_slab",
    "brick_slab",
    "prismarine_slab",
    "prismarine_brick_slab",
    "dark_prismarine_slab",
    "nether_brick_slab",
    "red_nether_brick_slab",
    "blackstone_slab",
    "polished_blackstone_slab",
    "polished_blackstone_brick_slab",
    "end_stone_brick_slab",
    "purpur_slab",
    "quartz_slab",
    "smooth_quartz_slab",
    "oak_slab",
    "spruce_slab",
    "birch_slab",
    "jungle_slab",
    "acacia_slab",
    "cherry_slab",
    "dark_oak_slab",
    "pale_oak_slab",
    "mangrove_slab",
    "bamboo_slab",
    "bamboo_mosaic_slab",
    "crimson_slab",
    "warped_slab",
    "deepslate_brick_slab",
    "deepslate_tile_slab",
    "polished_deepslate_slab",
    "cobbled_deepslate_slab",
    "cut_copper_slab",
    "exposed_cut_copper_slab",
    "weathered_cut_copper_slab",
    "oxidized_cut_copper_slab",
    "waxed_cut_copper_slab",
    "waxed_exposed_cut_copper_slab",
    "waxed_weathered_cut_copper_slab",
    "waxed_oxidized_cut_copper_slab",
    "tuff_slab",
    "polished_tuff_slab",
    "tuff_brick_slab"
};

static std::vector<std::string> small_dripleaf_placeable = {
    "clay",
    "coarse_dirt",
    "dirt",
    "farmland",
    "grass_block",
    "moss_block",
    "mud",
    "muddy_mangrove_roots",
    "mycelium",
    "podzol",
    "rooted_dirt"
};

static std::vector<std::string> small_flowers = {
    "dandelion",
    "poppy",
    "blue_orchid",
    "allium",
    "azure_bluet",
    "red_tulip",
    "orange_tulip",
    "white_tulip",
    "pink_tulip",
    "oxeye_daisy",
    "cornflower",
    "lily_of_the_valley",
    "torchflower",
    "wither_rose"
};

static std::vector<std::string> snaps_goat_horn = {
    "oak_log",
    "oak_wood",
    "stripped_oak_log",
    "stripped_oak_wood",
    "spruce_log",
    "spruce_wood",
    "stripped_spruce_log",
    "stripped_spruce_wood",
    "birch_log",
    "birch_wood",
    "stripped_birch_log",
    "stripped_birch_wood",
    "jungle_log",
    "jungle_wood",
    "stripped_jungle_log",
    "stripped_jungle_wood",
    "acacia_log",
    "acacia_wood",
    "stripped_acacia_log",
    "stripped_acacia_wood",
    "cherry_log",
    "cherry_wood",
    "stripped_cherry_log",
    "stripped_cherry_wood",
    "dark_oak_log",
    "dark_oak_wood",
    "stripped_dark_oak_log",
    "stripped_dark_oak_wood",
    "pale_oak_log",
    "pale_oak_wood",
    "stripped_pale_oak_log",
    "stripped_pale_oak_wood",
    "mangrove_log",
    "mangrove_wood",
    "stripped_mangrove_log",
    "stripped_mangrove_wood",
    "crimson_stem",
    "crimson_hyphae",
    "stripped_crimson_stem",
    "stripped_crimson_hyphae",
    "warped_stem",
    "warped_hyphae",
    "stripped_warped_stem",
    "stripped_warped_hyphae",
    "bamboo_block",
    "stripped_bamboo_block"
};

static std::vector<std::string> snow = {
    "snow",
    "snow_block",
    "powder_snow"
};

static std::vector<std::string> snow_layer_can_survive_on = {
    "ice",
    "packed_ice",
    "barrier",
    "honey_block",
    "soul_sand",
    "top_snow",
    "snow_block"
};

static std::vector<std::string> snow_layer_cannot_survive_on = {
    "ice",
    "packed_ice",
    "barrier",
    "honey_block",
    "soul_sand"
};

static std::vector<std::string> soul_fire_base_blocks = {
    "soul_sand",
    "soul_soil"
};

static std::vector<std::string> soul_speed_blocks = {
    "soul_sand",
    "soul_soil"
};

static std::vector<std::string> spruce_logs = {
    "spruce_log",
    "spruce_wood",
    "stripped_spruce_log",
    "stripped_spruce_wood"
};

static std::vector<std::string> stairs = {
    "oak_stairs",
    "cobblestone_stairs",
    "stone_stairs",
    "stone_brick_stairs",
    "mossy_stone_brick_stairs",
    "andesite_stairs",
    "polished_andesite_stairs",
    "diorite_stairs",
    "polished_diorite_stairs",
    "granite_stairs",
    "polished_granite_stairs",
    "mossy_cobblestone_stairs",
    "sandstone_stairs",
    "smooth_sandstone_stairs",
    "red_sandstone_stairs",
    "smooth_red_sandstone_stairs",
    "brick_stairs",
    "prismarine_stairs",
    "prismarine_brick_stairs",
    "dark_prismarine_stairs",
    "nether_brick_stairs",
    "red_nether_brick_stairs",
    "blackstone_stairs",
    "polished_blackstone_stairs",
    "polished_blackstone_brick_stairs",
    "end_stone_brick_stairs",
    "purpur_stairs",
    "quartz_stairs",
    "smooth_quartz_stairs",
    "spruce_stairs",
    "birch_stairs",
    "jungle_stairs",
    "acacia_stairs",
    "cherry_stairs",
    "dark_oak_stairs",
    "pale_oak_stairs",
    "mangrove_stairs",
    "bamboo_stairs",
    "bamboo_mosaic_stairs",
    "crimson_stairs",
    "warped_stairs",
    "deepslate_brick_stairs",
    "deepslate_tile_stairs",
    "polished_deepslate_stairs",
    "cobbled_deepslate_stairs",
    "cut_copper_stairs",
    "exposed_cut_copper_stairs",
    "weathered_cut_copper_stairs",
    "oxidized_cut_copper_stairs",
    "waxed_cut_copper_stairs",
    "waxed_exposed_cut_copper_stairs",
    "waxed_weathered_cut_copper_stairs",
    "waxed_oxidized_cut_copper_stairs",
    "tuff_stairs",
    "polished_tuff_stairs",
    "tuff_brick_stairs"
};

static std::vector<std::string> standing_signs = {
    "oak_sign",
    "spruce_sign",
    "birch_sign",
    "jungle_sign",
    "acacia_sign",
    "cherry_sign",
    "dark_oak_sign",
    "pale_oak_sign",
    "mangrove_sign",
    "bamboo_sign",
    "crimson_sign",
    "warped_sign"
};

static std::vector<std::string> stone_bricks = {
    "stone_bricks",
    "mossy_stone_bricks",
    "cracked_stone_bricks",
    "chiseled_stone_bricks"
};

static std::vector<std::string> stone_ore_replaceables = {
    "stone",
    "granite",
    "diorite",
    "andesite"
};

static std::vector<std::string> stone_pressure_plates = {
    "stone_pressure_plate",
    "polished_blackstone_pressure_plate"
};

static std::vector<std::string> strider_warm_blocks = {
    "lava"
};

static std::vector<std::string> sword_efficient = {
    "cobweb",
    "redstone_wire",
    "tripwire",
    "bamboo",
    "bamboo_sapling"
};

static std::vector<std::string> tall_flowers = {
    "sunflower",
    "lilac",
    "rose_bush",
    "peony",
    "pitcher_plant"
};

static std::vector<std::string> terracotta = {
    "terracotta",
    "white_terracotta",
    "orange_terracotta",
    "magenta_terracotta",
    "light_blue_terracotta",
    "yellow_terracotta",
    "lime_terracotta",
    "pink_terracotta",
    "gray_terracotta",
    "light_gray_terracotta",
    "cyan_terracotta",
    "purple_terracotta",
    "blue_terracotta",
    "brown_terracotta",
    "green_terracotta",
    "red_terracotta",
    "black_terracotta"
};

static std::vector<std::string> trapdoors = {
    "oak_trapdoor",
    "spruce_trapdoor",
    "birch_trapdoor",
    "jungle_trapdoor",
    "acacia_trapdoor",
    "cherry_trapdoor",
    "dark_oak_trapdoor",
    "pale_oak_trapdoor",
    "mangrove_trapdoor",
    "bamboo_trapdoor",
    "crimson_trapdoor",
    "warped_trapdoor",
    "iron_trapdoor",
    "copper_trapdoor",
    "exposed_copper_trapdoor",
    "weathered_copper_trapdoor",
    "oxidized_copper_trapdoor",
    "waxed_copper_trapdoor",
    "waxed_exposed_copper_trapdoor",
    "waxed_weathered_copper_trapdoor",
    "waxed_oxidized_copper_trapdoor"
};

static std::vector<std::string> triggers_ambient_desert_dry_vegetation_block_sounds = {
    "dead_bush"
};

static std::vector<std::string> triggers_ambient_desert_sand_block_sounds = {
    "sand",
    "sandstone",
    "smooth_sandstone",
    "cut_sandstone",
    "chiseled_sandstone",
    "sandstone_stairs",
    "smooth_sandstone_stairs",
    "sandstone_slab",
    "smooth_sandstone_slab",
    "cut_sandstone_slab",
    "sandstone_wall"
};

static std::vector<std::string> triggers_ambient_dried_ghast_block_sounds = {
    "bone_block"
};

static std::vector<std::string> underwater_bonemeals = {
    "seagrass",
    "tall_seagrass",
    "sea_pickle",
    "kelp",
    "kelp_plant"
};

static std::vector<std::string> unstable_bottom_center = {
    "fence_gate"
};

static std::vector<std::string> valid_spawn = {
    "grass_block",
    "podzol"
};

static std::vector<std::string> vibration_resonators = {
    "amethyst_block",
    "budding_amethyst"
};

static std::vector<std::string> walls = {
    "cobblestone_wall",
    "mossy_cobblestone_wall",
    "stone_brick_wall",
    "mossy_stone_brick_wall",
    "andesite_wall",
    "diorite_wall",
    "granite_wall",
    "sandstone_wall",
    "red_sandstone_wall",
    "brick_wall",
    "prismarine_wall",
    "nether_brick_wall",
    "red_nether_brick_wall",
    "blackstone_wall",
    "polished_blackstone_wall",
    "polished_blackstone_brick_wall",
    "end_stone_brick_wall",
    "deepslate_brick_wall",
    "deepslate_tile_wall",
    "polished_deepslate_wall",
    "cobbled_deepslate_wall",
    "mud_brick_wall",
    "tuff_wall",
    "polished_tuff_wall",
    "tuff_brick_wall"
};

static std::vector<std::string> wall_corals = {
    "tube_coral_wall_fan",
    "brain_coral_wall_fan",
    "bubble_coral_wall_fan",
    "fire_coral_wall_fan",
    "horn_coral_wall_fan",
    "dead_tube_coral_wall_fan",
    "dead_brain_coral_wall_fan",
    "dead_bubble_coral_wall_fan",
    "dead_fire_coral_wall_fan",
    "dead_horn_coral_wall_fan"
};

static std::vector<std::string> wall_hanging_signs = {
    "oak_wall_hanging_sign",
    "spruce_wall_hanging_sign",
    "birch_wall_hanging_sign",
    "acacia_wall_hanging_sign",
    "cherry_wall_hanging_sign",
    "jungle_wall_hanging_sign",
    "dark_oak_wall_hanging_sign",
    "pale_oak_wall_hanging_sign",
    "crimson_wall_hanging_sign",
    "warped_wall_hanging_sign",
    "mangrove_wall_hanging_sign",
    "bamboo_wall_hanging_sign"
};

static std::vector<std::string> wall_post_override = {
    "torch",
    "soul_torch",
    "redstone_torch",
    "tripwire_hook",
    "banner",
    "white_banner",
    "orange_banner",
    "magenta_banner",
    "light_blue_banner",
    "yellow_banner",
    "lime_banner",
    "pink_banner",
    "gray_banner",
    "light_gray_banner",
    "cyan_banner",
    "purple_banner",
    "blue_banner",
    "brown_banner",
    "green_banner",
    "red_banner",
    "black_banner"
};

static std::vector<std::string> wall_signs = {
    "oak_wall_sign",
    "spruce_wall_sign",
    "birch_wall_sign",
    "jungle_wall_sign",
    "acacia_wall_sign",
    "cherry_wall_sign",
    "dark_oak_wall_sign",
    "pale_oak_wall_sign",
    "mangrove_wall_sign",
    "bamboo_wall_sign",
    "crimson_wall_sign",
    "warped_wall_sign"
};

static std::vector<std::string> warped_stems = {
    "warped_stem",
    "warped_hyphae",
    "stripped_warped_stem",
    "stripped_warped_hyphae"
};

static std::vector<std::string> wart_blocks = {
    "nether_wart_block",
    "warped_wart_block"
};

static std::vector<std::string> wither_immune = {
    "bedrock",
    "end_portal",
    "end_portal_frame",
    "end_gateway",
    "command_block",
    "repeating_command_block",
    "chain_command_block",
    "barrier",
    "structure_block",
    "jigsaw",
    "moving_piston",
    "obsidian",
    "crying_obsidian",
    "respawn_anchor",
    "reinforced_deepslate"
};

static std::vector<std::string> wither_summon_base_blocks = {
    "soul_sand",
    "soul_soil"
};

static std::vector<std::string> wolves_spawnable_on = {
    "grass_block",
    "snow",
    "snow_block"
};

static std::vector<std::string> wooden_buttons = {
    "oak_button",
    "spruce_button",
    "birch_button",
    "jungle_button",
    "acacia_button",
    "cherry_button",
    "dark_oak_button",
    "pale_oak_button",
    "mangrove_button",
    "bamboo_button",
    "crimson_button",
    "warped_button"
};

static std::vector<std::string> wooden_doors = {
    "oak_door",
    "spruce_door",
    "birch_door",
    "jungle_door",
    "acacia_door",
    "cherry_door",
    "dark_oak_door",
    "pale_oak_door",
    "mangrove_door",
    "bamboo_door",
    "crimson_door",
    "warped_door"
};

static std::vector<std::string> wooden_fences = {
    "oak_fence",
    "spruce_fence",
    "birch_fence",
    "jungle_fence",
    "acacia_fence",
    "cherry_fence",
    "dark_oak_fence",
    "pale_oak_fence",
    "mangrove_fence",
    "bamboo_fence",
    "crimson_fence",
    "warped_fence"
};

static std::vector<std::string> wooden_pressure_plates = {
    "oak_pressure_plate",
    "spruce_pressure_plate",
    "birch_pressure_plate",
    "jungle_pressure_plate",
    "acacia_pressure_plate",
    "cherry_pressure_plate",
    "dark_oak_pressure_plate",
    "pale_oak_pressure_plate",
    "mangrove_pressure_plate",
    "bamboo_pressure_plate",
    "crimson_pressure_plate",
    "warped_pressure_plate"
};

static std::vector<std::string> wooden_slabs = {
    "oak_slab",
    "spruce_slab",
    "birch_slab",
    "jungle_slab",
    "acacia_slab",
    "cherry_slab",
    "dark_oak_slab",
    "pale_oak_slab",
    "mangrove_slab",
    "bamboo_slab",
    "bamboo_mosaic_slab",
    "crimson_slab",
    "warped_slab"
};

static std::vector<std::string> wooden_stairs = {
    "oak_stairs",
    "spruce_stairs",
    "birch_stairs",
    "jungle_stairs",
    "acacia_stairs",
    "cherry_stairs",
    "dark_oak_stairs",
    "pale_oak_stairs",
    "mangrove_stairs",
    "bamboo_stairs",
    "bamboo_mosaic_stairs",
    "crimson_stairs",
    "warped_stairs"
};

static std::vector<std::string> wooden_trapdoors = {
    "oak_trapdoor",
    "spruce_trapdoor",
    "birch_trapdoor",
    "jungle_trapdoor",
    "acacia_trapdoor",
    "cherry_trapdoor",
    "dark_oak_trapdoor",
    "pale_oak_trapdoor",
    "mangrove_trapdoor",
    "bamboo_trapdoor",
    "crimson_trapdoor",
    "warped_trapdoor"
};

static std::vector<std::string> wool = {
    "white_wool",
    "orange_wool",
    "magenta_wool",
    "light_blue_wool",
    "yellow_wool",
    "lime_wool",
    "pink_wool",
    "gray_wool",
    "light_gray_wool",
    "cyan_wool",
    "purple_wool",
    "blue_wool",
    "brown_wool",
    "green_wool",
    "red_wool",
    "black_wool"
};

static std::vector<std::string> wool_carpets = {
    "white_carpet",
    "orange_carpet",
    "magenta_carpet",
    "light_blue_carpet",
    "yellow_carpet",
    "lime_carpet",
    "pink_carpet",
    "gray_carpet",
    "light_gray_carpet",
    "cyan_carpet",
    "purple_carpet",
    "blue_carpet",
    "brown_carpet",
    "green_carpet",
    "red_carpet",
    "black_carpet"
};

static std::unordered_map<std::string, std::vector<std::string>> tagMap = {
    { "acacia_logs", acacia_logs },
    { "air", air },
    { "all_hanging_signs", all_hanging_signs },
    { "all_signs", all_signs },
    { "ancient_city_replaceable", ancient_city_replaceable },
    { "anvil", anvil },
    { "axolotl_always_hostiles", axolotl_always_hostiles },
    { "azalea_grows_on", azalea_grows_on },
    { "azalea_root_replaceable", azalea_root_replaceable },
    { "bamboo_blocks", bamboo_blocks },
    { "bamboo_plantable_on", bamboo_plantable_on },
    { "banners", banners },
    { "base_stone_nether", base_stone_nether },
    { "base_stone_overworld", base_stone_overworld },
    { "beacon_base_blocks", beacon_base_blocks },
    { "beds", beds },
    { "bee_growables", bee_growables },
    { "beehives", beehives },
    { "big_dripleaf_placeable", big_dripleaf_placeable },
    { "birch_logs", birch_logs },
    { "buttons", buttons },
    { "campfires", campfires },
    { "candle_cakes", candle_cakes },
    { "candles", candles },
    { "cauldrons", cauldrons },
    { "cave_vines", cave_vines },
    { "cherry_logs", cherry_logs },
    { "climbable", climbable },
    { "coal_ores", coal_ores },
    { "completes_find_tree_tutorial", completes_find_tree_tutorial },
    { "concrete_powder", concrete_powder },
    { "copper_ores", copper_ores },
    { "coral_blocks", coral_blocks },
    { "coral_plants", coral_plants },
    { "corals", corals },
    { "crimson_stems", crimson_stems },
    { "crops", crops },
    { "crystal_sound_blocks", crystal_sound_blocks },
    { "dark_oak_logs", dark_oak_logs },
    { "dead_bush_may_place_on", dead_bush_may_place_on },
    { "deepslate_ore_replaceables", deepslate_ore_replaceables },
    { "diamond_ores", diamond_ores },
    { "dirt", dirt },
    { "doors", doors },
    { "dragon_immune", dragon_immune },
    { "dragon_transparent", dragon_transparent },
    { "dripstone_replaceable_blocks", dripstone_replaceable_blocks },
    { "emerald_ores", emerald_ores },
    { "enchantment_power_provider", enchantment_power_provider },
    { "enchantment_power_transmitter", enchantment_power_transmitter },
    { "enderman_holdable", enderman_holdable },
    { "fall_damage_resetting", fall_damage_resetting },
    { "features_cannot_replace", features_cannot_replace },
    { "fence_gates", fence_gates },
    { "fences", fences },
    { "fire", fire },
    { "flowers", flowers },
    { "fox_food", fox_food },
    { "frog_prefer_jump_to", frog_prefer_jump_to },
    { "geode_invalid_blocks", geode_invalid_blocks },
    { "gold_ores", gold_ores },
    { "guarded_by_piglins", guarded_by_piglins },
    { "hoglin_repellents", hoglin_repellents },
    { "ice", ice },
    { "ignored_by_piglin_babies", ignored_by_piglin_babies },
    { "impermeable", impermeable },
    { "infiniburn_end", infiniburn_end },
    { "infiniburn_nether", infiniburn_nether },
    { "infiniburn_overworld", infiniburn_overworld },
    { "inside_step_sound_blocks", inside_step_sound_blocks },
    { "iron_ores", iron_ores },
    { "jungle_logs", jungle_logs },
    { "lapis_ores", lapis_ores },
    { "lava_pool_stone_cannot_replace", lava_pool_stone_cannot_replace },
    { "leaves", leaves },
    { "lectern_books", lectern_books },
    { "logs", logs },
    { "logs_that_burn", logs_that_burn },
    { "lush_ground_replaceable", lush_ground_replaceable },
    { "maintain_farmland", maintain_farmland },
    { "mangrove_logs", mangrove_logs },
    { "mangrove_logs_can_grow_through", mangrove_logs_can_grow_through },
    { "mangrove_roots_can_grow_through", mangrove_roots_can_grow_through },
    { "moss_replaceable", moss_replaceable },
    { "mushroom_grow_block", mushroom_grow_block },
    { "needs_diamond_tool", needs_diamond_tool },
    { "needs_iron_tool", needs_iron_tool },
    { "needs_stone_tool", needs_stone_tool },
    { "nether_carver_replaceables", nether_carver_replaceables },
    { "non_flammable_wood", non_flammable_wood },
    { "nylium", nylium },
    { "oak_logs", oak_logs },
    { "occludes_vibration_signals", occludes_vibration_signals },
    { "overworld_carver_replaceables", overworld_carver_replaceables },
    { "overworld_natural_logs", overworld_natural_logs },
    { "pale_oak_logs", pale_oak_logs },
    { "parrots_spawnable_on", parrots_spawnable_on },
    { "piglin_loved", piglin_loved },
    { "piglin_repellents", piglin_repellents },
    { "planks", planks },
    { "polar_bears_spawnable_on_alternate", polar_bears_spawnable_on_alternate },
    { "portals", portals },
    { "pressure_plates", pressure_plates },
    { "prevent_mob_spawning_inside", prevent_mob_spawning_inside },
    { "rabbits_spawnable_on", rabbits_spawnable_on },
    { "redstone_ores", redstone_ores },
    { "replaceable", replaceable },
    { "replaceable_by_trees", replaceable_by_trees },
    { "sand", sand },
    { "saplings", saplings },
    { "sculk_replaceable", sculk_replaceable },
    { "sculk_replaceable_world_gen", sculk_replaceable_world_gen },
    { "shulker_boxes", shulker_boxes },
    { "signs", signs },
    { "slabs", slabs },
    { "small_dripleaf_placeable", small_dripleaf_placeable },
    { "small_flowers", small_flowers },
    { "snaps_goat_horn", snaps_goat_horn },
    { "snow", snow },
    { "snow_layer_can_survive_on", snow_layer_can_survive_on },
    { "snow_layer_cannot_survive_on", snow_layer_cannot_survive_on },
    { "soul_fire_base_blocks", soul_fire_base_blocks },
    { "soul_speed_blocks", soul_speed_blocks },
    { "spruce_logs", spruce_logs },
    { "stairs", stairs },
    { "standing_signs", standing_signs },
    { "stone_bricks", stone_bricks },
    { "stone_ore_replaceables", stone_ore_replaceables },
    { "stone_pressure_plates", stone_pressure_plates },
    { "strider_warm_blocks", strider_warm_blocks },
    { "sword_efficient", sword_efficient },
    { "tall_flowers", tall_flowers },
    { "terracotta", terracotta },
    { "trapdoors", trapdoors },
    { "triggers_ambient_desert_dry_vegetation_block_sounds", triggers_ambient_desert_dry_vegetation_block_sounds },
    { "triggers_ambient_desert_sand_block_sounds", triggers_ambient_desert_sand_block_sounds },
    { "triggers_ambient_dried_ghast_block_sounds", triggers_ambient_dried_ghast_block_sounds },
    { "underwater_bonemeals", underwater_bonemeals },
    { "unstable_bottom_center", unstable_bottom_center },
    { "valid_spawn", valid_spawn },
    { "vibration_resonators", vibration_resonators },
    { "walls", walls },
    { "wall_corals", wall_corals },
    { "wall_hanging_signs", wall_hanging_signs },
    { "wall_post_override", wall_post_override },
    { "wall_signs", wall_signs },
    { "warped_stems", warped_stems },
    { "wart_blocks", wart_blocks },
    { "wither_immune", wither_immune },
    { "wither_summon_base_blocks", wither_summon_base_blocks },
    { "wolves_spawnable_on", wolves_spawnable_on },
    { "wooden_buttons", wooden_buttons },
    { "wooden_doors", wooden_doors },
    { "wooden_fences", wooden_fences },
    { "wooden_pressure_plates", wooden_pressure_plates },
    { "wooden_slabs", wooden_slabs },
    { "wooden_stairs", wooden_stairs },
    { "wooden_trapdoors", wooden_trapdoors },
    { "wool", wool },
    { "wool_carpets", wool_carpets },
    { "mineable/axe", mineable_axe },
    { "mineable/hoe", mineable_hoe },
    { "mineable/pickaxe", mineable_pickaxe },
    { "mineable/shovel", mineable_shovel },
};

const std::unordered_map<std::string, std::vector<std::string>>& getTagMap() {
    return tagMap;
}