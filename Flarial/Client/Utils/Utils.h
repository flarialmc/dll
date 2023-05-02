#pragma once
#include "RenderUtils.h"
#include <type_traits>
#include <filesystem>
#include <functional>
#include <MinHook.h>
#include <Windows.h>
#include <typeindex>
#include <fstream>
#include <ostream>
#include <bitset>
#include <string>
#include <vector>
#include <regex>
#include <map>

enum MouseAction { None = 0, Left = 1, Right = 2, Middle = 3, Scroll = 4 };

class Utils {
public:
	static std::string getRoamPath();
public:
	static bool isValidDomain(std::string);
public:
	static void reachOff(float*, float, float);
public:
	static std::string sanitize(std::string text);
public:
	static std::string toLower(std::string input);
public:
	static auto splitStringBy(std::string, std::string) -> std::vector<std::string>;
public:
	template <unsigned int IIdx, typename TRet, typename... TArgs>
	static inline auto CallVFunc(void* thisptr, TArgs... argList) -> TRet {
		using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
	};
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
		return Vec3<T>(std::floorf(this->x), std::floorf(this->y), std::floorf(this->z));
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

template<typename T>
class AABB {
public:
	Vec3<float> lower;
	Vec3<float> upper;

	AABB(Vec3<float> vecA = Vec3(0.f, 0.f, 0.f), Vec3<float> vecB = Vec3(0.f, 0.f, 0.f)) {
		this->lower = vecA;
		this->upper = vecB;
	};
	bool intersects(AABB aabb) {
		return aabb.upper.x > lower.x && upper.x > aabb.lower.x &&
			aabb.upper.y > lower.y && upper.y > aabb.lower.y &&
			aabb.upper.z > lower.z && upper.z > aabb.lower.z; //we need this for ca
	}
};

/* View Matrix Stuff */

struct glmatrixf {
	union {
		float v[16];
		float v_nested[4][4];
	};

	__forceinline float operator[](int i) const { return v[i]; }
	__forceinline float& operator[](int i) { return v[i]; }

#define MULMAT(row, col) v[col + row] = x[row] * y[col] + x[row + 4] * y[col + 1] + x[row + 8] * y[col + 2] + x[row + 12] * y[col + 3];

	template <class XT, class YT>
	void mul(const XT x[16], const YT y[16]) {
		MULMAT(0, 0);
		MULMAT(1, 0);
		MULMAT(2, 0);
		MULMAT(3, 0);
		MULMAT(0, 4);
		MULMAT(1, 4);
		MULMAT(2, 4);
		MULMAT(3, 4);
		MULMAT(0, 8);
		MULMAT(1, 8);
		MULMAT(2, 8);
		MULMAT(3, 8);
		MULMAT(0, 12);
		MULMAT(1, 12);
		MULMAT(2, 12);
		MULMAT(3, 12);
	}

#undef MULMAT

	glmatrixf* correct() {
		glmatrixf* newMatPtr = new glmatrixf;

		for (int i = 0; i < 4; i++) {
			newMatPtr->v[i * 4 + 0] = v[0 + i];
			newMatPtr->v[i * 4 + 1] = v[4 + i];
			newMatPtr->v[i * 4 + 2] = v[8 + i];
			newMatPtr->v[i * 4 + 3] = v[12 + i];
		}
		return newMatPtr;
	};

	inline bool OWorldToScreen(Vec3<float> origin, Vec3<float> pos, Vec2<float>& screen, Vec2<float> fov, Vec2<float> displaySize) {
		pos = pos.sub(origin);

		float x = transformx(pos);
		float y = transformy(pos);
		float z = transformz(pos);

		if (z > 0)
			return false;

		float mX = (float)displaySize.x / 2.0F;
		float mY = (float)displaySize.y / 2.0F;

		screen.x = mX + (mX * x / -z * fov.x);
		screen.y = mY - (mY * y / -z * fov.y);

		return true;
	}

