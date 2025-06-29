#pragma once
#include "json.hpp"


enum class ImageFormat : int {
    None = 0x0,
    R8Unorm = 0x1,
    RGB8Unorm = 0x2,
    RGBA8Unorm = 0x3,
};

namespace Bedrock::Application {
    template<typename T0>
    class ThreadOwner {
    public:
        T0 mObject;
        bool mThreadIdInitialized{};
        std::thread::id mThreadId;
        u_int mThreadCheckIndex{};

    public:
        // prevent constructor by default
        ThreadOwner &operator=(ThreadOwner const &) = delete;

        ThreadOwner(ThreadOwner const &) = delete;

        ThreadOwner() = delete;
    };
}

class Blob {
public:
    using value_type = unsigned char;
    using size_type = size_t;
    using pointer = value_type *;
    using iterator = value_type *;
    using const_pointer = value_type const *;
    using const_iterator = value_type const *;

    using delete_function = void (*)(pointer);

    struct Deleter {
    public:
        delete_function mFn;

        [[nodiscard]] _CONSTEXPR23 Deleter() : mFn(Blob::defaultDeleter) {
        }

        [[nodiscard]] _CONSTEXPR23 Deleter(delete_function fn) : mFn(fn) {
        }

        void operator()(pointer x) const { mFn(x); }
    };

    using pointer_type = std::unique_ptr<value_type[], Deleter>;

    pointer_type mBlob{}; // this+0x0
    size_type mSize{}; // this+0x10

    [[nodiscard]] _CONSTEXPR23 Blob() = default;

    [[nodiscard]] _CONSTEXPR23 Blob(std::span<value_type> s, Deleter deleter = {}) : mSize(s.size()) { // NOLINT
        mBlob = pointer_type(new value_type[mSize], deleter);
        std::copy(s.begin(), s.end(), mBlob.get());
    }

    [[nodiscard]] _CONSTEXPR23 pointer data() const { return mBlob.get(); }

    [[nodiscard]] _CONSTEXPR23 size_type size() const { return mSize; }

    [[nodiscard]] _CONSTEXPR23 std::span<value_type> view() const { return {data(), size()}; }

    //LL_CLANG_CEXPR Blob& operator=(Blob&&) noexcept = default;
    [[nodiscard]] _CONSTEXPR23 Blob(Blob &&) noexcept = default;

    [[nodiscard]] _CONSTEXPR23 Blob(Blob const &other) : Blob(other.view(), other.mBlob.get_deleter()) {
    }

    _CONSTEXPR23 Blob &operator=(Blob const &other) {
        if (this != &other) {
            mSize = other.mSize;
            mBlob = pointer_type(new value_type[mSize], other.mBlob.get_deleter());
            std::copy(other.data(), other.data() + mSize, mBlob.get());
        }
        return *this;
    }

    static Blob fromVector(std::vector<unsigned char> const &vec) {
        Blob blob;
        blob.mSize = vec.size();
        blob.mBlob = pointer_type(new value_type[blob.mSize], Deleter());
        std::copy(vec.begin(), vec.end(), blob.mBlob.get());
        return blob;
    }

public:
    static inline void defaultDeleter(pointer ptr) { delete[] ptr; }
};

enum class ImageUsage : unsigned char {
    Unknown = 0x0,
    sRGB = 0x1,
    Data = 0x2,
};

struct Image {
public:
    ImageFormat imageFormat{}; // this+0x0
    uint32_t mWidth{}; // this+0x4
    uint32_t mHeight{}; // this+0x8
    uint32_t mDepth{}; // this+0xC
    ImageUsage mUsage{}; // this+0x10
    Blob mImageBytes; // this+0x18

    [[nodiscard]] constexpr Image() = default;

    //LL_CLANG_CEXPR Image& operator=(Image&&) noexcept = default;
    [[nodiscard]] constexpr Image(Image &&) noexcept = default;

    //LL_CLANG_CEXPR Image& operator=(Image const&) noexcept = default;
    [[nodiscard]] constexpr Image(Image const &) noexcept = default;
};

