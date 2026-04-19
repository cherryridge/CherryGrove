#pragma once
#include <type_traits>
#include <utility>

#include "concepts.hpp"

namespace Util {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    using std::to_underlying, std::conditional_t;

    template <typename EnumType, EnumType count>
    struct BitField {
    private:
        template <EnumType count_>
        using Size = conditional_t<
            to_underlying(count_) <= (sizeof(u8) << 3), u8,
            conditional_t<to_underlying(count_) <= (sizeof(u16) << 3), u16,
                conditional_t<to_underlying(count_) <= (sizeof(u32) << 3), u32,
                    conditional_t<to_underlying(count_) <= (sizeof(u64) << 3), u64,
                        void
                    >
                >
            >
        >;

        Size<count> bits{0};

    public:
        explicit BitField() noexcept = default;

        template <typename... Flags> requires (sizeof...(Flags) > 0) && (Equal<Flags, EnumType> && ...)
        [[nodiscard]] explicit BitField(Flags&&... flags) noexcept {
            (set(flags), ...);
        }

        [[nodiscard]] bool get(EnumType flag) const noexcept { return (bits & (static_cast<Size<count>>(1) << to_underlying(flag))) != 0; }

        void set(EnumType flag) noexcept { bits |= (static_cast<Size<count>>(1) << to_underlying(flag)); }

        void reset(EnumType flag) noexcept { bits &= ~(static_cast<Size<count>>(1) << to_underlying(flag)); }

        void toggle(EnumType flag) noexcept {
            if (get(flag)) reset(flag);
            else set(flag);
        }

        [[nodiscard]] bool any() const noexcept { return bits != 0; }
        [[nodiscard]] bool none() const noexcept { return bits == 0; }
        [[nodiscard]] bool all() const noexcept { return bits == (static_cast<Size<count>>(1) << to_underlying(count)) - 1; }

        [[nodiscard]] Size<count> raw() const noexcept { return bits; }
        void clear() noexcept { bits = 0; }
    };
}