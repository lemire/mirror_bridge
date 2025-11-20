// Plain Lua C API binding for runtime benchmarks (baseline)
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include "../shared/benchmark_class.hpp"
#include <cstring>

// Wrapper struct
struct BenchmarkClassWrapper {
    BenchmarkClass* obj;
    bool owns;
};

// Get wrapper from Lua stack
static BenchmarkClassWrapper* check_wrapper(lua_State* L, int idx) {
    return static_cast<BenchmarkClassWrapper*>(luaL_checkudata(L, idx, "BenchmarkClass"));
}

// Constructor
static int benchmark_new(lua_State* L) {
    BenchmarkClassWrapper* wrapper = static_cast<BenchmarkClassWrapper*>(
        lua_newuserdata(L, sizeof(BenchmarkClassWrapper)));
    wrapper->obj = new BenchmarkClass();
    wrapper->owns = true;

    luaL_getmetatable(L, "BenchmarkClass");
    lua_setmetatable(L, -2);
    return 1;
}

// Garbage collection
static int benchmark_gc(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    if (wrapper->owns && wrapper->obj) {
        delete wrapper->obj;
    }
    return 0;
}

// Methods
static int benchmark_null_call(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    wrapper->obj->null_call();
    return 0;
}

static int benchmark_add_int(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    int x = luaL_checkinteger(L, 2);
    int result = wrapper->obj->add_int(x);
    lua_pushinteger(L, result);
    return 1;
}

static int benchmark_multiply_double(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    double x = luaL_checknumber(L, 2);
    double result = wrapper->obj->multiply_double(x);
    lua_pushnumber(L, result);
    return 1;
}

static int benchmark_concat_string(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    const char* s = luaL_checkstring(L, 2);
    std::string result = wrapper->obj->concat_string(s);
    lua_pushstring(L, result.c_str());
    return 1;
}

static int benchmark_get_string(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    std::string result = wrapper->obj->get_string();
    lua_pushstring(L, result.c_str());
    return 1;
}

static int benchmark_set_string(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    const char* s = luaL_checkstring(L, 2);
    wrapper->obj->set_string(s);
    return 0;
}

static int benchmark_add_to_vector(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    double x = luaL_checknumber(L, 2);
    wrapper->obj->add_to_vector(x);
    return 0;
}

static int benchmark_get_vector(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    std::vector<double> vec = wrapper->obj->get_vector();
    lua_createtable(L, vec.size(), 0);
    for (size_t i = 0; i < vec.size(); i++) {
        lua_pushnumber(L, vec[i]);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int benchmark_set_vector(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    std::vector<double> vec;
    int len = lua_rawlen(L, 2);
    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, 2, i);
        vec.push_back(lua_tonumber(L, -1));
        lua_pop(L, 1);
    }
    wrapper->obj->set_vector(vec);
    return 0;
}

static int benchmark_get_counter(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    lua_pushinteger(L, wrapper->obj->get_counter());
    return 1;
}

static int benchmark_set_counter(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    int c = luaL_checkinteger(L, 2);
    wrapper->obj->set_counter(c);
    return 0;
}

// Property accessors via __index and __newindex
static int benchmark_index(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    const char* key = lua_tostring(L, 2);

    if (strcmp(key, "counter") == 0) {
        lua_pushinteger(L, wrapper->obj->counter);
        return 1;
    } else if (strcmp(key, "value") == 0) {
        lua_pushnumber(L, wrapper->obj->value);
        return 1;
    } else if (strcmp(key, "name") == 0) {
        lua_pushstring(L, wrapper->obj->name.c_str());
        return 1;
    } else if (strcmp(key, "data") == 0) {
        lua_createtable(L, wrapper->obj->data.size(), 0);
        for (size_t i = 0; i < wrapper->obj->data.size(); i++) {
            lua_pushnumber(L, wrapper->obj->data[i]);
            lua_rawseti(L, -2, i + 1);
        }
        return 1;
    }

    // Check methods
    if (strcmp(key, "null_call") == 0) {
        lua_pushcfunction(L, benchmark_null_call);
        return 1;
    } else if (strcmp(key, "add_int") == 0) {
        lua_pushcfunction(L, benchmark_add_int);
        return 1;
    } else if (strcmp(key, "multiply_double") == 0) {
        lua_pushcfunction(L, benchmark_multiply_double);
        return 1;
    } else if (strcmp(key, "concat_string") == 0) {
        lua_pushcfunction(L, benchmark_concat_string);
        return 1;
    } else if (strcmp(key, "get_string") == 0) {
        lua_pushcfunction(L, benchmark_get_string);
        return 1;
    } else if (strcmp(key, "set_string") == 0) {
        lua_pushcfunction(L, benchmark_set_string);
        return 1;
    } else if (strcmp(key, "add_to_vector") == 0) {
        lua_pushcfunction(L, benchmark_add_to_vector);
        return 1;
    } else if (strcmp(key, "get_vector") == 0) {
        lua_pushcfunction(L, benchmark_get_vector);
        return 1;
    } else if (strcmp(key, "set_vector") == 0) {
        lua_pushcfunction(L, benchmark_set_vector);
        return 1;
    } else if (strcmp(key, "get_counter") == 0) {
        lua_pushcfunction(L, benchmark_get_counter);
        return 1;
    } else if (strcmp(key, "set_counter") == 0) {
        lua_pushcfunction(L, benchmark_set_counter);
        return 1;
    }

    return 0;
}

static int benchmark_newindex(lua_State* L) {
    BenchmarkClassWrapper* wrapper = check_wrapper(L, 1);
    const char* key = lua_tostring(L, 2);

    if (strcmp(key, "counter") == 0) {
        wrapper->obj->counter = luaL_checkinteger(L, 3);
    } else if (strcmp(key, "value") == 0) {
        wrapper->obj->value = luaL_checknumber(L, 3);
    } else if (strcmp(key, "name") == 0) {
        wrapper->obj->name = luaL_checkstring(L, 3);
    } else if (strcmp(key, "data") == 0) {
        luaL_checktype(L, 3, LUA_TTABLE);
        wrapper->obj->data.clear();
        int len = lua_rawlen(L, 3);
        for (int i = 1; i <= len; i++) {
            lua_rawgeti(L, 3, i);
            wrapper->obj->data.push_back(lua_tonumber(L, -1));
            lua_pop(L, 1);
        }
    }

    return 0;
}

// Module initialization
extern "C" int luaopen_bench_plain_lua(lua_State* L) {
    // Create metatable
    luaL_newmetatable(L, "BenchmarkClass");

    lua_pushcfunction(L, benchmark_index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, benchmark_newindex);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, benchmark_gc);
    lua_setfield(L, -2, "__gc");

    lua_pop(L, 1);

    // Create module table
    lua_newtable(L);
    lua_pushcfunction(L, benchmark_new);
    lua_setfield(L, -2, "BenchmarkClass");

    return 1;
}
