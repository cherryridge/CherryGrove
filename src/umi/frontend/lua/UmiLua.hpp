#pragma once
#include <atomic>
#include <boost/unordered/unordered_flat_map.hpp>
#include <luacode.h>
#include <lualib.h>

namespace UmiLua {
    typedef uint32_t u32;
    typedef u32 InstanceID;
    inline constexpr InstanceID INVALID_INSTANCE_ID = 0;
    using std::atomic, std::move, boost::unordered::unordered_flat_map;

    inline unordered_flat_map<u32, lua_State*> contextRegistry;
    inline atomic<InstanceID> nextId{1};

    inline void init() noexcept {}
    inline void shutdown() noexcept {
        for (const auto& [id, state] : contextRegistry) {
            lua_gc(state, LUA_GCCOLLECT, 0);
            lua_close(state);
        }
    }

    inline InstanceID createInstance() noexcept {
        lua_State* temp = luaL_newstate();
        luaL_openlibs(temp);
        InstanceID id = nextId.fetch_add(1, std::memory_order_relaxed);
        contextRegistry.emplace(id, move(temp));
        return id;
    }

    inline bool destroyInstance(InstanceID id) noexcept {
        if (contextRegistry.find(id) == contextRegistry.end()) return false;
        contextRegistry.erase(id);
        return true;
    }
}