#pragma once
#include <Windows.h>
#include <MinHook.h>
#include <algorithm>
#include <format>
#include <optional>
#include <assert.h>
#include <vector>

template <typename F> auto IIFE(F f) { return f(); }

template <class T> struct remove_cvref { typedef std::remove_cv_t<std::remove_reference_t<T>> type; };

template <class T> using remove_cvref_t = typename remove_cvref<T>::type;

template <typename Ret, typename Type> Ret& direct_access(Type* type, size_t offset) {
	union {
		size_t raw;
		Type* source;
		Ret* target;
	} u;
	u.source = type;
	u.raw += offset;
	return *u.target;
}

#define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name
#define DEF_FIELD_RW(type, name) __declspec(property(get = get##name, put = set##name)) type name

#define FAKE_FIELD(type, name)                                                                                       \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name()

#define BUILD_ACCESS(ptr, type, name, offset)                                                                        \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name() const { return direct_access<type>(ptr, offset); }												     \
void set##name(type v) const { direct_access<type>(ptr, offset) = v; }

#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))

class Mem {
	static bool MHInitialized;
public:
	static HMODULE getDll();
public:
	static uintptr_t findSig(const char* szSignature);
	static uintptr_t findMultiLvlPtr(uintptr_t baseAddr, std::vector<unsigned int> offsets, bool basePure = false);
	static void hookFunc(void* pTarget, void* pDetour, void** ppOriginal, std::string name);

	template <unsigned int index>
	static void HookVFunc(uintptr_t sigOffset, void* pDetour, void** ppOriginal, std::string name) {
		uintptr_t** vTable = reinterpret_cast<uintptr_t**>(sigOffset + 3 + 7);

		hookFunc(vTable[index], pDetour, ppOriginal, name);
	}

	template < typename ret>
	static inline ret FuncFromSigOffset(uintptr_t sig, int offset) {
		
		return reinterpret_cast<ret>(sig + offset + 4 + *reinterpret_cast<int*>(sig + offset));
	}

	template<class T>
	static void SafeRelease(T** ppT) {
		if (*ppT) {
			(*ppT)->Release();
			*ppT = NULL;
		}
	}
};