#include "ItemCounter.hpp"

#include <algorithm>
#include <cctype>

#include "Client.hpp"
#include "Events/Game/TickEvent.hpp"
#include "GUI/Engine/Constraints.hpp"
#include "Modules/ClickGUI/ClickGUI.hpp"
#include "Manager.hpp"
#include "SDK/Client/Render/BaseActorRenderContext.hpp"
#include <SDK/Client/Item/ItemRegistryHelper.hpp>
#include "Utils/Render/PositionUtils.hpp"

namespace {
    struct ItemQuery {
        std::string normalizedInput = {};
        std::string namespacePart = {};
        std::string itemPart = {};
        bool hasNamespace = false;
        bool formatValid = false;
        bool placeholder = true;
    };

    std::string trimCopy(const std::string& input) {
        if (input.empty()) return {};

        size_t start = 0;
        while (start < input.size() && std::isspace(static_cast<unsigned char>(input[start]))) {
            ++start;
        }

        size_t end = input.size();
        while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))) {
            --end;
        }

        return input.substr(start, end - start);
    }

    ItemQuery parseItemQuery(const std::string& rawName) {
        ItemQuery query;
        query.normalizedInput = String::toLower(trimCopy(rawName));
        std::ranges::replace(query.normalizedInput, ' ', '_');

        query.placeholder = query.normalizedInput.empty() ||
            query.normalizedInput == "item name here" ||
            query.normalizedInput == "item_name_here";

        if (query.placeholder) return query;

        const auto isAllowed = [](char c) {
            return (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') ||
                c == '_' || c == ':' || c == '/' || c == '.' || c == '-';
        };

        if (!std::ranges::all_of(query.normalizedInput, isAllowed)) {
            return query;
        }

        const size_t colonPos = query.normalizedInput.find(':');
        if (colonPos != std::string::npos) {
            if (query.normalizedInput.find(':', colonPos + 1) != std::string::npos) {
                return query;
            }

            query.hasNamespace = true;
            query.namespacePart = query.normalizedInput.substr(0, colonPos);
            query.itemPart = query.normalizedInput.substr(colonPos + 1);

            if (query.namespacePart.empty() || query.itemPart.empty()) {
                return query;
            }
        } else {
            query.itemPart = query.normalizedInput;
        }

        query.formatValid = !query.itemPart.empty();
        return query;
    }

    std::string getNormalizedStackFullId(ItemStack* stack) {
        if (!stack || !stack->getItem()) return {};

        Item* item = stack->getItem();

        std::string itemName = String::toLower(item->name);
        std::ranges::replace(itemName, ' ', '_');
        if (itemName.empty()) return {};

        if (itemName.find(':') != std::string::npos) {
            return itemName;
        }

        std::string itemNamespace = String::toLower(item->Namespace);
        std::ranges::replace(itemNamespace, ' ', '_');
        if (itemNamespace.empty()) itemNamespace = "minecraft";

        return itemNamespace + ":" + itemName;
    }

    std::string getNormalizedStackItemId(ItemStack* stack) {
        std::string fullId = getNormalizedStackFullId(stack);
        if (fullId.empty()) return {};

        const size_t colonPos = fullId.find(':');
        if (colonPos == std::string::npos) return fullId;

        return fullId.substr(colonPos + 1);
    }

    bool doesStackMatchQuery(ItemStack* stack, const ItemQuery& query, std::string* resolvedIdOut = nullptr) {
        if (!query.formatValid || !stack || !stack->getItem()) return false;

        const std::string fullId = getNormalizedStackFullId(stack);
        if (fullId.empty()) return false;

        bool matches = false;
        if (query.hasNamespace) {
            matches = (fullId == query.normalizedInput);
        } else {
            matches = (getNormalizedStackItemId(stack) == query.itemPart || fullId == query.itemPart);
        }

        if (matches && resolvedIdOut != nullptr) {
            *resolvedIdOut = fullId;
        }

        return matches;
    }

    template <typename PlayerT>
    ItemStack* findFirstMatchingStack(PlayerT* player, const ItemQuery& query, std::string* resolvedIdOut = nullptr) {
        if (!player || !player->getSupplies()) return nullptr;

        auto* supplies = player->getSupplies();
        auto* inventory = supplies->getInventory();
        if (!inventory) return nullptr;

        ItemStack* offhandItem = player->getOffhandSlot();
        if (doesStackMatchQuery(offhandItem, query, resolvedIdOut)) {
            return offhandItem;
        }

        for (int slot = 0; slot < 36; slot++) {
            ItemStack* item = inventory->getItem(slot);
            if (doesStackMatchQuery(item, query, resolvedIdOut)) {
                return item;
            }
        }

        return nullptr;
    }
}


void ItemCounter::onEnable() {
    Listen(this, TickEvent, &ItemCounter::onTick)
    Listen(this, RenderEvent, &ItemCounter::onRender)
    Listen(this, SetupAndRenderEvent, &ItemCounter::onSetupAndRender)
    Module::onEnable();
}

void ItemCounter::onDisable() {
    Deafen(this, TickEvent, &ItemCounter::onTick)
    Deafen(this, RenderEvent, &ItemCounter::onRender)
    Deafen(this, SetupAndRenderEvent, &ItemCounter::onSetupAndRender)
    registryValidationCache.clear();
    registryAvailable = false;
    tickDataReady = false;
    Module::onDisable();
}

void ItemCounter::onSetup() {
    items = 0;
    subIndexesToRender.clear();
    itemCountBySubIndex.clear();
    itemSeenInInventory.clear();
    resolvedItemIdBySubIndex.clear();
    iconRenderQueue.clear();
    registryValidationCache.clear();
    registryAvailable = false;
    tickDataReady = false;

    for (int i = 1; i < 2000; i++) {
        std::string subIndex = std::format("-{}", i);
        if (this->settings.getSettingByName<std::string>("itemName" + subIndex) != nullptr &&
            this->settings.getSettingByName<std::string>("text" + subIndex) != nullptr) {
            setDef("displayOrder" + subIndex, i);
            setDef("useItemTexture" + subIndex, false);
            setDef("replaceNameWithTexture" + subIndex, true);
            setDef("textureCountPlacement" + subIndex, (std::string) "Next To Item");
            setDef("textureCountMinecraftFont" + subIndex, false);
            setDef("hideWhenZero" + subIndex, false);
            setDef("hideCountIfOne" + subIndex, false);
            items += 1;
            subIndexesToRender.push_back(i);
        }
    }

    normalizeDisplayOrder();
}

int ItemCounter::getNextAvailableIndex() {
    int index = 1;
    while (this->settings.getSettingByName<std::string>("itemName-" + FlarialGUI::cached_to_string(index)) != nullptr) {
        index += 1;
        // Safety check to prevent infinite loop
        if (index > 2000) {
            break;
        }
    }
    return index;
}

int ItemCounter::getNextAvailableOrder() {
    int maxOrder = 0;
    for (int i : subIndexesToRender) {
        const auto* orderSetting = settings.getSettingByName<int>(std::format("displayOrder-{}", i));
        if (orderSetting != nullptr) {
            maxOrder = std::max(maxOrder, orderSetting->value);
        }
    }
    return maxOrder + 1;
}

std::vector<int> ItemCounter::getOrderedSubIndexes() {
    std::vector<int> ordered = subIndexesToRender;

    std::ranges::sort(ordered, [this](const int a, const int b) {
        const auto* orderASetting = settings.getSettingByName<int>(std::format("displayOrder-{}", a));
        const auto* orderBSetting = settings.getSettingByName<int>(std::format("displayOrder-{}", b));

        const int orderA = orderASetting != nullptr ? orderASetting->value : a;
        const int orderB = orderBSetting != nullptr ? orderBSetting->value : b;

        if (orderA == orderB) return a < b;
        return orderA < orderB;
    });

    return ordered;
}

