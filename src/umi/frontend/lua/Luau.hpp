#pragma once
#include <cstdint>

namespace UmiLuau {
    typedef uint32_t u32;
    typedef u32 InstanceID;
    inline constexpr InstanceID INVALID_INSTANCE_ID = 0;

    void init() noexcept;
    void shutdown() noexcept;

    InstanceID createInstance() noexcept;
    bool destroyInstance(InstanceID id) noexcept;
}