#pragma once
#include <string>
#include <cstdlib>
#include <cmath>

class Utils
{
public:
    static std::string getRoamingPath();
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
public:
    auto dist(const Vec2<T>& v) -> float {
        auto dX = this->x - v.x;
        auto dY = this->y - v.y;

        return sqrt(dX * dX + dY * dY);
    };
};

template<typename T>
class Vec3 : public Vec2<T> {
public:
    T z = 0;
public:
    Vec3(T x = 0, T y = 0, T z = 0) : Vec2<T>(x, y) {
        this->z = z;
    };
public:
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
    auto dist(const Vec3<T> v) -> float {
        auto dX = this->x - v.x;
        auto dY = this->y - v.y;
        auto dZ = this->z - v.z;

        return sqrt(dX * dX + dY * dY + dZ * dZ);
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