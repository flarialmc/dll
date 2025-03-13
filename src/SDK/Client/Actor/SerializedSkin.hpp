#pragma once
#include "../Render/image.hpp"
#include <string>

class SerializedSkin {
public:
    std::string                                              mId;
    std::string                                              mPlayFabId;
    std::string                                              mFullId;
    std::string                                              mResourcePatch;
    std::string                                              mDefaultGeometryName;
    mce::Image                                               mSkinImage;
    mce::Image                                               mCapeImage;
};