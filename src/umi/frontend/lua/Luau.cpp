#include <atomic>
#include <cstdint>
#include <boost/unordered/unordered_flat_map.hpp>
#include <luacode.h>
#include <lualib.h>

#include "Luau.hpp"

namespace UmiLuau {
    typedef uint32_t u32;
    using std::atomic, std::move, boost::unordered::unordered_flat_map;

    static unordered_flat_map<u32, lua_State*> contextRegistry;
    static atomic<InstanceID> nextId{1};

    void init() noexcept {}
    void shutdown() noexcept {
        for (const auto& [id, state] : contextRegistry) {
            lua_gc(state, LUA_GCCOLLECT, 0);
            lua_close(state);
        }
    }

    InstanceID createInstance() noexcept {
        lua_State* temp = luaL_newstate();
        luaL_openlibs(temp);
        //todo: add UMI bindings
        InstanceID id = nextId.fetch_add(1, std::memory_order_relaxed);
        contextRegistry.emplace(id, move(temp));
        return id;
    }

    bool destroyInstance(InstanceID id) noexcept {
        if (contextRegistry.find(id) == contextRegistry.end()) return false;
        contextRegistry.erase(id);
        return true;
    }
}