#pragma once


enum class ImageFormat : int {
    None = 0x0,
    R8Unorm = 0x1,
    RGB8Unorm = 0x2,
    RGBA8Unorm = 0x3,
};

class Blob {
public:
    using value_type = unsigned char;
    using size_type = size_t;
    using pointer = value_type*;
    using iterator = value_type*;
    using const_pointer = value_type const*;
    using const_iterator = value_type const*;

    using delete_function = void (*)(pointer);

    struct Deleter {
    public:
        delete_function mFn;

        [[nodiscard]] _CONSTEXPR23 Deleter() : mFn(Blob::defaultDeleter) {}

        [[nodiscard]] _CONSTEXPR23 Deleter(delete_function fn) : mFn(fn) {}

        void operator()(pointer x) const { mFn(x); }
    };

    using pointer_type = std::unique_ptr<value_type[], Deleter>;

    pointer_type mBlob{}; // this+0x0
    size_type    mSize{}; // this+0x10

    [[nodiscard]] _CONSTEXPR23 Blob() = default;

    [[nodiscard]] _CONSTEXPR23 Blob(std::span<value_type> s, Deleter deleter = {}) : mSize(s.size()) { // NOLINT
        mBlob = pointer_type(new value_type[mSize], deleter);
        std::copy(s.begin(), s.end(), mBlob.get());
    }

    [[nodiscard]] _CONSTEXPR23 pointer data() const { return mBlob.get(); }

    [[nodiscard]] _CONSTEXPR23 size_type size() const { return mSize; }

    [[nodiscard]] _CONSTEXPR23 std::span<value_type> view() const { return { data(), size() }; }

    //LL_CLANG_CEXPR Blob& operator=(Blob&&) noexcept = default;
    [[nodiscard]] _CONSTEXPR23 Blob(Blob&&) noexcept = default;

    [[nodiscard]] _CONSTEXPR23 Blob(Blob const& other) : Blob(other.view(), other.mBlob.get_deleter()) {}

    _CONSTEXPR23 Blob& operator=(Blob const& other) {
        if (this != &other) {
            mSize = other.mSize;
            mBlob = pointer_type(new value_type[mSize], other.mBlob.get_deleter());
            std::copy(other.data(), other.data() + mSize, mBlob.get());
        }
        return *this;
    }

    static Blob fromVector(std::vector<unsigned char> const& vec) {
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
    uint32_t        mWidth{};      // this+0x4
    uint32_t        mHeight{};     // this+0x8
    uint32_t        mDepth{};      // this+0xC
    ImageUsage  mUsage{};      // this+0x10
    Blob        mImageBytes;   // this+0x18

    [[nodiscard]] constexpr Image() = default;
    //LL_CLANG_CEXPR Image& operator=(Image&&) noexcept = default;
    [[nodiscard]] constexpr Image(Image&&) noexcept = default;
    //LL_CLANG_CEXPR Image& operator=(Image const&) noexcept = default;
    [[nodiscard]] constexpr Image(Image const&) noexcept = default;
};

class PlayerSkin {
public:
    std::string                                          mId;                             // this+0x0
    std::string                                          mPlayFabId;                      // this+0x20
    std::string                                          mFullId;                         // this+0x40
    std::string                                          mResourcePatch;                  // this+0x60
    std::string                                          mDefaultGeometryName;            // this+0x80
    Image                                                mSkinImage;                      // this+0xA0
    Image                                                mCapeImage;                      // this+0xC0
};