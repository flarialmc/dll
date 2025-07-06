#pragma once

#include <ranges>
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <stdexcept>

struct MCCColor {
	union {
		struct {
			float r, g, b, a;
		};
		float arr[4]{};
	};
	// bool shouldDelete = true;

	MCCColor() {
		this->r = 1.0f;
		this->g = 1.0f;
		this->b = 1.0f;
		this->a = 1.0f;
	};

	MCCColor(const MCCColor& other) {
		this->r = other.r;
		this->g = other.g;
		this->b = other.b;
		this->a = other.a;
		// this->shouldDelete = other.shouldDelete;
	}

	explicit MCCColor(const float* arr) {
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
		// this->shouldDelete = shouldDelete;
	};

	[[nodiscard]] MCCColor lerp(const MCCColor& o, float t) const;

	[[nodiscard]] MCCColor changeBrightness(float b) const;
};

template<typename T>
class Vec2 {
public:
	T x = 0, y = 0;
public:
	explicit Vec2(T x = 0, T y = 0) {
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

	auto mul(float v) -> Vec2<T> {
		return Vec2<T>(this->x * v, this->y * v);
	};

	auto dist(const Vec2<T>& v) -> float {
		auto dX = this->x - v.x;
		auto dY = this->y - v.y;

		return sqrt(dX * dX + dY * dY);
	};

	auto ToInt() {
		return Vec2<int>(x, y);
	}

	auto operator+(const Vec2<T> Vec) {
		return Vec2(this->x + Vec.x, this->y + Vec.y);
	}

	auto operator==(const Vec2<T> Vec) {
		return this->x == Vec.x && this->y == Vec.y;
	}

	auto operator-(const Vec2<T> Vec) {
		return Vec2(this->x - Vec.x, this->y - Vec.y);
	}
};

template<typename T>
class Vec3 : public Vec2<T> {
public:
	T z = 0;
public:
	explicit Vec3(T x = 0, T y = 0, T z = 0) : Vec2<T>(x, y) {
		this->z = z;
	};

	auto ToFloat() {
		return Vec3<float>(this->x, this->y, z);
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
		return Vec3<T>(this->x - x, this->y - y, this->z - z);
	};

	auto sub(const Vec3<T>& vec) const -> Vec3<T> {
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

	auto lerp(const Vec3<T>& vec, T t) -> Vec3<T> {
		return Vec3<T>(std::lerp(this->x, vec.x, t), std::lerp(this->y, vec.y, t), std::lerp(this->z, vec.z, t));
	};

	auto mul(T v) -> Vec3<T> {
		return Vec3<T>(this->x * v, this->y * v, this->z * v);
	};
public:
	auto dist(const Vec3<T> pos) const -> float {
		return sqrt((std::pow(this->x - pos.x, 2)) + (std::pow(this->y - pos.y, 2)) + (std::pow(this->z - pos.z, 2)));
	};
};

template<typename T>
class Vec4 : public Vec3<T> {
public:
	T w = 0;
public:
	explicit Vec4(T x = 0, T y = 0, T z = 0, T w = 0) : Vec3<T>(x, y, z) {
		this->w = w;
	};
};

struct AABB {
	Vec3<float> lower;
	Vec3<float> upper;

	AABB() = default;

	AABB(Vec3<float> l, Vec3<float> h) : lower(l), upper(h) {};

	AABB(const AABB& aabb) {
		lower = Vec3<float>(aabb.lower);
		upper = Vec3<float>(aabb.upper);
	}

	AABB(Vec3<float> lower, float width, float height, float eyeHeight) {
		lower = lower.sub(Vec3<float>(width, eyeHeight * 2, width).div(2));
		this->lower = lower;
		this->upper = Vec3<float>{ lower.x + width, lower.y + height, lower.z + width };
	}

	bool isFullBlock() {
		auto diff = lower.sub(upper);
		return fabsf(diff.y) == 1 && fabsf(diff.x) == 1 && fabsf(diff.z) == 1;
	}

	AABB expanded(float amount) {
		return { lower.sub(amount), upper.add(amount) };
	}

	AABB expandedXZ(float amount) {
		return { lower.sub(amount, 0.f, amount), upper.add(amount, 0.f, amount) };
	}

	AABB expandedXYZ(const float amount) {
		return { lower.sub(amount, 0.f, amount), upper.add(amount, amount, amount) };
	}

	Vec3<float> centerPoint() {
		Vec3<float> diff = upper.sub(lower);
		return lower.add(diff.mul(0.5f));
	}

	bool intersects(const AABB& aabb) {
		return aabb.upper.x > lower.x && upper.x > aabb.lower.x &&
			aabb.upper.y > lower.y && upper.y > aabb.lower.y &&
			aabb.upper.z > lower.z && upper.z > aabb.lower.z;
	}

	bool intersectsXZ(const AABB& aabb) {
		return aabb.upper.x > lower.x && upper.x > aabb.lower.x &&
			aabb.upper.z > lower.z && upper.z > aabb.lower.z;
	}
};

class Utils {
public:
	static std::string getRoamingPath();
	static std::string getClientPath();
	static std::string getConfigsPath();

	static void MessageDialogW(PCWSTR pText, PCWSTR pTitle);

	static std::string getAssetsPath();
	static std::string getLogsPath();

	static std::string getKeyAsString(int key, bool isCapital = false, bool isKeybind = true);
	static std::string getMouseAsString(int key);

	static int getStringAsKey(const std::string& str);

	static std::vector<int> getStringAsKeys(const std::string& str);

	static bool CursorInEllipse(float ellipseX, float ellipseY, float radiusX, float radiusY);

	static int CountBytes(const std::string& data);

	static constexpr unsigned int hash(const char* str) {
		unsigned int hash = 5381;
		int c;

		while ((c = *str++))
			hash = ((hash << 5) + hash) + c; // hash * 33 + c

		return hash;
	}

	static std::vector<std::string> splitString(const std::string& str, char delimiter) {
		return str
			| std::views::split(delimiter)
			| std::views::transform([](auto&& token) { return std::string{ token.begin(), token.end() }; })
			| std::ranges::to<std::vector>();
	}

	static uint64_t getCurrentMs();

	static std::string downloadFile(const std::string& url);

	static std::string sanitizeName(const std::string& name);

	static void extractFromFile(const std::string& zipFilePath, const std::string& destinationFolder);
};

class String {
	static inline std::map<std::string, std::string> cacheRemoveColor;
	static inline std::map<std::string, std::string> cacheRemoveAlnum;
	static inline std::mutex cacheMutex;
public:
	static std::string replaceAll(std::string& string, std::string_view c1, std::string_view c2);

	static bool find(const std::string& string, const std::string& find);

	static bool hasEnding(const std::string& string, const std::string& ending);

	static std::wstring StrToWStr(std::string const& s);

	static std::string WStrToStr(const std::wstring& ws);

	static std::string removeColorCodes(const std::string& string);

	static std::string removeNonAlphanumeric(const std::string& string);

	static std::string removeNonNumeric(const std::string& string, bool integer = false);

	static std::vector<std::string> split(std::string_view str, char delimiter);

	static std::string toLower(std::string input);

	static std::string toUpper(std::string input);
};

struct CaretMeasureData {
	int Position{};
	bool isSingleline{};// false|insert

	CaretMeasureData() {
		CaretMeasureData(0xFFFFFFFF, true);
	};

	CaretMeasureData(int position, bool singlelines) {
		this->Position = position;
		this->isSingleline = singlelines;
	};
};