void ItemCounter::normalizeDisplayOrder() {
    std::vector<int> ordered = getOrderedSubIndexes();
    int normalizedOrder = 1;

    for (int subIndexInt : ordered) {
        settings.setValue(std::format("displayOrder-{}", subIndexInt), normalizedOrder++);
    }

    subIndexesToRender = std::move(ordered);
}

void ItemCounter::multiDefaultConfig(int subIndexInt) {
    const std::string subIndex = std::format("-{}", subIndexInt);

    setDef("itemName" + subIndex, std::string("item name here"));
    setDef("displayOrder" + subIndex, subIndexInt);
    setDef("useCustomSettings" + subIndex, false);
    setDef("showCustomSettings" + subIndex, false);
    setDef("useItemTexture" + subIndex, true);
    setDef("replaceNameWithTexture" + subIndex, true);
    setDef("textureCountPlacement" + subIndex, (std::string) "Next To Item");
    setDef("textureCountMinecraftFont" + subIndex, false);
    setDef("hideWhenZero" + subIndex, false);
    setDef("hideCountIfOne" + subIndex, false);

    setDef("percentageX" + subIndex, 0.0f);
    setDef("percentageY" + subIndex, 0.0f);

    setDef("uiscale" + subIndex, 0.5f);
    setDef("BlurEffect" + subIndex, false);
    setDef("rounding" + subIndex, 32.0f);
    setDef("showBg" + subIndex, true);
    setDef("rectShadow" + subIndex, false);
    setDef("rectShadowOffset" + subIndex, 0.003f);
    setDef("border" + subIndex, false);
    setDef("borderWidth" + subIndex, 1.0f);
    setDef("glow" + subIndex, false);
    setDef("glowAmount" + subIndex, 30.0f);

    setDef("text" + subIndex, (std::string) std::string("{name}: {value}"));
    setDef("textscale" + subIndex, 1.5f);
    setDef("textalignment" + subIndex, (std::string) "Center");
    setDef("textShadow" + subIndex, false);
    setDef("textShadowOffset" + subIndex, 0.003f);

    setDef("text" + subIndex, (std::string) "fafafa", 1.f, false);
    setDef("bg" + subIndex, (std::string) "000000", 0.55f, false);
    setDef("rectShadow" + subIndex, (std::string) "00000", 0.55f, false);
    setDef("textShadow" + subIndex, (std::string) "00000", 0.55f, false);
    setDef("border" + subIndex, (std::string) "000000", 1.f, false);
    setDef("glow" + subIndex, (std::string) "F0F0F0", 1.f, false);

    setDef("responsivewidth" + subIndex, true);
    setDef("reversepaddingx" + subIndex, false);
    setDef("reversepaddingy" + subIndex, false);
    setDef("padx" + subIndex, 0.0f);
    setDef("pady" + subIndex, 0.0f);
    setDef("rectwidth" + subIndex, 1.0f);
    setDef("rectheight" + subIndex, 1.0f);
    setDef("rotation" + subIndex, 0.0f);

}

void ItemCounter::multiClearConfig(int subIndexInt) {
    const std::string subIndex = std::format("-{}", subIndexInt);

    settings.deleteSetting("itemName" + subIndex);
    settings.deleteSetting("displayOrder" + subIndex);
    settings.deleteSetting("useCustomSettings" + subIndex);
    settings.deleteSetting("showCustomSettings" + subIndex);
    settings.deleteSetting("useItemTexture" + subIndex);
    settings.deleteSetting("replaceNameWithTexture" + subIndex);
    settings.deleteSetting("textureCountPlacement" + subIndex);
    settings.deleteSetting("textureCountMinecraftFont" + subIndex);
    settings.deleteSetting("hideWhenZero" + subIndex);
    settings.deleteSetting("hideCountIfOne" + subIndex);

    settings.deleteSetting("percentageX" + subIndex);
    settings.deleteSetting("percentageY" + subIndex);

    settings.deleteSetting("uiscale" + subIndex);
    settings.deleteSetting("BlurEffect" + subIndex);
    settings.deleteSetting("rounding" + subIndex);
    settings.deleteSetting("showBg" + subIndex);
    settings.deleteSetting("rectShadow" + subIndex);
    settings.deleteSetting("rectShadowOffset" + subIndex);
    settings.deleteSetting("border" + subIndex);
    settings.deleteSetting("borderWidth" + subIndex);
    settings.deleteSetting("glow" + subIndex);
    settings.deleteSetting("glowAmount" + subIndex);

    settings.deleteSetting("text" + subIndex);
    settings.deleteSetting("textscale" + subIndex);
    settings.deleteSetting("textalignment" + subIndex);
    settings.deleteSetting("textShadow" + subIndex);
    settings.deleteSetting("textShadowOffset" + subIndex);

    for (std::string colSetting: std::vector<std::string>{ "text", "bg", "rectShadow",  "textShadow", "border", "glow"}) {
        settings.deleteSetting(colSetting + subIndex + "Col");
        settings.deleteSetting(colSetting + subIndex + "RGB");
        settings.deleteSetting(colSetting + subIndex + "Opacity");
    }

    settings.deleteSetting("responsivewidth" + subIndex);
    settings.deleteSetting("reversepaddingx" + subIndex);
    settings.deleteSetting("reversepaddingy" + subIndex);
    settings.deleteSetting("padx" + subIndex);
    settings.deleteSetting("pady" + subIndex);
    settings.deleteSetting("rectwidth" + subIndex);
    settings.deleteSetting("rectheight" + subIndex);
    settings.deleteSetting("rotation" + subIndex);

}

void ItemCounter::defaultConfig() {
    Module::defaultConfig("all");
    setDef("groupTextures", false);
    setDef("groupVertical", false);
    setDef("groupSpacing", 1.0f);
    setDef("vanillaCountOffsetX", 0.0f);
    setDef("vanillaCountOffsetY", 0.0f);
    setDef("showSettings", false);
}

