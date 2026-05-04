#pragma once
#include <array>

#include "concepts.hpp"

namespace Util {
    using std::array, Util::UInt;
    
    template <typename T, size_t size, size_t sizeR> requires UInt<T>
    struct BijectorArray {
    private:
        using Arr = array<T, size>;
        using ArrR = array<T, sizeR>;
        Arr forward;
        ArrR reverse;

    public:
        //Don't mark explicit because we want syntax sugar of `BijectorArray arr = {{ ... }};`.
        [[nodiscard]] constexpr BijectorArray(const Arr& input, T invalid = 0, bool isDestinationInstead = false) : forward(input) {
            static_assert(sizeof(T) <= sizeof(size_t), "Type T too large to be used as array index.");
            T maxv = 0;
            for (T i = 0; i < size; i++) if (input[i] > maxv) maxv = input[i];
            if (maxv + 1 > sizeR) throw "BijectorArray: sizeR too small for the provided input array.";
            forward.fill(invalid);
            reverse.fill(invalid);
            if (isDestinationInstead) for (T i = 0; i < size; i++) {
                reverse[i] = input[i];
                forward[input[i]] = i;
            }
            else for (T i = 0; i < size; i++) {
                forward[i] = input[i];
                reverse[input[i]] = i;
            }
        }
        [[nodiscard]] constexpr BijectorArray(const BijectorArray&) noexcept = default;
        [[nodiscard]] constexpr BijectorArray(BijectorArray&&) noexcept = default;
        constexpr BijectorArray& operator=(const BijectorArray&) noexcept = default;
        constexpr BijectorArray& operator=(BijectorArray&&) noexcept = default;

        [[nodiscard]] constexpr T to(T input) const noexcept { return forward[input]; }
        [[nodiscard]] constexpr T from(T input) const noexcept { return reverse[input]; }
    };
}