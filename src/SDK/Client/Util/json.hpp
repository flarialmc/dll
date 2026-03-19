
#pragma once

#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <json/json.hpp>

namespace MinecraftJson
{
    // Matches BDS Json::ValueType (enum class : schar)
    enum class ValueType : int8_t {
        Null = 0,
        Int = 1,
        Uint = 2,
        Real = 3,
        String = 4,
        Boolean = 5,
        Array = 6,
        Object = 7,
    };

    class Value {
    public:
        // Game's CZString is just a char pointer (8 bytes)
        class CZString {
        public:
            char* cstr_{nullptr};

            CZString() = default;
            explicit CZString(char const* str) : cstr_(const_cast<char*>(str)) {}

            bool operator<(const CZString& other) const {
                if (cstr_) return strcmp(cstr_, other.cstr_) < 0;
                return false;
            }

            [[nodiscard]] char const* c_str() const { return cstr_; }
        };

        // Game uses a custom comparator with transparent lookup
        struct CZStringCompare {
            using is_transparent = void;
            bool operator()(const CZString& l, const CZString& r) const { return l < r; }
        };

        using ObjectValues = std::map<CZString, Value, CZStringCompare>;
        using ArrayValues = std::vector<Value*>;

        union ValueHolder {
            int64_t       mInt;       // this+0x0
            uint64_t      mUInt64;    // this+0x0
            double        mReal;      // this+0x0
            bool          mBool;      // this+0x0
            CZString*     mString;    // this+0x0 — String values point to a CZString object
            ObjectValues* mMap;       // this+0x0 — Object values
            ArrayValues*  mArray;     // this+0x0 — Array values (vector of Value pointers)
        };

        ValueHolder mValue;           // this+0x00, 8 bytes
        ValueType   mType;            // this+0x08, 1 byte (enum class : int8_t)
        // 7 bytes implicit padding to align struct to 8

    public:
        [[nodiscard]] std::string toString() const
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
                if (mValue.mString && mValue.mString->cstr_)
                    result = std::string("\"") + mValue.mString->cstr_ + std::string("\"");
                else
                    result = "\"\"";
                break;
            case ValueType::Boolean:
                result = mValue.mBool ? "true" : "false";
                break;
            case ValueType::Array:
                result = "[size: ";
                result += mValue.mArray ? std::to_string(mValue.mArray->size()) : "0";
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

        nlohmann::json toNlohmannJson() const {
            switch (mType) {
                case ValueType::Null:    return nullptr;
                case ValueType::Int:     return mValue.mInt;
                case ValueType::Uint:    return mValue.mUInt64;
                case ValueType::Real:    return mValue.mReal;
                case ValueType::Boolean: return mValue.mBool;
                case ValueType::String:
                    if (mValue.mString && mValue.mString->cstr_)
                        return std::string(mValue.mString->cstr_);
                    return std::string();
                case ValueType::Array: {
                    auto arr = nlohmann::json::array();
                    if (mValue.mArray) {
                        for (const auto* elem : *mValue.mArray)
                            arr.push_back(elem->toNlohmannJson());
                    }
                    return arr;
                }
                case ValueType::Object: {
                    auto obj = nlohmann::json::object();
                    if (mValue.mMap) {
                        for (const auto& [k, v] : *mValue.mMap) {
                            if (k.c_str())
                                obj[std::string(k.c_str())] = v.toNlohmannJson();
                        }
                    }
                    return obj;
                }
                default: return nullptr;
            }
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
        char padding[256]{}; // game's Reader is >0x28 bytes; pad generously
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

    static_assert(sizeof(Value::CZString) == 8, "CZString must be 8 bytes (just a char pointer)");
    static_assert(sizeof(Value) == 0x10, "Value size is invalid");
};
