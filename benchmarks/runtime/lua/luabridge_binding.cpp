// LuaBridge3 Lua binding for runtime benchmarks
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>
#include "../shared/benchmark_class.hpp"

extern "C" int luaopen_bench_luabridge(lua_State* L) {
    luabridge::getGlobalNamespace(L)
        .beginClass<BenchmarkClass>("BenchmarkClass")
            .addConstructor<void(*)()>()

            // Methods
            .addFunction("null_call", &BenchmarkClass::null_call)
            .addFunction("add_int", &BenchmarkClass::add_int)
            .addFunction("multiply_double", &BenchmarkClass::multiply_double)
            .addFunction("concat_string", &BenchmarkClass::concat_string)
            .addFunction("get_string", &BenchmarkClass::get_string)
            .addFunction("set_string", &BenchmarkClass::set_string)
            .addFunction("add_to_vector", &BenchmarkClass::add_to_vector)
            .addFunction("get_vector", &BenchmarkClass::get_vector)
            .addFunction("set_vector", &BenchmarkClass::set_vector)
            .addFunction("get_counter", &BenchmarkClass::get_counter)
            .addFunction("set_counter", &BenchmarkClass::set_counter)

            // Properties
            .addProperty("counter", &BenchmarkClass::counter)
            .addProperty("value", &BenchmarkClass::value)
            .addProperty("name", &BenchmarkClass::name)
            .addProperty("data", &BenchmarkClass::data)
        .endClass();

    // Return module table
    lua_newtable(L);
    luabridge::push(L, luabridge::LuaRef::getGlobal(L, "BenchmarkClass"));
    lua_setfield(L, -2, "BenchmarkClass");

    return 1;
}
