#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <libplatform/libplatform.h>
#define V8_COMPRESS_POINTERS 1
#define V8_31BIT_SMIS_ON_64BIT_ARCH 1
#define V8_ENABLE_SANDBOX 1
#ifdef DEBUG
    #define V8_ENABLE_CHECKS 1
#endif
#include <v8.h>

#include "../../../debug/Logger.hpp"
#include "hello.hpp"

namespace UmiJS {
    typedef uint32_t u32;
    typedef u32 InstanceID;
    using std::atomic, std::vector, std::memory_order_relaxed, std::unique_ptr, std::string, boost::unordered::unordered_flat_map;
    
    inline constexpr InstanceID INVALID_INSTANCE_ID = 0;

    struct JsInstance {
        v8::Global<v8::Context> context;
        v8::Global<v8::Script> script;
    };

    inline v8::Isolate::CreateParams createParams;
    inline v8::Isolate* isolate{nullptr};
    inline unique_ptr<v8::Platform> platform{v8::platform::NewDefaultPlatform()};
    inline unordered_flat_map<InstanceID, v8::Global<v8::Context>> contextRegistry;
    inline unordered_flat_map<InstanceID, v8::Global<v8::UnboundScript>> scriptRegistry;
    inline atomic<InstanceID> nextId{1};

    inline void init() noexcept {
        v8::V8::InitializePlatform(platform.get());
        v8::V8::Initialize();
        createParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        isolate = v8::Isolate::New(createParams);
        {
            v8::Isolate::Scope isolate_scope(isolate);
            v8::HandleScope handle_scope(isolate);
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            v8::Context::Scope context_scope(context);
            v8::MaybeLocal<v8::String> source = v8::String::NewFromUtf8(isolate, JS_TEST_HELLO_CHERRYGROVE, v8::NewStringType::kNormal);
            v8::Local<v8::String> local;
            if (source.ToLocal(&local)) {
                v8::Local<v8::Script> script = v8::Script::Compile(context, local).ToLocalChecked();
                v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
                v8::String::Utf8Value utf8(isolate, result);
                lout << *utf8 << endl;
            }
            else lerr << "[V8] JSFuck test failed!" << endl;
        }
    }

    inline void shutdown() noexcept {
        isolate->Dispose();
        delete createParams.array_buffer_allocator;
        v8::V8::Dispose();
        v8::V8::DisposePlatform();
    }

    inline InstanceID createInstance() noexcept {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = v8::Context::New(isolate);
        InstanceID id = nextId.fetch_add(1, memory_order_relaxed);
        contextRegistry.emplace(id, v8::Global<v8::Context>(isolate, context));
        return id;
    }
}