// node-addon-api binding for runtime benchmarks (modern C++ wrapper for N-API)
#include <napi.h>
#include "../shared/benchmark_class.hpp"

class BenchmarkClassWrapper : public Napi::ObjectWrap<BenchmarkClassWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    BenchmarkClassWrapper(const Napi::CallbackInfo& info);

private:
    BenchmarkClass obj;

    // Methods
    Napi::Value NullCall(const Napi::CallbackInfo& info);
    Napi::Value AddInt(const Napi::CallbackInfo& info);
    Napi::Value MultiplyDouble(const Napi::CallbackInfo& info);
    Napi::Value ConcatString(const Napi::CallbackInfo& info);
    Napi::Value GetString(const Napi::CallbackInfo& info);
    void SetString(const Napi::CallbackInfo& info);
    void AddToVector(const Napi::CallbackInfo& info);
    Napi::Value GetVector(const Napi::CallbackInfo& info);
    void SetVector(const Napi::CallbackInfo& info);
    Napi::Value GetCounter(const Napi::CallbackInfo& info);
    void SetCounter(const Napi::CallbackInfo& info);

    // Property accessors
    Napi::Value GetCounterProp(const Napi::CallbackInfo& info);
    void SetCounterProp(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetValueProp(const Napi::CallbackInfo& info);
    void SetValueProp(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetNameProp(const Napi::CallbackInfo& info);
    void SetNameProp(const Napi::CallbackInfo& info, const Napi::Value& value);
    Napi::Value GetDataProp(const Napi::CallbackInfo& info);
    void SetDataProp(const Napi::CallbackInfo& info, const Napi::Value& value);
};

BenchmarkClassWrapper::BenchmarkClassWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<BenchmarkClassWrapper>(info) {
}

Napi::Object BenchmarkClassWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "BenchmarkClass", {
        InstanceMethod("null_call", &BenchmarkClassWrapper::NullCall),
        InstanceMethod("add_int", &BenchmarkClassWrapper::AddInt),
        InstanceMethod("multiply_double", &BenchmarkClassWrapper::MultiplyDouble),
        InstanceMethod("concat_string", &BenchmarkClassWrapper::ConcatString),
        InstanceMethod("get_string", &BenchmarkClassWrapper::GetString),
        InstanceMethod("set_string", &BenchmarkClassWrapper::SetString),
        InstanceMethod("add_to_vector", &BenchmarkClassWrapper::AddToVector),
        InstanceMethod("get_vector", &BenchmarkClassWrapper::GetVector),
        InstanceMethod("set_vector", &BenchmarkClassWrapper::SetVector),
        InstanceMethod("get_counter", &BenchmarkClassWrapper::GetCounter),
        InstanceMethod("set_counter", &BenchmarkClassWrapper::SetCounter),

        InstanceAccessor("counter", &BenchmarkClassWrapper::GetCounterProp, &BenchmarkClassWrapper::SetCounterProp),
        InstanceAccessor("value", &BenchmarkClassWrapper::GetValueProp, &BenchmarkClassWrapper::SetValueProp),
        InstanceAccessor("name", &BenchmarkClassWrapper::GetNameProp, &BenchmarkClassWrapper::SetNameProp),
        InstanceAccessor("data", &BenchmarkClassWrapper::GetDataProp, &BenchmarkClassWrapper::SetDataProp),
    });

    exports.Set("BenchmarkClass", func);
    return exports;
}

Napi::Value BenchmarkClassWrapper::NullCall(const Napi::CallbackInfo& info) {
    obj.null_call();
    return info.Env().Undefined();
}

Napi::Value BenchmarkClassWrapper::AddInt(const Napi::CallbackInfo& info) {
    int x = info[0].As<Napi::Number>().Int32Value();
    return Napi::Number::New(info.Env(), obj.add_int(x));
}

Napi::Value BenchmarkClassWrapper::MultiplyDouble(const Napi::CallbackInfo& info) {
    double x = info[0].As<Napi::Number>().DoubleValue();
    return Napi::Number::New(info.Env(), obj.multiply_double(x));
}

Napi::Value BenchmarkClassWrapper::ConcatString(const Napi::CallbackInfo& info) {
    std::string s = info[0].As<Napi::String>().Utf8Value();
    return Napi::String::New(info.Env(), obj.concat_string(s));
}

Napi::Value BenchmarkClassWrapper::GetString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), obj.get_string());
}

void BenchmarkClassWrapper::SetString(const Napi::CallbackInfo& info) {
    std::string s = info[0].As<Napi::String>().Utf8Value();
    obj.set_string(s);
}

void BenchmarkClassWrapper::AddToVector(const Napi::CallbackInfo& info) {
    double x = info[0].As<Napi::Number>().DoubleValue();
    obj.add_to_vector(x);
}

Napi::Value BenchmarkClassWrapper::GetVector(const Napi::CallbackInfo& info) {
    auto vec = obj.get_vector();
    Napi::Array arr = Napi::Array::New(info.Env(), vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        arr[i] = Napi::Number::New(info.Env(), vec[i]);
    }
    return arr;
}

void BenchmarkClassWrapper::SetVector(const Napi::CallbackInfo& info) {
    Napi::Array arr = info[0].As<Napi::Array>();
    std::vector<double> vec;
    for (uint32_t i = 0; i < arr.Length(); i++) {
        vec.push_back(arr.Get(i).As<Napi::Number>().DoubleValue());
    }
    obj.set_vector(vec);
}

Napi::Value BenchmarkClassWrapper::GetCounter(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), obj.get_counter());
}

void BenchmarkClassWrapper::SetCounter(const Napi::CallbackInfo& info) {
    int c = info[0].As<Napi::Number>().Int32Value();
    obj.set_counter(c);
}

Napi::Value BenchmarkClassWrapper::GetCounterProp(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), obj.counter);
}

void BenchmarkClassWrapper::SetCounterProp(const Napi::CallbackInfo& info, const Napi::Value& value) {
    obj.counter = value.As<Napi::Number>().Int32Value();
}

Napi::Value BenchmarkClassWrapper::GetValueProp(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), obj.value);
}

void BenchmarkClassWrapper::SetValueProp(const Napi::CallbackInfo& info, const Napi::Value& value) {
    obj.value = value.As<Napi::Number>().DoubleValue();
}

Napi::Value BenchmarkClassWrapper::GetNameProp(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), obj.name);
}

void BenchmarkClassWrapper::SetNameProp(const Napi::CallbackInfo& info, const Napi::Value& value) {
    obj.name = value.As<Napi::String>().Utf8Value();
}

Napi::Value BenchmarkClassWrapper::GetDataProp(const Napi::CallbackInfo& info) {
    Napi::Array arr = Napi::Array::New(info.Env(), obj.data.size());
    for (size_t i = 0; i < obj.data.size(); i++) {
        arr[i] = Napi::Number::New(info.Env(), obj.data[i]);
    }
    return arr;
}

void BenchmarkClassWrapper::SetDataProp(const Napi::CallbackInfo& info, const Napi::Value& value) {
    Napi::Array arr = value.As<Napi::Array>();
    obj.data.clear();
    for (uint32_t i = 0; i < arr.Length(); i++) {
        obj.data.push_back(arr.Get(i).As<Napi::Number>().DoubleValue());
    }
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return BenchmarkClassWrapper::Init(env, exports);
}

NODE_API_MODULE(bench_node_addon_api, Init)
