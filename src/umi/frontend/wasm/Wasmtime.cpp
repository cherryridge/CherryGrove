#include <atomic>
#include <fstream>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <wasmtime.hh>

#include "../../../debug/Logger.hpp"
#include "Wasmtime.hpp"

namespace UmiWasmtime {
    typedef uint8_t u8;
    using std::atomic, std::get, std::ifstream, std::istreambuf_iterator, std::vector, boost::unordered::unordered_flat_map, std::memory_order_relaxed;

    static wasmtime::Engine engine;
    static unordered_flat_map<ModuleID, wasmtime::Module> registry;
    static atomic<ModuleID> nextId{1};

    void init() noexcept {
        auto res = wasmtime::Module::compile(engine, WASMTIME_TEST);
        if (!res) {
            lerr << "[Wasmtime] Compilation error occured!" << endl;
            return;
        }
        wasmtime::Store store(engine);
        wasmtime::Module module = res.unwrap();
        auto instance_res = wasmtime::Instance::create(store, module, {});
        if (!instance_res) {
            lerr << "[Wasmtime] Instantiation error occured!" << endl;
            return;
        }
        wasmtime::Instance instance = instance_res.unwrap();
        wasmtime::Func add = get<wasmtime::Func>(*instance.get(store, "add"));
        auto result_res = add.call(store, {35, 7});
        if (!result_res) {
            lerr << "[Wasmtime] Function call error occured!" << endl;
            return;
        }
        vector<wasmtime::Val> results = result_res.unwrap();
        if (results.size() == 1 && results[0].kind() == wasmtime::ValKind::I32) lout << "[Wasmtime] 35 + 7 = " << results[0].i32() << endl;
        else lerr << "[Wasmtime] Function call returned wrong result!" << endl;
    }

    void shutdown() noexcept {}

    ModuleID addModule(const char* path) noexcept {
        ifstream file(path, std::ios::in | std::ios::binary);
        if (!file.is_open()) return INVALID_MODULE_HANDLE;
        vector<u8> buffer(istreambuf_iterator<char>(file), {});
        if (buffer.empty()) return INVALID_MODULE_HANDLE;
        auto res = wasmtime::Module::compile(engine, wasmtime::Span<u8>(buffer.data(), buffer.size()));
        if (res) {
            ModuleID id = nextId.fetch_add(1, memory_order_relaxed);
            registry.emplace(id, res.unwrap());
            return id;
        }
        return INVALID_MODULE_HANDLE;
    }
}