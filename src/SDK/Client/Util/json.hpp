
#pragma once

#include <map>
#include <string>
#include <string_view>
#include <json/json.hpp>

namespace MinecraftJson
{
    enum class ValueType : int32_t {
        Null = 0x0,
        Int = 0x1,
        Uint = 0x2,
        Real = 0x3,
        String = 0x4,
        Boolean = 0x5,
        Array = 0x6,
        Object = 0x7,
    };

    using ArrayIndex = unsigned int;

    class Value { // equivalent to bds' Json::Value, but we dont wanna confuse it with nlohmann json
    public:
        class CZString {
        public:
            enum class DuplicationPolicy : unsigned int {
                noDuplication = 0,
                duplicate = 1,
                duplicateOnCopy = 2,
                Mask = 3,
            };
            struct StringStorage {
                DuplicationPolicy policy_ : 2;
                unsigned int              length_ : 30; // 1GB max
            };

            char const* cstr_;
            union {
                ArrayIndex    index_{};
                StringStorage storage_;
            };

            CZString(ArrayIndex index) : index_(index) {}

            CZString(char const* str, unsigned int length, DuplicationPolicy allocate) : cstr_(str) {
                storage_.policy_ = allocate;
                storage_.length_ = length & 0x3FFFFFFF;
            }
            constexpr CZString& operator=(CZString const& other) {
                if (this == &other) {
                    return *this;
                }
                cstr_ = other.cstr_;
                index_ = other.index_;
                return *this;
            }

            [[nodiscard]] constexpr std::strong_ordering operator<=>(CZString const& other) const {
                if (!cstr_) return index_ <=> other.index_;
                unsigned int this_len = this->storage_.length_;
                unsigned int other_len = other.storage_.length_;
                unsigned int min_len = std::min<unsigned int>(this_len, other_len);
                int  comp = memcmp(this->cstr_, other.cstr_, min_len);

                if (comp == 0) return this_len <=> other_len;
                return comp <=> 0;
            }

            [[nodiscard]] constexpr ArrayIndex  index() const { return index_; }
            [[nodiscard]] constexpr char const* c_str() const { return cstr_; }
            [[nodiscard]] constexpr bool        isStaticString() const {
                return storage_.policy_ == DuplicationPolicy::noDuplication;
            }
        };


        /*
        Null    = 0x0,
        Int     = 0x1,
        Uint    = 0x2,
        Real    = 0x3,
        String  = 0x4,
        Boolean = 0x5,
        Array   = 0x6,
        Object  = 0x7,*/

        union ValueHolder { // tozic i dont think u understand how unions work
            int64_t                    mInt;       // this+0x0
            uint64_t                   mUInt64;    // this+0x0
            long double                mReal;      // this+0x0
            bool                       mBool;      // this+0x0
            char* mString;   // this+0x0
            std::map<CZString, Value>* mMap;      // this+0x0
        };

        ValueHolder mValue;                       // this+0x0000

        ValueType   mType : 8;                    // this+0x0028
        int32_t     mAllocated : 1;               // this+0x002C
    public:
        [[nodiscard]] std::string toString()
        {
            std::string result;
            switch (mType)
            {
            case ValueType::Null:
                result = "[nulltype]";
                break;
            case ValueType::Int:
                result = std::to_string(mValue.mInt);
                break;
            case ValueType::Uint:
                result = std::to_string(mValue.mUInt64);
                break;
            case ValueType::Real:
                result = std::to_string(mValue.mReal);
                break;
            case ValueType::String:
                result = std::string("\"") + mValue.mString + std::string("\"");
                break;
            case ValueType::Boolean:
                result = mValue.mBool ? "true" : "false";
                break;
            case ValueType::Array:
                result = "[size: ";
                result += std::to_string(mValue.mMap->size());
                result += "]";
                break;
            case ValueType::Object:
                result = "[object]";
                break;
            default:
                result = "what?";
                break;
            }
            return result;
        }

        std::string toStyledString(std::string* output) {
            static uintptr_t func = Memory::offsetFromSig(GET_SIG_ADDRESS("Json::Value::ToStyledString"), 1);
            Memory::CallFunc<void, MinecraftJson::Value*, std::string*>((void*)func, this, output);
            return *output;
        }

        nlohmann::json toNlohmannJson() {
            /*std::string styledString;
            this->toStyledString(&styledString);
            return nlohmann::json::parse(styledString);*/
        }

        static inline MinecraftJson::Value fromNlohmannJson(nlohmann::json const& json)
        {

        }
        Value(std::string const& value) {
            memset(this, 0, sizeof(*this));
            static uintptr_t func = GET_SIG_ADDRESS("Json::Reader::Reader");
            void* Reader = nullptr;
            Memory::CallFunc<void, void*>((void*)func, Reader);

            static uintptr_t func2 = GET_SIG_ADDRESS("Json::Reader::Parse");
            Memory::CallFunc<void, void*, std::string, MinecraftJson::Value*, bool>((void*)func2, Reader, value, this, false);
        }

        Value() {
            memset(this, 0, sizeof(*this));
        }
    };

    class Reader
    {
    public:
        Reader()
        {
            static uintptr_t func = GET_SIG_ADDRESS("Json::Reader::Reader");
            Memory::CallFunc<void, void*>((void*)func, this);
        }
        Value Parse(std::string const& json)
        {
            static uintptr_t func2 = GET_SIG_ADDRESS("Json::Reader::Parse");
            Value result;
            Memory::CallFunc<void, void*, std::string, MinecraftJson::Value*, bool>((void*)func2, this, json, &result, false);
            return result;
        }
    };

    static_assert(sizeof(Value) == 0x10, "Value size is invalid");
};