// Plain N-API binding for runtime benchmarks (baseline)
#include <node_api.h>
#include "../shared/benchmark_class.hpp"
#include <cstring>

// Wrapper for BenchmarkClass
struct BenchmarkClassWrapper {
    BenchmarkClass* obj;
    napi_env env;
    napi_ref wrapper_ref;
};

// Destructor callback
static void finalize_callback(napi_env env, void* finalize_data, void* finalize_hint) {
    BenchmarkClassWrapper* wrapper = static_cast<BenchmarkClassWrapper*>(finalize_data);
    if (wrapper->obj) {
        delete wrapper->obj;
    }
    delete wrapper;
}

// Unwrap helper
static BenchmarkClassWrapper* unwrap(napi_env env, napi_callback_info info) {
    napi_value this_arg;
    napi_get_cb_info(env, info, nullptr, nullptr, &this_arg, nullptr);

    BenchmarkClassWrapper* wrapper;
    napi_unwrap(env, this_arg, reinterpret_cast<void**>(&wrapper));
    return wrapper;
}

// Constructor
static napi_value constructor(napi_env env, napi_callback_info info) {
    napi_value this_arg;
    napi_get_cb_info(env, info, nullptr, nullptr, &this_arg, nullptr);

    BenchmarkClassWrapper* wrapper = new BenchmarkClassWrapper();
    wrapper->obj = new BenchmarkClass();
    wrapper->env = env;

    napi_wrap(env, this_arg, wrapper, finalize_callback, nullptr, &wrapper->wrapper_ref);
    return this_arg;
}

// Methods
static napi_value null_call(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);
    wrapper->obj->null_call();

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value add_int(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    int32_t x;
    napi_get_value_int32(env, args[0], &x);

    int result = wrapper->obj->add_int(x);

    napi_value result_val;
    napi_create_int32(env, result, &result_val);
    return result_val;
}

static napi_value multiply_double(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    double x;
    napi_get_value_double(env, args[0], &x);

    double result = wrapper->obj->multiply_double(x);

    napi_value result_val;
    napi_create_double(env, result, &result_val);
    return result_val;
}

static napi_value concat_string(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    size_t str_len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &str_len);
    char* str_buf = new char[str_len + 1];
    napi_get_value_string_utf8(env, args[0], str_buf, str_len + 1, &str_len);

    std::string result = wrapper->obj->concat_string(str_buf);
    delete[] str_buf;

    napi_value result_val;
    napi_create_string_utf8(env, result.c_str(), result.length(), &result_val);
    return result_val;
}

static napi_value get_string(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);
    std::string result = wrapper->obj->get_string();

    napi_value result_val;
    napi_create_string_utf8(env, result.c_str(), result.length(), &result_val);
    return result_val;
}

static napi_value set_string(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    size_t str_len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &str_len);
    char* str_buf = new char[str_len + 1];
    napi_get_value_string_utf8(env, args[0], str_buf, str_len + 1, &str_len);

    wrapper->obj->set_string(str_buf);
    delete[] str_buf;

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value add_to_vector(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    double x;
    napi_get_value_double(env, args[0], &x);

    wrapper->obj->add_to_vector(x);

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value get_vector(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);
    std::vector<double> vec = wrapper->obj->get_vector();

    napi_value result;
    napi_create_array_with_length(env, vec.size(), &result);

    for (size_t i = 0; i < vec.size(); i++) {
        napi_value elem;
        napi_create_double(env, vec[i], &elem);
        napi_set_element(env, result, i, elem);
    }

    return result;
}

static napi_value set_vector(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    uint32_t len;
    napi_get_array_length(env, args[0], &len);

    std::vector<double> vec;
    for (uint32_t i = 0; i < len; i++) {
        napi_value elem;
        napi_get_element(env, args[0], i, &elem);
        double val;
        napi_get_value_double(env, elem, &val);
        vec.push_back(val);
    }

    wrapper->obj->set_vector(vec);

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value get_counter(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);
    int result = wrapper->obj->get_counter();

    napi_value result_val;
    napi_create_int32(env, result, &result_val);
    return result_val;
}

static napi_value set_counter(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    int32_t c;
    napi_get_value_int32(env, args[0], &c);

    wrapper->obj->set_counter(c);

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

// Property getters
static napi_value get_counter_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    napi_value result;
    napi_create_int32(env, wrapper->obj->counter, &result);
    return result;
}

static napi_value get_value_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    napi_value result;
    napi_create_double(env, wrapper->obj->value, &result);
    return result;
}

static napi_value get_name_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    napi_value result;
    napi_create_string_utf8(env, wrapper->obj->name.c_str(), wrapper->obj->name.length(), &result);
    return result;
}

static napi_value get_data_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    napi_value result;
    napi_create_array_with_length(env, wrapper->obj->data.size(), &result);

    for (size_t i = 0; i < wrapper->obj->data.size(); i++) {
        napi_value elem;
        napi_create_double(env, wrapper->obj->data[i], &elem);
        napi_set_element(env, result, i, elem);
    }

    return result;
}

// Property setters
static napi_value set_counter_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    int32_t val;
    napi_get_value_int32(env, args[0], &val);
    wrapper->obj->counter = val;

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value set_value_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    double val;
    napi_get_value_double(env, args[0], &val);
    wrapper->obj->value = val;

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value set_name_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    size_t str_len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &str_len);
    char* str_buf = new char[str_len + 1];
    napi_get_value_string_utf8(env, args[0], str_buf, str_len + 1, &str_len);

    wrapper->obj->name = str_buf;
    delete[] str_buf;

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value set_data_prop(napi_env env, napi_callback_info info) {
    BenchmarkClassWrapper* wrapper = unwrap(env, info);

    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    uint32_t len;
    napi_get_array_length(env, args[0], &len);

    wrapper->obj->data.clear();
    for (uint32_t i = 0; i < len; i++) {
        napi_value elem;
        napi_get_element(env, args[0], i, &elem);
        double val;
        napi_get_value_double(env, elem, &val);
        wrapper->obj->data.push_back(val);
    }

    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

// Module initialization
static napi_value init(napi_env env, napi_value exports) {
    // Define class
    napi_value cons;

    napi_property_descriptor properties[] = {
        // Methods (using snake_case to match Mirror Bridge)
        {"null_call", nullptr, null_call, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"add_int", nullptr, add_int, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"multiply_double", nullptr, multiply_double, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"concat_string", nullptr, concat_string, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"get_string", nullptr, get_string, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"set_string", nullptr, set_string, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"add_to_vector", nullptr, add_to_vector, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"get_vector", nullptr, get_vector, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"set_vector", nullptr, set_vector, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"get_counter", nullptr, get_counter, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"set_counter", nullptr, set_counter, nullptr, nullptr, nullptr, napi_default, nullptr},

        // Properties
        {"counter", nullptr, nullptr, get_counter_prop, set_counter_prop, nullptr, napi_default, nullptr},
        {"value", nullptr, nullptr, get_value_prop, set_value_prop, nullptr, napi_default, nullptr},
        {"name", nullptr, nullptr, get_name_prop, set_name_prop, nullptr, napi_default, nullptr},
        {"data", nullptr, nullptr, get_data_prop, set_data_prop, nullptr, napi_default, nullptr},
    };

    napi_define_class(env, "BenchmarkClass", NAPI_AUTO_LENGTH, constructor, nullptr,
                     sizeof(properties) / sizeof(properties[0]), properties, &cons);

    napi_set_named_property(env, exports, "BenchmarkClass", cons);
    return exports;
}

NAPI_MODULE(bench_plain_js, init)
