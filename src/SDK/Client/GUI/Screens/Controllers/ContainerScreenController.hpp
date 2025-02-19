#pragma once

class ContainerScreenController {
public:
    void _handlePlaceAll(std::string collectionName, int32_t slot);
    void _handleTakeAll(std::string collectionName, int32_t slot);
    void swap(std::string srcCollectionName, int32_t srcSlot, std::string dstCollectionName, int32_t dstSlot);
};
