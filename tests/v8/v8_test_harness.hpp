// V8 Test Harness for Mirror Bridge
// ============================================================================
//
// This header provides a minimal V8 test harness for testing Mirror Bridge
// V8 bindings. It handles V8 initialization, script execution, and cleanup.
//
// USAGE:
//   Include this header, define the module init function, and call run_v8_test:
//
//   #include "v8_test_harness.hpp"
//   #include "binding.cpp"  // Defines the module init function
//
//   int main(int argc, char* argv[]) {
//       return run_v8_test(argc, argv, "test.js", point2d_init);
//   }
//
// ============================================================================

#pragma once

#include <libplatform/libplatform.h>
#include <v8.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>

// ============================================================================
// Helper: Read file contents
// ============================================================================

inline std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ============================================================================
// Helper: Print function for JavaScript
// ============================================================================

inline void v8_print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    for (int i = 0; i < args.Length(); i++) {
        if (i > 0) std::cout << " ";
        v8::String::Utf8Value str(isolate, args[i]);
        std::cout << (*str ? *str : "(null)");
    }
    std::cout << std::endl;
}

// ============================================================================
// Module init function type
// ============================================================================

using ModuleInitFunc = void (*)(v8::Isolate*, v8::Local<v8::Object>);

// ============================================================================
// Run V8 Test
// ============================================================================

inline int run_v8_test(
    int argc,
    char* argv[],
    const char* test_script_path,
    ModuleInitFunc module_init)
{
    // Initialize V8
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    // Create isolate
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);

    int exit_code = 0;

    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        // Create global template with print function
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        global->Set(
            v8::String::NewFromUtf8(isolate, "print").ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, v8_print)
        );

        // Create context
        v8::Local<v8::Context> context = v8::Context::New(isolate, nullptr, global);
        v8::Context::Scope context_scope(context);

        // Create exports object and initialize module
        v8::Local<v8::Object> exports = v8::Object::New(isolate);
        module_init(isolate, exports);

        // Expose bound classes directly in global scope
        // This allows test scripts to use `new ClassName()` directly
        v8::Local<v8::Array> property_names = exports->GetOwnPropertyNames(context).ToLocalChecked();
        for (uint32_t i = 0; i < property_names->Length(); i++) {
            v8::Local<v8::Value> key = property_names->Get(context, i).ToLocalChecked();
            v8::Local<v8::Value> value = exports->Get(context, key).ToLocalChecked();
            context->Global()->Set(context, key, value).Check();
        }

        // Read and execute test script
        std::string script_source = read_file(test_script_path);
        if (script_source.empty()) {
            std::cerr << "Error: Empty or missing test script" << std::endl;
            exit_code = 1;
        } else {
            v8::Local<v8::String> source = v8::String::NewFromUtf8(
                isolate, script_source.c_str()
            ).ToLocalChecked();

            v8::TryCatch try_catch(isolate);
            v8::Local<v8::Script> script;

            if (!v8::Script::Compile(context, source).ToLocal(&script)) {
                v8::String::Utf8Value error(isolate, try_catch.Exception());
                std::cerr << "Compile error: " << *error << std::endl;
                exit_code = 1;
            } else {
                v8::Local<v8::Value> result;
                if (!script->Run(context).ToLocal(&result)) {
                    v8::String::Utf8Value error(isolate, try_catch.Exception());
                    std::cerr << "Runtime error: " << *error << std::endl;
                    exit_code = 1;
                } else {
                    // Check if script returned false (test failure)
                    if (result->IsFalse()) {
                        exit_code = 1;
                    }
                }
            }
        }
    }

    // Cleanup
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::DisposePlatform();
    delete create_params.array_buffer_allocator;

    return exit_code;
}
