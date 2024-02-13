#pragma once
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>

struct MCCColor {
    union {
        struct {
            float r, g, b, a;
        };
        float arr[4];
    };
    bool shouldDelete = true;

    MCCColor() {
        this->r = 1;
        this->g = 1;
        this->b = 1;
        this->a = 1;
    };

    MCCColor(const MCCColor& other) {
        this->r = other.r;
        this->g = other.g;
        this->b = other.b;
        this->a = other.a;
        this->shouldDelete = other.shouldDelete;
    }

    MCCColor(const float* arr) {
        this->arr[0] = arr[0];
        this->arr[1] = arr[1];
        this->arr[2] = arr[2];
        this->arr[3] = arr[3];
    };

    MCCColor(const float r, const float g, const float b, const float a = 1) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    };

    MCCColor(const int r, const int g, const int b, const int a = 255) {
        this->r = r / 255.0f;
        this->g = g / 255.0f;
        this->b = b / 255.0f;
        this->a = a / 255.0f;
    };

    MCCColor(const float r, const float g, const float b, const float a, const bool shouldDelete) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
        this->shouldDelete = shouldDelete;
    };

    MCCColor lerp(const MCCColor& o, float t) const;

    MCCColor changeBrightness(float b) const;
};

template<typename T>
class Vec2 {
public:
    T x = 0, y = 0;
public:
    Vec2(T x = 0, T y = 0) {
        this->x = x;
        this->y = y;
    };
public:
    auto sub(const Vec2<T>& v) -> Vec2<T> {
        return Vec2<T>(this->x - v.x, this->y - v.y);
    };
    auto add(const Vec2<T>& v) -> Vec2<T> {
        return Vec2<T>(this->x + v.x, this->y + v.y);
    };
    auto div(const Vec2<T>& v) -> Vec2<T> {
        return Vec2<T>(this->x / v.x, this->y / v.y);
    };
    auto mul(const Vec2<T>& v) -> Vec2<T> {
        return Vec2<T>(this->x * v.x, this->y * v.y);
    };
    auto dist(const Vec2<T>& v) -> float {
        auto dX = this->x - v.x;
        auto dY = this->y - v.y;

        return sqrt(dX * dX + dY * dY);
    };
    auto ToInt() {
        int x = this->x;
        int y = this->y;
        return Vec2<int>(x, y);
    }
};

template<typename T>
class Vec3 : public Vec2<T> {
public:
    T z = 0;
public:
    Vec3(T x = 0, T y = 0, T z = 0) : Vec2<T>(x, y) {
        this->z = z;
    };

    auto ToFloat() {
        float x = this->x;
        float y = this->y;
        float z = this->z;
        return Vec3<float>(x, y, z);
    }

    auto add(T x, T y, T z) -> Vec3<T> {
        return Vec3<T>(this->x + x, this->y + y, this->z + z);
    };

    auto add(const Vec3<T>& vec) -> Vec3<T> {
        return Vec3<T>(this->x + vec.x, this->y + vec.y, this->z + vec.z);
    };

    auto add(T v) -> Vec3<T> {
        return Vec3<T>(this->x + v, this->y + v, this->z + v);
    };

    auto sub(T x, T y, T z) -> Vec3<T> {
        return Vec3<T>(this->x + x, this->y + y, this->z + z);
    };

    auto sub(const Vec3<T>& vec) -> Vec3<T> {
        return Vec3<T>(this->x - vec.x, this->y - vec.y, this->z - vec.z);
    };

    auto sub(T v) -> Vec3<T> {
        return Vec3<T>(this->x - v, this->y - v, this->z - v);
    };

    auto floor() -> Vec3<T> {
        return Vec3<T>(std::floor(this->x), std::floor(this->y), std::floor(this->z));
    };

    auto div(T x, T y, T z) -> Vec3<T> {
        return Vec3<T>(this->x / x, this->y / y, this->z / z);
    };

    auto div(const Vec3<T>& vec) -> Vec3<T> {
        return Vec3<T>(this->x / vec.x, this->y / vec.y, this->z / vec.z);
    };

    auto div(T v) -> Vec3<T> {
        return Vec3<T>(this->x / v, this->y / v, this->z / v);
    };

    auto mul(T x, T y, T z) -> Vec3<T> {
        return Vec3<T>(this->x * x, this->y * y, this->z * z);
    };

    auto mul(const Vec3<T>& vec) -> Vec3<T> {
        return Vec3<T>(this->x * vec.x, this->y * vec.y, this->z * vec.z);
    };

    auto mul(T v) -> Vec3<T> {
        return Vec3<T>(this->x * v, this->y * v, this->z * v);
    };
public:
    auto dist(const Vec3<T> pos) -> float {
        return sqrt((std::pow(this->x - pos.x, 2)) + (std::pow(this->y - pos.y, 2)) + (std::pow(this->z - pos.z, 2)));
    };
};

template<typename T>
class Vec4 : public Vec3<T> {
public:
    T w = 0;
public:
    Vec4(T x = 0, T y = 0, T z = 0, T w = 0) : Vec3<T>(x, y, z) {
        this->w = w;
    };
};

struct AABB {
    Vec3<float> lower;
    Vec3<float> upper;

    AABB() {}
    AABB(Vec3<float> l, Vec3<float> h) : lower(l), upper(h) {};
    AABB(const AABB& aabb) {
        lower = Vec3<float>(aabb.lower);
        upper = Vec3<float>(aabb.upper);
    }
    AABB(Vec3<float> lower, float width, float height, float eyeHeight) {
        lower = lower.sub(Vec3<float>(width, eyeHeight * 2, width).div(2));
        this->lower = lower;
        this->upper = { lower.x + width, lower.y + height, lower.z + width };
    }

    bool isFullBlock() {
        auto diff = lower.sub(upper);
        return fabsf(diff.y) == 1 && fabsf(diff.x) == 1 && fabsf(diff.z) == 1;
    }

    AABB expanded(float amount) {
        return AABB(lower.sub(amount), upper.add(amount));
    }

    AABB expandedXZ(float amount) {
        return AABB(lower.sub(amount, 0.f, amount), upper.add(amount, 0.f, amount));
    }

    Vec3<float> centerPoint() {
        Vec3<float> diff = upper.sub(lower);
        return lower.add(diff.mul(0.5f));
    }

    bool intersects(AABB aabb) {
        return aabb.upper.x > lower.x && upper.x > aabb.lower.x &&
            aabb.upper.y > lower.y && upper.y > aabb.lower.y &&
            aabb.upper.z > lower.z && upper.z > aabb.lower.z;
    }

    bool intersectsXZ(AABB aabb) {
        return aabb.upper.x > lower.x && upper.x > aabb.lower.x &&
            aabb.upper.z > lower.z && upper.z > aabb.lower.z;
    }
};


class Utils
{
public:
    static std::string getRoamingPath();

    static std::string GetKeyAsString(int key, bool isCapital = false, bool isKeybind = true);

    static int GetStringAsKey(const std::string &str);

    static std::string removeColorCodes(const std::string& input);

    static std::vector<int> GetStringAsKeys(const std::string str);

    static std::string removeNonAlphanumeric(const std::string& input);

    static std::string remomveNonNumeric(const std::string &input);
};

#pragma once

struct CaretMeasureData
{
    int Position;
    bool isSingleline;// false|insert

    CaretMeasureData()
    {
        CaretMeasureData(0xFFFFFFFF, true);
    };

    CaretMeasureData(int position, bool singlelines)
    {
        this->Position = position;
        this->isSingleline = singlelines;
    };
};