	inline void mul(const glmatrixf& x, const glmatrixf& y) {
		mul(x.v, y.v);
	}

	inline void translate(float x, float y, float z) {
		v[12] += x;
		v[13] += y;
		v[14] += z;
	}

	inline void translate(const Vec3<float>& o) {
		translate(o.x, o.y, o.z);
	}

	inline void scale(float x, float y, float z) {
		v[0] *= x;
		v[1] *= x;
		v[2] *= x;
		v[3] *= x;
		v[4] *= y;
		v[5] *= y;
		v[6] *= y;
		v[7] *= y;
		v[8] *= z;
		v[9] *= z;
		v[10] *= z;
		v[11] *= z;
	}

	inline void invertnormal(Vec3<float>& dir) const {
		Vec3 n(dir);
		dir.x = n.x * v[0] + n.y * v[1] + n.z * v[2];
		dir.y = n.x * v[4] + n.y * v[5] + n.z * v[6];
		dir.z = n.x * v[8] + n.y * v[9] + n.z * v[10];
	}

	inline void invertvertex(Vec3<float>& pos) const {
		Vec3 p(pos);
		p.x -= v[12];
		p.y -= v[13];
		p.z -= v[14];
		pos.x = p.x * v[0] + p.y * v[1] + p.z * v[2];
		pos.y = p.x * v[4] + p.y * v[5] + p.z * v[6];
		pos.z = p.x * v[8] + p.y * v[9] + p.z * v[10];
	}

	inline void transform(const Vec3<float>& in, Vec4<float>& out) const {
		out.x = transformx(in);
		out.y = transformy(in);
		out.z = transformz(in);
		out.w = transformw(in);
	}

	__forceinline float transformx(const Vec3<float>& p) const {
		return p.x * v[0] + p.y * v[4] + p.z * v[8] + v[12];
	}

	__forceinline float transformy(const Vec3<float>& p) const {
		return p.x * v[1] + p.y * v[5] + p.z * v[9] + v[13];
	}

	__forceinline float transformz(const Vec3<float>& p) const {
		return p.x * v[2] + p.y * v[6] + p.z * v[10] + v[14];
	}

	__forceinline float transformw(const Vec3<float>& p) const {
		return p.x * v[3] + p.y * v[7] + p.z * v[11] + v[15];
	}

	__forceinline Vec3<float> gettranslation() const {
		return Vec3(v[12], v[13], v[14]);
	}

	//assault cube world2screen
	Vec3<float> transform(glmatrixf* matrix, Vec3<float>& totransform) {
		return Vec3<float>(matrix->transformx(totransform),
						   matrix->transformy(totransform),
						   matrix->transformz(totransform))
			.div(matrix->transformw(totransform));
	};

	///pos should be the exact center of the enemy model for scaling to work properly
	Vec3<float> WorldToScreen(Vec3<float> pos, int width, int height) {
		//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
		Vec4<float> clipCoords;
		clipCoords.x = pos.x * v[0] + pos.y * v[4] + pos.z * v[8] + v[12];
		clipCoords.y = pos.x * v[1] + pos.y * v[5] + pos.z * v[9] + v[13];
		clipCoords.z = pos.x * v[2] + pos.y * v[6] + pos.z * v[10] + v[14];
		clipCoords.w = pos.x * v[3] + pos.y * v[7] + pos.z * v[11] + v[15];

		//perspective division, dividing by clip.W = Normalized Device Coordinates
		Vec3<float> NDC;
		NDC.x = clipCoords.x / clipCoords.w;
		NDC.y = clipCoords.y / clipCoords.w;
		NDC.z = clipCoords.z / clipCoords.w;

		//viewport tranform to screenCooords

		Vec3<float> playerscreen;
		playerscreen.x = (width / 2 * NDC.x) + (NDC.x + width / 2);
		playerscreen.y = -(height / 2 * NDC.y) + (NDC.y + height / 2);

		return playerscreen;
	};
};