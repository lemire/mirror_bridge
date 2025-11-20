// sol2 Lua binding for runtime benchmarks
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../shared/benchmark_class.hpp"

extern "C" int luaopen_bench_sol2(lua_State* L) {
    sol::state_view lua(L);

    sol::table module = lua.create_table();

    module.new_usertype<BenchmarkClass>("BenchmarkClass",
        sol::constructors<BenchmarkClass()>(),

        // Methods
        "null_call", &BenchmarkClass::null_call,
        "add_int", &BenchmarkClass::add_int,
        "multiply_double", &BenchmarkClass::multiply_double,
        "concat_string", &BenchmarkClass::concat_string,
        "get_string", &BenchmarkClass::get_string,
        "set_string", &BenchmarkClass::set_string,
        "add_to_vector", &BenchmarkClass::add_to_vector,
        "get_vector", &BenchmarkClass::get_vector,
        "set_vector", sol::overload(
            [](BenchmarkClass& obj, const sol::table& tbl) {
                std::vector<double> vec;
                for (size_t i = 1; i <= tbl.size(); ++i) {
                    vec.push_back(tbl[i]);
                }
                obj.set_vector(vec);
            }
        ),
        "get_counter", &BenchmarkClass::get_counter,
        "set_counter", &BenchmarkClass::set_counter,

        // Properties
        "counter", &BenchmarkClass::counter,
        "value", &BenchmarkClass::value,
        "name", &BenchmarkClass::name,
        "data", &BenchmarkClass::data
    );

    sol::stack::push(L, module);
    return 1;
}