void ItemCounter::multiSettingsRender(int subIndexInt) {

    const std::string subIndex = std::format("-{}", subIndexInt);

    const bool c = getOps<bool>("useCustomSettings" + subIndex) && getOps<bool>("showCustomSettings" + subIndex);

    if (c) addHeader(std::format("Main {}", subIndexInt));
    addConditionalSlider(c, "UI Scale", "", settings.getSettingByName<float>("uiscale" + subIndex)->value, 2.0f);
    addConditionalToggle(c, "Translucency", "A blur effect, MAY BE PERFORMANCE HEAVY!", settings.getSettingByName<bool>("BlurEffect" + subIndex)->value);
    addConditionalSlider(c, "Rounding", "Rounding of the rectangle", settings.getSettingByName<float>("rounding" + subIndex)->value);
    addConditionalToggle(c, "Background", "", settings.getSettingByName<bool>("showBg" + subIndex)->value);
    addConditionalToggle(c && getOps<bool>("showBg" + subIndex), "Background Shadow", "Displays a shadow under the background", settings.getSettingByName<bool>("rectShadow" + subIndex)->value);
    addConditionalSlider(c && getOps<bool>("showBg" + subIndex) && getOps<bool>("rectShadow" + subIndex), "Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("rectShadowOffset" + subIndex)->value, 0.02f, 0.001f);
    addConditionalToggle(c, "Border", "", settings.getSettingByName<bool>("border" + subIndex)->value);
    addConditionalSlider(c && getOps<bool>("border" + subIndex), "Border Thickness", "", settings.getSettingByName<float>("borderWidth" + subIndex)->value, 4.f);
    addConditionalToggle(c, "Glow", "", settings.getSettingByName<bool>("glow" + subIndex)->value);
    addConditionalSlider(c && getOps<bool>("glow" + subIndex), "Glow Amount", "", settings.getSettingByName<float>("glowAmount" + subIndex)->value, 100.f);
    if (c) extraPadding();

    if (c) addHeader(std::format("Text {}", subIndexInt));
    addConditionalTextBox(c, "Format", "Use {value} for count, {name} for configured name, and {id} for resolved id.", 50, "text" + subIndex);
    addConditionalSlider(c, "Text Scale", "", settings.getSettingByName<float>("textscale" + subIndex)->value, 2.0f);
    addConditionalDropdown(c, "Text Alignment", "", std::vector<std::string>{"Left", "Center", "Right"}, "textalignment" + subIndex, false);
    addConditionalToggle(c, "Text Shadow", "Displays a shadow under the text", settings.getSettingByName<bool>("textShadow" + subIndex)->value);
    addConditionalSlider(getOps<bool>("textShadow" + subIndex), "Shadow Offset", "How far the shadow will be.", settings.getSettingByName<float>("textShadowOffset" + subIndex)->value, 0.02f, 0.001f);
    if (c) extraPadding();

    if (c) addHeader(std::format("Colors {}", subIndexInt));
    addConditionalColorPicker(
        c, "Text Color", "",
        settings.getSettingByName<std::string>("text" + subIndex + "Col")->value,
        settings.getSettingByName<float>("text" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("text" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("showBg" + subIndex), "Background Color", "",
        settings.getSettingByName<std::string>("bg" + subIndex + "Col")->value,
        settings.getSettingByName<float>("bg" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("bg" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("rectShadow" + subIndex), "Background Shadow Color", "",
        settings.getSettingByName<std::string>("rectShadow" + subIndex + "Col")->value,
        settings.getSettingByName<float>("rectShadow" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("rectShadow" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("textShadow" + subIndex), "Text Shadow Color", "",
        settings.getSettingByName<std::string>("textShadow" + subIndex + "Col")->value,
        settings.getSettingByName<float>("textShadow" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("textShadow" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("border" + subIndex), "Border Color", "",
        settings.getSettingByName<std::string>("border" + subIndex + "Col")->value,
        settings.getSettingByName<float>("border" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("border" + subIndex + "RGB")->value
    );
    addConditionalColorPicker(
        c && getOps<bool>("glow" + subIndex), "Glow Color", "",
        settings.getSettingByName<std::string>("glow" + subIndex + "Col")->value,
        settings.getSettingByName<float>("glow" + subIndex + "Opacity")->value,
        settings.getSettingByName<bool>("glow" + subIndex + "RGB")->value
    );
    if (c) extraPadding();

    if (c) addHeader(std::format("Misc {}", subIndexInt));
    addConditionalToggle(c, "Responsive Rectangle", "Rectangle resizes with text", settings.getSettingByName<bool>("responsivewidth" + subIndex)->value);
    addConditionalToggle(c, "Reverse Padding X", "For Text Position", settings.getSettingByName<bool>("reversepaddingx" + subIndex)->value);
    addConditionalToggle(c, "Reverse Padding Y", "For Text Position", settings.getSettingByName<bool>("reversepaddingy" + subIndex)->value);
    addConditionalSlider(c, "Padding X", "For Text Position", settings.getSettingByName<float>("padx" + subIndex)->value, 1.f);
    addConditionalSlider(c, "Padding Y", "For Text Position", settings.getSettingByName<float>("pady" + subIndex)->value, 1.f);
    addConditionalSlider(c, "Rectangle Width", "", settings.getSettingByName<float>("rectwidth" + subIndex)->value, 2.f, 0.001f);
    addConditionalSlider(c, "Rectangle Height", "", settings.getSettingByName<float>("rectheight" + subIndex)->value, 2.f, 0.001f);
    addConditionalSlider(c, "Rotation", "", settings.getSettingByName<float>("rotation" + subIndex)->value, 360.f, 0, false);

}

void ItemCounter::settingsRender(float settingsOffset) {
    initSettingsPage();

    addButton("Add Item", "adds an item", "Add", [this] {
        int index = getNextAvailableIndex();
        multiDefaultConfig(index);
        settings.setValue(std::format("displayOrder-{}", index), getNextAvailableOrder());
        subIndexesToRender.push_back(index);
        items += 1;
        normalizeDisplayOrder();
        Client::SaveSettings();
        FlarialGUI::Notify(std::format("Successfully created new item counter with index <{}>!", index));
    });

    extraPadding();

    addToggle("Group Texture Counters", "Group texture-mode entries into one stacked layout.", "groupTextures");
    addConditionalToggle(getOps<bool>("groupTextures"), "Vertical Group", "Switch between vertical and horizontal grouped layout.", "groupVertical");
    addConditionalSlider(getOps<bool>("groupTextures"), "Group Spacing", "Spacing multiplier between grouped entries.", "groupSpacing", 5.0f, 0.0f, false);

    addSlider("Vanilla Count Offset X", "Horizontal offset for Minecraft-font bottom-right counts (all counters).", "vanillaCountOffsetX", 1.0f, -1.0f, false);
    addSlider("Vanilla Count Offset Y", "Vertical offset for Minecraft-font bottom-right counts (all counters).", "vanillaCountOffsetY", 1.0f, -1.0f, false);

    extraPadding();

    // Snapshot tick-thread maps for the settings UI — prevents races with onTick's map swap.
    std::unordered_map<std::string, bool> snapRegistryCache;
    std::unordered_map<int, bool> snapItemSeen;
    std::unordered_map<int, std::string> snapResolvedId;
    bool snapRegistryAvailable;
    {
        std::lock_guard<std::mutex> lock(tickDataMutex);
        snapRegistryCache = registryValidationCache;
        snapItemSeen = itemSeenInInventory;
        snapResolvedId = resolvedItemIdBySubIndex;
        snapRegistryAvailable = registryAvailable;
    }

    const std::vector<int> orderedSubIndexes = getOrderedSubIndexes();
    for (auto i: orderedSubIndexes) {
        std::string subIndex = std::format("-{}", i);

        addHeader(std::format("Item - {}", i));
        setDef("textureCountPlacement" + subIndex, (std::string) "Next To Item");
        setDef("textureCountMinecraftFont" + subIndex, false);

        std::string& itemNameSetting = settings.getSettingByName<std::string>("itemName" + subIndex)->value;
        addTextBox("Item Name", "Use [namespace:]item_name (e.g. minecraft:totem_of_undying, custom:item_name)", itemNameSetting, 50);

        if (ItemQuery query = parseItemQuery(itemNameSetting); query.placeholder) {
            addElementText("Validation", "Waiting for item id input.");
        } else if (!query.formatValid) {
            addElementText("Validation", "Invalid item id format. Allowed: a-z, 0-9, _, -, ., / and optional single namespace ':'.");
        } else {
            // Read from snapshot maps (taken under lock before the loop).
            bool inRegistry = false;
            if (auto it = snapRegistryCache.find(query.normalizedInput); it != snapRegistryCache.end())
                inRegistry = it->second;
            bool inInventory = false;
            if (auto it = snapItemSeen.find(i); it != snapItemSeen.end())
                inInventory = it->second;
            std::string resolvedId = query.normalizedInput;
            if (auto it = snapResolvedId.find(i); it != snapResolvedId.end())
                resolvedId = it->second;

            if (inRegistry && inInventory) {
                addElementText("Validation", std::format("Registered item. Matched in inventory as '{}'.", resolvedId));
            } else if (inRegistry && !inInventory) {
                addElementText("Validation", "Registered item. Not currently in your inventory/offhand.");
            } else if (!inRegistry && inInventory) {
                // Item found in inventory but not in our registry lookup
                // (could be a custom item from a behavior pack, or registry was unavailable)
                addElementText("Validation", std::format("Matched in inventory as '{}'. (Not found in base registry.)", resolvedId));
            } else if (snapRegistryAvailable) {
                addElementText("Validation", "Item not found in registry or inventory. Check the name/namespace.");
            } else {
                addElementText("Validation", "Valid format. Registry unavailable; item not in inventory.");
            }
        }
        extraPadding();

        addToggle("Use Item Texture", "Render the item's texture with count text (no slot background).", settings.getSettingByName<bool>("useItemTexture" + subIndex)->value);
        addConditionalToggle(
            getOps<bool>("useItemTexture" + subIndex),
            "Texture Replaces {name}",
            "When enabled, {name} resolves to empty text so you can render texture + count.",
            settings.getSettingByName<bool>("replaceNameWithTexture" + subIndex)->value
        );
        addConditionalDropdown(
            getOps<bool>("useItemTexture" + subIndex),
            "Texture Count Position",
            "Render the count next to, left of, or at the bottom-right of the icon.",
            std::vector<std::string>{"Next To Item", "Left of Item", "Bottom Right"},
            "textureCountPlacement" + subIndex,
            false
        );
        addConditionalToggle(
            getOps<bool>("useItemTexture" + subIndex) &&
                getOps<std::string>("textureCountPlacement" + subIndex) == "Bottom Right",
            "Bottom-Right Count Uses Minecraft Font",
            "Use Minecraft's native count font for bottom-right texture counts.",
            settings.getSettingByName<bool>("textureCountMinecraftFont" + subIndex)->value
        );

        setDef("hideWhenZero" + subIndex, false);
        addToggle("Hide When Zero", "Hides the counter when the item count is 0. Shows with count 64 in edit mode.", settings.getSettingByName<bool>("hideWhenZero" + subIndex)->value);

        setDef("hideCountIfOne" + subIndex, false);
        addConditionalToggle(
            getOps<bool>("useItemTexture" + subIndex),
            "Hide Count If 1",
            "When you have exactly 1 of the item, only show the icon without any count text.",
            settings.getSettingByName<bool>("hideCountIfOne" + subIndex)->value
        );

        std::vector<int> groupedOrdered = orderedSubIndexes;
        std::erase_if(
            groupedOrdered,
            [this](int subIdx) {
                const std::string idxStr = std::format("-{}", subIdx);
                const auto* useTextureSetting = settings.getSettingByName<bool>("useItemTexture" + idxStr);
                return useTextureSetting == nullptr || !useTextureSetting->value;
            }
        );

        auto groupedIt = std::ranges::find(groupedOrdered, i);
        const bool canMoveUp = groupedIt != groupedOrdered.end() && groupedIt != groupedOrdered.begin();
        const bool canMoveDown = groupedIt != groupedOrdered.end() && (groupedIt + 1) != groupedOrdered.end();

        int moveUpTarget = -1;
        int moveDownTarget = -1;
        if (canMoveUp) moveUpTarget = *(groupedIt - 1);
        if (canMoveDown) moveDownTarget = *(groupedIt + 1);

        addConditionalButton(
            getOps<bool>("groupTextures") && getOps<bool>("useItemTexture" + subIndex) && canMoveUp,
            "Grouped Order",
            std::format("Position {}/{} in grouped layout.", groupedIt == groupedOrdered.end() ? 0 : static_cast<int>(std::distance(groupedOrdered.begin(), groupedIt)) + 1, static_cast<int>(groupedOrdered.size())),
            "Move Up",
            [this, i, moveUpTarget] {
                if (moveUpTarget < 0) return;

                const std::string curKey = std::format("displayOrder-{}", i);
                const std::string targetKey = std::format("displayOrder-{}", moveUpTarget);
                auto* curOrder = settings.getSettingByName<int>(curKey);
                auto* targetOrder = settings.getSettingByName<int>(targetKey);
                if (!curOrder || !targetOrder) return;

                std::swap(curOrder->value, targetOrder->value);
                normalizeDisplayOrder();
                Client::SaveSettings();
                FlarialGUI::Notify(std::format("Moved grouped item <{}> up.", i));
            }
        );

        addConditionalButton(
            getOps<bool>("groupTextures") && getOps<bool>("useItemTexture" + subIndex) && canMoveDown,
            "Grouped Order",
            std::format("Position {}/{} in grouped layout.", groupedIt == groupedOrdered.end() ? 0 : static_cast<int>(std::distance(groupedOrdered.begin(), groupedIt)) + 1, static_cast<int>(groupedOrdered.size())),
            "Move Down",
            [this, i, moveDownTarget] {
                if (moveDownTarget < 0) return;

                const std::string curKey = std::format("displayOrder-{}", i);
                const std::string targetKey = std::format("displayOrder-{}", moveDownTarget);
                auto* curOrder = settings.getSettingByName<int>(curKey);
                auto* targetOrder = settings.getSettingByName<int>(targetKey);
                if (!curOrder || !targetOrder) return;

                std::swap(curOrder->value, targetOrder->value);
                normalizeDisplayOrder();
                Client::SaveSettings();
                FlarialGUI::Notify(std::format("Moved grouped item <{}> down.", i));
            }
        );

        addToggle("Use Custom Settings", "allows you to change render settings specifically for this item", settings.getSettingByName<bool>("useCustomSettings" + subIndex)->value);
        addConditionalToggle(getOps<bool>("useCustomSettings" + subIndex), "Show Custom Settings", "", settings.getSettingByName<bool>("showCustomSettings" + subIndex)->value);

        multiSettingsRender(i);

        addButton("Delete item", "deletes the item", "Delete", [this, i] {
            const auto it = std::ranges::find(subIndexesToRender, i);
            if (it != subIndexesToRender.end()) subIndexesToRender.erase(it);
            multiClearConfig(i);
            items--;
            normalizeDisplayOrder();
            Client::SaveSettings();
            FlarialGUI::Notify(std::format("Successfully removed item counter with index <{}>!", i));
        });

        extraPadding();
    }

    if (items > 0) {
        addToggle("Show Settings", "", "showSettings");
        if (getOps<bool>("showSettings")) defaultAddSettings("all");
    }


    FlarialGUI::UnsetScrollView();
    resetPadding();
}

void ItemCounter::multiNormalRenderCore(
    int index,
    std::string &text,
    int subIndexInt,
    const bool useCustom,
    const bool showTexture,
    const bool drawCountBottomRight,
    const int itemCount,
    const bool useMinecraftCountFont,
    const Vec2<float>* forcedTopLeft,
    Vec2<float>* outRectSize,
    const bool suppressText
) {
    if (!this->isEnabled()) return;

    if (index >= static_cast<int>(prevAlignments.size())) {
        prevAlignments.resize(static_cast<size_t>(index) + 1, DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
    }

    const bool usingForcedTopLeft = forcedTopLeft != nullptr;

    std::string subIndex = std::format("-{}", subIndexInt);
    const std::string subIndex2 = subIndex;
    if (!useCustom) subIndex = "";

    const float rotation = getOps<float>("rotation" + subIndex);
    DWRITE_TEXT_ALIGNMENT alignment = alignments[getOps<std::string>("textalignment" + subIndex)];
    // Detect "Left of Item" placement mode from the per-item setting.
    const bool countLeftOfItem = showTexture &&
        settings.getSettingByName<std::string>(std::format("textureCountPlacement-{}", subIndexInt)) != nullptr &&
        getOps<std::string>(std::format("textureCountPlacement-{}", subIndexInt)) == "Left of Item";

    // Force TEXT alignment (not element anchor) when in texture mode:
    // "Next To Item" → left-align text (after icon), "Bottom Right" → right, "Left of Item" → right (text hugs icon).
    DWRITE_TEXT_ALIGNMENT textAlignment;
    if (showTexture) {
        if (drawCountBottomRight || countLeftOfItem) {
            textAlignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
        } else {
            textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        }
    } else {
        textAlignment = alignment;
    }
    const bool responsivewidth = getOps<bool>("responsivewidth" + subIndex);
    float paddingX = getOps<float>("padx" + subIndex);
    float paddingY = getOps<float>("pady" + subIndex);

    if (getOps<bool>("reversepaddingx" + subIndex)) paddingX = -(getOps<float>("padx" + subIndex));
    if (getOps<bool>("reversepaddingy" + subIndex)) paddingY = -(getOps<float>("pady" + subIndex));

    const float textWidth = Constraints::RelativeConstraint(0.7f * getOps<float>("uiscale" + subIndex));
    const float textHeight = Constraints::RelativeConstraint(0.1f * getOps<float>("uiscale" + subIndex));
    const float textSize = Constraints::SpacingConstraint(3.2f, textHeight) * getOps<float>("textscale" + subIndex);
    const float textureSize = textHeight * 0.95f;
    const bool hasVisibleText = !text.empty();
    const float textureGap = (showTexture && hasVisibleText) ? Constraints::SpacingConstraint(0.08f, textHeight) : 0.0f;
    const float textureWidth = showTexture ? (textureSize + textureGap) : 0.0f;

    Vec2<float> settingperc = Vec2(getOps<float>("percentageX" + subIndex2), getOps<float>("percentageY" + subIndex2));

    const float realspacing = Constraints::SpacingConstraint(0.05f, textWidth);

    ImVec2 textMetrics = ImVec2(0.0f, 0.0f);
    if (hasVisibleText) {
        textMetrics = FlarialGUI::getFlarialTextSize(
            FlarialGUI::to_wide(text).c_str(),
            1000000,
            textHeight,
            textAlignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL, true
        );
    }

    const float textContentWidth = hasVisibleText ? (textMetrics.x + Constraints::SpacingConstraint(2.0f, realspacing)) : 0.0f;

    // For "Left of Item", always use fixed width so the icon stays at a stable
    // position when text content changes (e.g., "9x" → "10x" with proportional fonts).
    // The icon is at rectWidth - textureSize, so a responsive rectWidth shifts it.
    const float rectWidth = (
        (countLeftOfItem || !responsivewidth)
            ? (Constraints::RelativeConstraint(0.225f * getOps<float>("uiscale" + subIndex)) * getOps<float>("rectwidth" + subIndex))
            : textContentWidth * getOps<float>("rectwidth" + subIndex)
    ) + textureWidth;
    const float rectHeight = textHeight * getOps<float>("rectheight" + subIndex);

    Vec2<float> topleft;

    if (usingForcedTopLeft) {
        topleft = *forcedTopLeft;
    } else {
        if (settingperc.x != 0) topleft = Vec2(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
        else topleft = Constraints::CenterConstraint(rectWidth, rectHeight);

        if (prevAlignments[index] == DWRITE_TEXT_ALIGNMENT_JUSTIFIED) prevAlignments[index] = alignment;

        if (prevAlignments[index] != alignment) {
            float toAdjust = 0;
            const auto prev = prevAlignments[index];

            if (prev != alignment) {
                if (prev == DWRITE_TEXT_ALIGNMENT_CENTER) {
                    toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_LEADING ? -0.5f : 0.5f) * rectWidth;
                } else if (prev == DWRITE_TEXT_ALIGNMENT_LEADING) {
                    toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_CENTER ? 0.5f : 1.f) * rectWidth;
                } else if (prev == DWRITE_TEXT_ALIGNMENT_TRAILING) {
                    toAdjust = (alignment == DWRITE_TEXT_ALIGNMENT_CENTER ? -0.5f : -1.f) * rectWidth;
                }
            }

            settings.setValue("percentageX" + subIndex2, (topleft.x + toAdjust) / MC::windowSize.x);

            settingperc = Vec2(getOps<float>("percentageX" + subIndex2), getOps<float>("percentageY" + subIndex2));
            topleft = Vec2(settingperc.x * (MC::windowSize.x), settingperc.y * (MC::windowSize.y));
        }

        prevAlignments[index] = alignment;

        if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) {
            topleft.x -= (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);
        }

        if (ClickGUI::editmenu) {
            FlarialGUI::SetWindowRect(topleft.x, topleft.y, rectWidth, rectHeight, index, this->name + subIndex);

            checkForRightClickAndOpenSettings(topleft.x, topleft.y, rectWidth, rectHeight);

            Vec2<float> vec2 = FlarialGUI::CalculateMovedXY(topleft.x, topleft.y, index, rectWidth, rectHeight);

            if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) vec2.x += (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);

            topleft.x = vec2.x;
            topleft.y = vec2.y;

            const Vec2<float> percentages = Constraints::CalculatePercentage(topleft.x, topleft.y, 0, 0);
            settings.setValue("percentageX" + subIndex2, percentages.x);
            settings.setValue("percentageY" + subIndex2, percentages.y);

            if (alignment != DWRITE_TEXT_ALIGNMENT_LEADING) topleft.x -= (alignment == DWRITE_TEXT_ALIGNMENT_TRAILING ? rectWidth : rectWidth / 2.f);
        }
    }

    const Vec2<float> rounde = Constraints::RoundingConstraint(getOps<float>("rounding" + subIndex) * getOps<float>("uiscale" + subIndex), getOps<float>("rounding" + subIndex) * getOps<float>("uiscale" + subIndex));
    ImVec2 rotationCenter;

    if (rotation > 0.0f) {
        rotationCenter = ImVec2(topleft.x + rectWidth / 2.0f, topleft.y + rectHeight / 2.0f);
        FlarialGUI::ImRotateStart();
    }

    // Suppress background elements when in texture mode — they render in the D2D layer
    // which composites on top of the MC item texture rendered in onSetupAndRender.
    // Use text shadow instead of background for texture-mode entries.
    if (!showTexture) {
        if (getOps<bool>("glow" + subIndex))
            FlarialGUI::ShadowRect(
                Vec2(topleft.x, topleft.y),
                Vec2(rectWidth, textHeight * getOps<float>("rectheight" + subIndex)),
                getColor("glow" + subIndex), rounde.x,
                (getOps<float>("glowAmount" + subIndex) / 100.f) * Constraints::PercentageConstraint(0.1f, "top" + subIndex));

        float blur = Client::settings.getSettingByName<float>("blurintensity")->value;
        if (getOps<bool>("BlurEffect" + subIndex)) FlarialGUI::BlurRect(D2D1::RoundedRect(D2D1::RectF(topleft.x, topleft.y, topleft.x + rectWidth, topleft.y + rectHeight), rounde.x, rounde.x));

        if (getOps<bool>("rectShadow" + subIndex))
            FlarialGUI::RoundedRect(
                topleft.x + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
                topleft.y + Constraints::RelativeConstraint(getOps<float>("rectShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
                getColor("rectShadow" + subIndex),
                rectWidth,
                rectHeight,
                rounde.x,
                rounde.x
            );

        if (getOps<bool>("showBg" + subIndex))
            FlarialGUI::RoundedRect(
                topleft.x,
                topleft.y,
                getColor("bg" + subIndex),
                rectWidth,
                rectHeight,
                rounde.x,
                rounde.x
            );
    }

    const float paddingOffsetX = Constraints::SpacingConstraint(paddingX, textWidth);
    const float iconRenderY = topleft.y + (rectHeight - textureSize) / 2.0f;
    // In texture mode, align text vertically with the icon instead of using rect-based paddingY.
    // PaddingY was designed for positioning text within a background rect; in texture mode
    // (no background), it would displace the text far from the icon.
    const float textRenderY = showTexture
        ? iconRenderY
        : topleft.y + Constraints::SpacingConstraint(paddingY, textWidth);

    // "Left of Item": text on left, icon on right  →  "1x [diamond]"
    // "Next To Item": icon on left, text on right  →  "[diamond] 1x"
    float textRenderX, iconRenderX;
    float textRectWidth = rectWidth; // available width for text layout
    if (countLeftOfItem) {
        textRenderX = topleft.x + paddingOffsetX;
        iconRenderX = topleft.x + rectWidth - textureSize;
        textRectWidth = rectWidth - textureWidth; // constrain so text doesn't overlap icon
    } else {
        textRenderX = topleft.x + paddingOffsetX + textureWidth;
        iconRenderX = topleft.x + paddingOffsetX;
    }
    const bool drawBottomRightCount = showTexture && drawCountBottomRight && itemCount > 0;

    if (showTexture && SDK::hasInstanced && SDK::clientInstance != nullptr && SDK::clientInstance->getGuiData() != nullptr) {
        const float guiScale = std::max(SDK::clientInstance->getGuiData()->getGuiScale(), 0.001f);
        const float iconScale = std::max(textureSize / (16.0f * guiScale), 0.01f);

        const float uiscaleVal = getOps<float>("uiscale" + subIndex);
        const float shadowOffsetD2D = Constraints::RelativeConstraint(getOps<float>("textShadowOffset" + subIndex)) * uiscaleVal;
        // Scale MC font proportionally with uiscale (normalized to default 0.65) and textscale.
        const float fontScale = getOps<float>("textscale" + subIndex) * (uiscaleVal / 0.65f);

        iconRenderQueue.push_back(IconRenderRequest{
            subIndexInt,
            iconRenderX,
            iconRenderY,
            iconScale,
            textureSize,
            itemCount,
            drawBottomRightCount,
            useMinecraftCountFont,
            shadowOffsetD2D,
            fontScale
        });
    }

    if (hasVisibleText && !suppressText) {
        if (getOps<bool>("textShadow" + subIndex))
            FlarialGUI::FlarialTextWithFont(
                textRenderX + Constraints::RelativeConstraint(getOps<float>("textShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
                textRenderY + Constraints::RelativeConstraint(getOps<float>("textShadowOffset" + subIndex)) * getOps<float>("uiscale" + subIndex),
                FlarialGUI::to_wide(text).c_str(),
                textRectWidth,
                rectHeight,
                textAlignment,
                textSize, DWRITE_FONT_WEIGHT_NORMAL,
                getColor("textShadow" + subIndex),
                true
            );

        FlarialGUI::FlarialTextWithFont(
            textRenderX,
            textRenderY,
            FlarialGUI::to_wide(text).c_str(),
            textRectWidth,
            rectHeight,
            textAlignment,
            textSize, DWRITE_FONT_WEIGHT_NORMAL,
            getColor("text" + subIndex),
            true
        );
    }

    if (drawBottomRightCount && !useMinecraftCountFont) {
        const std::string countText = std::to_string(itemCount);
        const float countRectX = iconRenderX + textureSize * (2.0f / 16.0f);
        const float countRectY = iconRenderY + textureSize * (9.0f / 16.0f);
        const float countRectSize = textureSize;
        const float countTextSize = std::max(1.0f, Constraints::SpacingConstraint(2.6f, textureSize));
        const float countShadowOffset = std::max(1.0f, textureSize * 0.08f);

        if (getOps<bool>("textShadow" + subIndex)) {
            FlarialGUI::FlarialTextWithFont(
                countRectX + countShadowOffset,
                countRectY + countShadowOffset,
                FlarialGUI::to_wide(countText).c_str(),
                countRectSize,
                countRectSize,
                DWRITE_TEXT_ALIGNMENT_TRAILING,
                countTextSize,
                DWRITE_FONT_WEIGHT_NORMAL,
                getColor("textShadow" + subIndex),
                true
            );
        }

        FlarialGUI::FlarialTextWithFont(
            countRectX,
            countRectY,
            FlarialGUI::to_wide(countText).c_str(),
            countRectSize,
            countRectSize,
            DWRITE_TEXT_ALIGNMENT_TRAILING,
            countTextSize,
            DWRITE_FONT_WEIGHT_NORMAL,
            getColor("text" + subIndex),
            true
        );
    }


    if (getOps<bool>("border" + subIndex))
        FlarialGUI::RoundedHollowRect(
            topleft.x,
            topleft.y,
            Constraints::RelativeConstraint((getOps<float>("borderWidth" + subIndex) * getOps<float>("uiscale" + subIndex)) / 100.0f, "height", true),
            getColor("border" + subIndex),
            rectWidth,
            textHeight * getOps<float>("rectheight" + subIndex),
            rounde.x,
            rounde.x
        );

    if (outRectSize != nullptr) {
        *outRectSize = Vec2(rectWidth, rectHeight);
    }

    if (rotation > 0.0f) FlarialGUI::ImRotateEnd(rotation + 90.f, rotationCenter);

    if (!usingForcedTopLeft && ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
}

void ItemCounter::onTick(TickEvent& event) {
    if (!this->isEnabled()) return;
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) return;

    if (SDK::getCurrentScreen() != "hud_screen") return;

    const auto player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies()) return;

    const auto supplies = player->getSupplies();
    const auto inventory = supplies->getInventory();
    if (!inventory) return;

    // Get the item registry for validation (once per tick, shared across all sub-indexes).
    // The registry lives on Level, not ClientInstance — access it via the player's Level*.
    auto* level = player->getLevel();
    ItemRegistryRef registryRef{};
    if (level) {
        registryRef = level->getItemRegistry();
    }
    const bool isRegistryAvailable = registryRef.valid();

    // Snapshot previous seen/resolvedId state so we can keep items "sticky" when count drops to 0.
    // Without this, showTexture flips false at 0 count, causing the renderer to fall back to
    // text-only mode (background rect, wrong opacity) while still showing texture-mode text like ": 0".
    std::unordered_map<int, bool> prevItemSeen;
    std::unordered_map<int, std::string> prevResolvedId;
    {
        std::lock_guard<std::mutex> lock(tickDataMutex);
        prevItemSeen = itemSeenInInventory;
        prevResolvedId = resolvedItemIdBySubIndex;
    }

    // Build into local maps first, then swap into the member maps atomically.
    // This prevents the 1-frame glitch where onRender reads empty/partially-filled maps
    // during the gap between clear() and repopulation.
    std::unordered_map<int, int> newItemCount;
    std::unordered_map<int, bool> newItemSeen;
    std::unordered_map<int, std::string> newResolvedId;

    for (int i : subIndexesToRender) {
        newItemCount[i] = 0;
        newItemSeen[i] = false;

        const SettingType<std::string>* setting = settings.getSettingByName<std::string>(std::format("itemName-{}", i));
        if (setting == nullptr) continue;

        ItemQuery query = parseItemQuery(setting->value);
        if (!query.formatValid) continue;

        // Registry validation: check if the item name exists in the game's ItemRegistry.
        // Lock protects registryValidationCache from concurrent find() in settingsRender.
        if (isRegistryAvailable) {
            std::lock_guard<std::mutex> lock(tickDataMutex);
            if (!registryValidationCache.contains(query.normalizedInput)) {
                const bool found = ItemRegistryHelper::validateItemName(
                    registryRef, query.normalizedInput,
                    query.namespacePart, query.itemPart, query.hasNamespace);
                registryValidationCache[query.normalizedInput] = found;
            }
        }

        int itemCount = 0;
        bool matched = false;
        std::string resolvedId = {};

        ItemStack* offhandItem = player->getOffhandSlot();
        std::string offhandResolvedId = {};
        if (doesStackMatchQuery(offhandItem, query, &offhandResolvedId)) {
            itemCount += offhandItem->count;
            matched = true;
            resolvedId = offhandResolvedId;
        }

        for (int slot = 0; slot < 36; slot++) {
            ItemStack* item = inventory->getItem(slot);
            std::string stackResolvedId = {};
            if (doesStackMatchQuery(item, query, &stackResolvedId)) {
                itemCount += item->count;
                if (!matched) {
                    matched = true;
                    resolvedId = stackResolvedId;
                }
            }
        }

        newItemCount[i] = itemCount;
        newItemSeen[i] = matched;
        if (matched && !resolvedId.empty()) {
            newResolvedId[i] = resolvedId;
        }

        // Sticky seen: once an item has been found in inventory, keep it "seen" even at 0 count.
        // This prevents showTexture from flipping false, which would switch the renderer to
        // text-only mode (opaque background, wrong layout) while still using texture-mode text.
        if (!matched) {
            if (auto it = prevItemSeen.find(i); it != prevItemSeen.end() && it->second) {
                newItemSeen[i] = true;
                if (auto rid = prevResolvedId.find(i); rid != prevResolvedId.end()) {
                    newResolvedId[i] = rid->second;
                }
            }
        }
    }

    // Swap fully-populated maps into members under lock — onRender/settingsRender snapshot
    // these maps under the same lock, so they never see a half-moved state.
    {
        std::lock_guard<std::mutex> lock(tickDataMutex);
        registryAvailable = isRegistryAvailable;
        itemCountBySubIndex = std::move(newItemCount);
        itemSeenInInventory = std::move(newItemSeen);
        resolvedItemIdBySubIndex = std::move(newResolvedId);
        tickDataReady = true;
    }
}

void ItemCounter::onRender(RenderEvent& event) {
    if (!this->isEnabled() || ClickGUI::blurActive) return;
    ClickGUI::HudFadeGuard fadeGuard;
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) return;
    if (SDK::clientInstance->getScreenName() != "hud_screen") return;

    iconRenderQueue.clear();

    // Snapshot tick-thread maps under lock — all reads below use these locals,
    // so the rest of onRender is completely lock-free.
    std::unordered_map<int, int> snapItemCount;
    std::unordered_map<int, bool> snapItemSeen;
    std::unordered_map<int, std::string> snapResolvedId;
    bool snapTickReady;
    {
        std::lock_guard<std::mutex> lock(tickDataMutex);
        snapItemCount = itemCountBySubIndex;
        snapItemSeen = itemSeenInInventory;
        snapResolvedId = resolvedItemIdBySubIndex;
        snapTickReady = tickDataReady;
    }

    const bool groupTextures = getOps<bool>("groupTextures");
    const bool groupVertical = getOps<bool>("groupVertical");
    const float groupSpacing = std::max(getOps<float>("groupSpacing"), 0.0f);

    Vec2<float> groupCursor = Vec2(0.0f, 0.0f);
    Vec2<float> groupStart = Vec2(0.0f, 0.0f);
    float groupMaxCrossSize = 0.0f;
    if (groupTextures) {
        Vec2<float> groupPerc = Vec2(getOps<float>("percentageX"), getOps<float>("percentageY"));
        if (groupPerc.x != 0 || groupPerc.y != 0) {
            groupCursor = Vec2(groupPerc.x * MC::windowSize.x, groupPerc.y * MC::windowSize.y);
        } else {
            groupCursor = Constraints::CenterConstraint(0, 0);
        }

        // Edit mode: provide a drag rect for the entire group using previous frame's bounds.
        if (ClickGUI::editmenu) {
            const float gw = lastGroupBounds.x > 1.0f ? lastGroupBounds.x : 50.0f;
            const float gh = lastGroupBounds.y > 1.0f ? lastGroupBounds.y : 30.0f;

            FlarialGUI::SetWindowRect(groupCursor.x, groupCursor.y, gw, gh, 70, this->name);
            checkForRightClickAndOpenSettings(groupCursor.x, groupCursor.y, gw, gh);

            Vec2<float> moved = FlarialGUI::CalculateMovedXY(groupCursor.x, groupCursor.y, 70, gw, gh);
            groupCursor = moved;

            const Vec2<float> percentages = Constraints::CalculatePercentage(moved.x, moved.y, 0, 0);
            settings.setValue("percentageX", percentages.x);
            settings.setValue("percentageY", percentages.y);
        }

        groupStart = groupCursor;
    }

    std::vector<int> orderedSubIndexes = getOrderedSubIndexes();

    int _temp = 71;
    for (int i: orderedSubIndexes) {

        std::string text{};
        std::string itemName{};
        if (this->settings.getSettingByName<std::string>(std::format("text-{}", i)) != nullptr) text = getOps<std::string>(std::format("text-{}", i));
        else continue;
        if (this->settings.getSettingByName<std::string>(std::format("itemName-{}", i)) != nullptr) itemName = getOps<std::string>(std::format("itemName-{}", i));
        else continue;

        ItemQuery query = parseItemQuery(itemName);

        // Read from snapshot maps (taken under lock at the top of onRender).
        int itemCount = 0;
        if (auto it = snapItemCount.find(i); it != snapItemCount.end()) itemCount = it->second;
        bool isItemSeen = false;
        if (auto it = snapItemSeen.find(i); it != snapItemSeen.end()) isItemSeen = it->second;

        const bool useItemTexture = settings.getSettingByName<bool>(std::format("useItemTexture-{}", i)) != nullptr &&
            getOps<bool>(std::format("useItemTexture-{}", i));

        // Don't render texture-mode entries until the first tick has populated inventory data.
        // This prevents the ": 0" flash on module enable or when adding new items.
        if (useItemTexture && !snapTickReady) {
            _temp += 1;
            continue;
        }

        const bool replaceNameWithTexture = settings.getSettingByName<bool>(std::format("replaceNameWithTexture-{}", i)) != nullptr &&
            getOps<bool>(std::format("replaceNameWithTexture-{}", i));
        const bool groupedLayoutEntry = groupTextures && useItemTexture;
        const std::string texturePlacement = settings.getSettingByName<std::string>(std::format("textureCountPlacement-{}", i)) != nullptr
            ? getOps<std::string>(std::format("textureCountPlacement-{}", i)) : "Next To Item";
        const bool countBottomRightSetting = texturePlacement == "Bottom Right";
        const bool countLeftOfItem = texturePlacement == "Left of Item";

        // Hide when zero: skip this entry entirely if count is 0 (unless in edit mode).
        const bool hideWhenZero = settings.getSettingByName<bool>(std::format("hideWhenZero-{}", i)) != nullptr &&
            getOps<bool>(std::format("hideWhenZero-{}", i));
        const bool isEditMode = ClickGUI::editmenu;

        if (hideWhenZero && itemCount == 0 && !isEditMode) {
            _temp += 1;
            continue;
        }
        // In edit mode with hide-when-zero, show a preview count of 64.
        if (hideWhenZero && isEditMode && itemCount == 0) {
            itemCount = 64;
            isItemSeen = true;
        }

        const bool showTexture = useItemTexture && query.formatValid && isItemSeen;
        const bool hideCountIfOne = useItemTexture &&
            settings.getSettingByName<bool>(std::format("hideCountIfOne-{}", i)) != nullptr &&
            getOps<bool>(std::format("hideCountIfOne-{}", i));
        // When hideCountIfOne is off, count=1 shows in the bottom-right overlay.
        // When on, count=1 is suppressed (only the icon renders).
        const bool useBottomRightCount = showTexture && countBottomRightSetting && !(hideCountIfOne && itemCount == 1);
        const bool useMinecraftCountFont = settings.getSettingByName<bool>(std::format("textureCountMinecraftFont-{}", i)) != nullptr &&
            getOps<bool>(std::format("textureCountMinecraftFont-{}", i));
        std::string resolvedId = query.normalizedInput;
        if (auto it = snapResolvedId.find(i); it != snapResolvedId.end()) resolvedId = it->second;
        // Use useItemTexture (setting) rather than showTexture (runtime state) to prevent
        // the item name flashing for a frame before the texture appears.
        const std::string nameReplacement = (useItemTexture && replaceNameWithTexture) ? "" : itemName;

        // When texture replaces the name, auto-adjust the format for clean display:
        //   "Next To Item":  ": {value}"   → ": 4"   (icon on left, count on right)
        //   "Left of Item":  "{value}x"    → "4x"    (count on left, icon on right)
        //   "Bottom Right":  no side text (count drawn on the icon itself)
        if (useItemTexture && countBottomRightSetting) {
            text.clear();
        } else if (useItemTexture && replaceNameWithTexture) {
            if (countLeftOfItem) {
                text = "{value}x";
            } else {
                text = ": {value}";
            }
        }

        // Hide count text when exactly 1 — just show the icon alone.
        // For side-by-side layouts (Left of Item / Next To Item), keep text for layout
        // sizing but suppress rendering so the icon doesn't shift position.
        // For Bottom Right, clear text outright (icon-only, no side text to preserve).
        const bool suppressCountText = hideCountIfOne && itemCount == 1 && !countBottomRightSetting;
        if (hideCountIfOne && itemCount == 1 && countBottomRightSetting) {
            text.clear();
        }

        const auto replaceToken = [](std::string& source, const std::string& token, const std::string& replacement) {
            std::string upperSource = String::toUpper(source);
            size_t pos = upperSource.find(token);

            while (pos != std::string::npos) {
                source.replace(pos, token.length(), replacement);
                upperSource = String::toUpper(source);
                pos = upperSource.find(token, pos + replacement.length());
            }
        };

        replaceToken(text, "{VALUE}", useBottomRightCount ? "" : std::to_string(itemCount));
        replaceToken(text, "{NAME}", nameReplacement);
        replaceToken(text, "{ID}", resolvedId.empty() ? itemName : resolvedId);


        if (groupedLayoutEntry) {
            Vec2<float> renderedSize;
            Vec2<float> forcedTopLeft = groupCursor;

            multiNormalRenderCore(
                _temp,
                text,
                i,
                getOps<bool>(std::format("useCustomSettings-{}", i)),
                showTexture,
                useBottomRightCount,
                itemCount,
                useBottomRightCount && useMinecraftCountFont,
                &forcedTopLeft,
                &renderedSize,
                suppressCountText
            );

            // Track max cross-axis size for bounding box (width for vertical, height for horizontal).
            groupMaxCrossSize = std::max(groupMaxCrossSize, groupVertical ? renderedSize.x : renderedSize.y);

            if (groupVertical) groupCursor.y += renderedSize.y * groupSpacing;
            else groupCursor.x += renderedSize.x * groupSpacing;
        } else {
            multiNormalRenderCore(
                _temp,
                text,
                i,
                getOps<bool>(std::format("useCustomSettings-{}", i)),
                showTexture,
                useBottomRightCount,
                itemCount,
                useBottomRightCount && useMinecraftCountFont,
                nullptr,
                nullptr,
                suppressCountText
            );
        }
        _temp += 1;
    }

    // Update group bounding box for next frame's edit-mode drag rect.
    if (groupTextures) {
        if (groupVertical) {
            lastGroupBounds = Vec2(
                std::max(1.0f, groupMaxCrossSize),
                std::max(1.0f, groupCursor.y - groupStart.y)
            );
        } else {
            lastGroupBounds = Vec2(
                std::max(1.0f, groupCursor.x - groupStart.x),
                std::max(1.0f, groupMaxCrossSize)
            );
        }

        if (ClickGUI::editmenu) FlarialGUI::UnsetWindowRect();
    }
}

void ItemCounter::onSetupAndRender(SetupAndRenderEvent& event) {
    if (!this->isEnabled() || iconRenderQueue.empty()) return;
    if (!SDK::hasInstanced || SDK::clientInstance == nullptr) return;
    if (SDK::clientInstance->getScreenName() != "hud_screen") return;

    auto* player = SDK::clientInstance->getLocalPlayer();
    if (!player || !player->getSupplies() || !player->getSupplies()->getInventory()) return;

    auto* muirc = event.getMuirc();
    if (!muirc) return;

    auto* ci = muirc->getClientInstance();
    if (!ci) return;

    BaseActorRenderContext barc(muirc->getScreenContext(), ci, ci->getMinecraftGame());
    if (!barc.itemRenderer) return;

    bool queuedMinecraftCountText = false;
    const CaretMeasureData caretData(-1, true);

    // Global vanilla-count text offsets (fraction of icon size).
    const float vanillaOffsetX = getOps<float>("vanillaCountOffsetX");
    const float vanillaOffsetY = getOps<float>("vanillaCountOffsetY");

    for (const IconRenderRequest& request : iconRenderQueue) {
        const auto* itemNameSetting = settings.getSettingByName<std::string>(std::format("itemName-{}", request.subIndex));
        if (!itemNameSetting) continue;

        ItemQuery query = parseItemQuery(itemNameSetting->value);
        if (!query.formatValid) continue;

        ItemStack* itemToRender = findFirstMatchingStack(player, query);
        if (!itemToRender || !itemToRender->getItem()) continue;

        const Vec2<float> scaledPos = PositionUtils::getScaledPos(Vec2(request.screenX, request.screenY));
        barc.itemRenderer->renderGuiItemNew(
            &barc,
            itemToRender,
            0,
            scaledPos.x,
            scaledPos.y,
            1.0f,
            request.scale,
            itemToRender->isEnchanted()
        );

        if (request.drawBottomRightCount && request.useMinecraftCountFont) {
            const D2D_COLOR_F textColorD2D = getColor(std::format("text-{}", request.subIndex));
            const mce::Color textColorNative(textColorD2D.r, textColorD2D.g, textColorD2D.b, textColorD2D.a);
            const bool drawShadow = settings.getSettingByName<bool>(std::format("textShadow-{}", request.subIndex)) != nullptr &&
                getOps<bool>(std::format("textShadow-{}", request.subIndex));
            const D2D_COLOR_F shadowColorD2D = getColor(std::format("textShadow-{}", request.subIndex));
            const mce::Color shadowColorNative(shadowColorD2D.r, shadowColorD2D.g, shadowColorD2D.b, shadowColorD2D.a);

            const Vec2<float> scaledIconSize = PositionUtils::getScaledPos(Vec2(request.iconSize, request.iconSize));
            const float offsetX = scaledIconSize.x * vanillaOffsetX;
            const float offsetY = scaledIconSize.y * vanillaOffsetY;
            const float textLeft = scaledPos.x + scaledIconSize.x * (2.0f / 16.0f) + offsetX;
            const float textTop = scaledPos.y + scaledIconSize.y * (9.0f / 16.0f) + offsetY;
            const float textRight = textLeft + scaledIconSize.x;
            const float textBottom = textTop + scaledIconSize.y;

            // Use the per-item shadow offset setting (converted from D2D pixels to MC GUI coords).
            const Vec2<float> scaledShadowOffset = PositionUtils::getScaledPos(
                Vec2(request.vanillaShadowOffset, request.vanillaShadowOffset));
            const float shadowOffset = std::max(1.0f, scaledShadowOffset.y);

            // Per-request font scale (accounts for textscale and uiscale).
            const TextMeasureData textData(request.vanillaFontScale, false, false);

            if (drawShadow) {
                RectangleArea shadowRect(
                    textLeft + shadowOffset,
                    textRight + shadowOffset,
                    textTop + shadowOffset,
                    textBottom + shadowOffset
                );
                muirc->drawDebugText(
                    shadowRect,
                    std::to_string(request.itemCount),
                    shadowColorNative,
                    1.0f,
                    ui::TextAlignment::RIGHT,
                    textData,
                    caretData
                );
            }

            RectangleArea textRect(textLeft, textRight, textTop, textBottom);
            muirc->drawDebugText(
                textRect,
                std::to_string(request.itemCount),
                textColorNative,
                1.0f,
                ui::TextAlignment::RIGHT,
                textData,
                caretData
            );
            queuedMinecraftCountText = true;
        }
    }

    if (queuedMinecraftCountText) {
        muirc->flushText(0.0f);
    }
}