enum class AnimatedTextureType : unsigned int {
    None = 0,
    Face = 1,
    Body32x32 = 2,
    Body128x128 = 3,
};

enum class AnimationExpression : unsigned int {
    Linear = 0,
    Blinking = 1,
};

class AnimatedImageData {
    AnimatedTextureType mType;
    AnimationExpression mExpression;
    Image mImage;
    alignas(4) float mFrames;
};

class MinEngineVersion {
public:
    std::aligned_storage_t<24, 8> mUnkce6b03;
    std::aligned_storage_t<4, 4> mUnk7524e3;
    std::aligned_storage_t<2, 2> mUnk2c6d18;
};

namespace persona {
    enum class PieceType : unsigned int {
        Unknown = 0,
        Skeleton = 1,
        Body = 2,
        Skin = 3,
        Bottom = 4,
        Feet = 5,
        Dress = 6,
        Top = 7,
        HighPants = 8,
        Hands = 9,
        Outerwear = 10,
        FacialHair = 11,
        Mouth = 12,
        Eyes = 13,
        Hair = 14,
        Hood = 15,
        Back = 16,
        FaceAccessory = 17,
        Head = 18,
        Legs = 19,
        LeftLeg = 20,
        RightLeg = 21,
        Arms = 22,
        LeftArm = 23,
        RightArm = 24,
        Capes = 25,
        ClassicSkin = 26,
        Emote = 27,
        Unsupported = 28,
        Count = 29,
    };
}

class mcUUID {
public:
    uint64_t mostSig, leastSig;

    bool operator==(const mcUUID &other) const {
        return mostSig == other.mostSig && leastSig == other.leastSig;
    }

    bool operator!=(const mcUUID &other) const {
        return !(*this == other);
    }
};

class SerializedPersonaPieceHandle {
public:
    std::string mPieceId;
    persona::PieceType mPieceType;
    mcUUID mPackId;
    bool mIsDefaultPiece;
    std::string mProductId;
};

namespace persona {
    class ArmSize {
    public:
        // ArmSize inner types define
        enum class Type : uint64_t {
            Slim = 0,
            Wide = 1,
            Count = 2,
            Unknown = 3,
        };
    };
}

class TintMapColor {
public:
    std::aligned_storage_t<64, 4> colors;
};

enum class TrustedSkinFlag : signed char {
    Unset = 0,
    False = 1,
    True  = 2,
};

class PlayerSkin {
public:
    std::string mId; // this+0x0
    std::string mPlayFabId; // this+0x20
    std::string mFullId; // this+0x40
    std::string mResourcePatch; // this+0x60
    std::string mDefaultGeometryName; // this+0x80
    Image mSkinImage; // this+0xA0
    Image mCapeImage; // this+0xD0
    char idklolwtfisthis[272];
    // std::vector<AnimatedImageData> mSkinAnimatedImages;
    // MinecraftJson::Value mGeometryData;
    // MinEngineVersion mGeometryDataMinEngineVersion;
    // MinecraftJson::Value mGeometryDataMutable;
    // std::string mAnimationData;
    // std::string mCapeId;
    // std::vector<SerializedPersonaPieceHandle> mPersonaPieces;
    // persona::ArmSize::Type mArmSizeType;
    // std::unordered_map<persona::PieceType, TintMapColor> mPieceTintColors;
    // char mSkinColor[16];
    // TrustedSkinFlag mIsTrustedSkin;
    // bool mIsPremium;
    // bool mIsPersona;
    // bool mIsPersonaCapeOnClassicSkin;
    // bool mIsPrimaryUser;
    // bool mOverridesPlayerAppearance;
};

class PlayerSkin_1_21_90 {
public:
    std::shared_ptr<Bedrock::Application::ThreadOwner<PlayerSkin> > impl;
};

namespace std {
    template<>
    struct hash<mcUUID> {
        size_t operator()(const mcUUID &uuid) const {
            // Combine the two 64-bit values using a simple hash combination
            return hash<uint64_t>{}(uuid.mostSig) ^ (hash<uint64_t>{}(uuid.leastSig) << 1);
        }
    };
}
