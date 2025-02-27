#include "ScriptLib.hpp"

#include <Utils/Utils.hpp>

// Flarial Scripting uses pure Lua C API
// so we need a way to store C++ functions
// and allow them to be called from Lua
struct LambdaWrapper {
    std::function<int(lua_State*)> fn;
    explicit LambdaWrapper(std::function<int(lua_State*)> f) : fn(std::move(f)) {}
};

struct ManagedClosure {
    std::shared_ptr<LambdaWrapper> wrapper;

    explicit ManagedClosure(std::shared_ptr<LambdaWrapper> w) : wrapper(std::move(w)) {}
    ~ManagedClosure() = default;
};

int closure(lua_State* L) {
    auto* closure = static_cast<ManagedClosure*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!closure) {
        luaL_error(L, "Missing ManagedClosure upvalue (how)");
        return 0;
    }

    return closure->wrapper->fn(L);
}

static int managedClosure(lua_State* L) {
    auto* closure = static_cast<ManagedClosure *>(lua_touserdata(L, 1));
    if (closure) {
        closure->~ManagedClosure();
    }
    return 0;
}

static void ensureManagedClosureMeta(lua_State* L) {
    if (luaL_newmetatable(L, "ManagedClosureMeta")) {
        lua_pushcfunction(L, managedClosure);
        lua_setfield(L, -2, "__gc");
    }
    lua_pop(L, 1);
}

void ScriptLib::registerFunction(lua_State* L, const std::function<int(lua_State*)> &callback, const char* functionName,const char* tablePath) {
    createNestedTable(L, tablePath);
    ensureManagedClosureMeta(L);

    auto wrapper = std::make_shared<LambdaWrapper>(LambdaWrapper{ callback });
    void* ud = lua_newuserdata(L, sizeof(ManagedClosure));
    new (ud) ManagedClosure{ std::move(wrapper) };

    luaL_getmetatable(L, "ManagedClosureMeta");
    lua_setmetatable(L, -2);

    lua_pushcclosure(L, &closure, 1);
    lua_setfield(L, -2, functionName);

    lua_pop(L, 1);
}

void ScriptLib::registerFunction(lua_State* L, const std::function<int(lua_State*)> &callback, const char* functionName) {
    ensureManagedClosureMeta(L);

    auto wrapper = std::make_shared<LambdaWrapper>(callback);
    void* ud = lua_newuserdata(L, sizeof(ManagedClosure));

    new (ud) ManagedClosure{std::move(wrapper)};

    luaL_getmetatable(L, "ManagedClosureMeta");
    lua_setmetatable(L, -2);

    lua_pushcclosure(L, &closure, 1);
    lua_setglobal(L, functionName);
}

void ScriptLib::createNestedTable(lua_State* L, const std::string& path) {
    if (path.empty()) {
        lua_pushglobaltable(L);
        return;
    }

    const auto tokens = String::split(path, '.');

    lua_pushglobaltable(L);

    for (auto& segment : tokens) {
        lua_getfield(L, -1, segment.c_str());
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setfield(L, -2, segment.c_str());
            lua_getfield(L, -1, segment.c_str());
        }
        lua_remove(L, -2);
    }
}