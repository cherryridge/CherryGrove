#pragma once
#include <bit>
#include <compare>
#include <ostream>
#include <glaze/glaze.hpp>

#include "../../debug/Logger.hpp"
#include "../../util/implHashFor.hpp"
#include "../../util/json/helpers.hpp"
#include "biid.hpp"

namespace InputHandler::BoolInput {
    typedef uint64_t u64;
    using std::popcount, std::countr_zero, std::partial_ordering, std::ostream;

    JSON_STRUCT KeyCombo {
        u64 lower{0}, middle{0}, higher{0}, align__{0};

        [[nodiscard]] explicit KeyCombo() noexcept = default;
        [[nodiscard]] KeyCombo(u64 lower, u64 middle, u64 higher) noexcept : lower(lower), middle(middle), higher(higher) {}
        [[nodiscard]] KeyCombo(BIInputSource source, u32 rawCode) noexcept { static_cast<void>(addKey(getBIID(source, rawCode))); }

        [[nodiscard]] KeyCombo(const KeyCombo&) noexcept = default;
        [[nodiscard]] KeyCombo(KeyCombo&&) noexcept = default;
        KeyCombo& operator=(const KeyCombo&) noexcept = default;
        KeyCombo& operator=(KeyCombo&&) noexcept = default;

        [[nodiscard]] u8 keyCount() const noexcept { return static_cast<u8>(popcount(lower) + popcount(middle) + popcount(higher)); }

        [[nodiscard]] BoolInputID firstKey() const noexcept {
            if (lower != 0) [[likely]] return static_cast<BoolInputID>(countr_zero(lower));
            else if (middle != 0) [[unlikely]] return static_cast<BoolInputID>(64 + countr_zero(middle));
            else if (higher != 0) [[unlikely]] return static_cast<BoolInputID>(128 + countr_zero(higher));
            return INVALID_BIID;
        }

        constexpr static const char* OUT_OF_BOUND_ERROR = "[InputHandler] Attempt to get/add/remove key with out-of-bound BIID: ";
        #define ASSERT_BIID_IN_BOUND(biid)              \
        if (biid >= BIID_COUNT) {                       \
            lerr << OUT_OF_BOUND_ERROR << biid << endl; \
            return false;                               \
        }

        [[nodiscard]] bool hasKey(BoolInputID biid) const noexcept {
            #if CG_DEBUG
                ASSERT_BIID_IN_BOUND(biid)
            #endif
            if (biid < 64) return (lower & (1ull << biid)) != 0;
            else if (biid < 128) return (middle & (1ull << (biid - 64))) != 0;
            else if (biid < 192) return (higher & (1ull << (biid - 128))) != 0;
            else return false;
        }

        //note: The following four functions are supposed to check for `INVALID_BIID` and always return false in that case, but since `INVALID_BIID` is `0xffff`, they will just go to the `else` branch and return false, which is the expected behavior. So we don't need an explicit check for `INVALID_BIID`.

        //This function just adds the key and returns whether the key already exists.
        //This function is not [[nodiscard]] to silence excessive warnings.
        bool addKey(BoolInputID biid) noexcept {
            #if CG_DEBUG
                ASSERT_BIID_IN_BOUND(biid)
            #endif
            const bool result = hasKey(biid);
            if (biid < 64) lower |= (1ull << biid);
            else if (biid < 128) middle |= (1ull << (biid - 64));
            else if (biid < 192) higher |= (1ull << (biid - 128));
            else [[unlikely]] return false;
            return result;
        }
        //This function returns whether a success addition is made.
        [[nodiscard]] bool addKey_strict(BoolInputID biid) noexcept {
            #if CG_DEBUG
                ASSERT_BIID_IN_BOUND(biid)
            #endif
            if (hasKey(biid)) return false;
            if (biid < 64) lower |= (1ull << biid);
            else if (biid < 128) middle |= (1ull << (biid - 64));
            else if (biid < 192) higher |= (1ull << (biid - 128));
            else [[unlikely]] return false;
            return true;
        }

        //This function just removes the key and returns whether the key has existed.
        //This function is not [[nodiscard]] to silence excessive warnings.
        bool removeKey(BoolInputID biid) noexcept {
            #if CG_DEBUG
                ASSERT_BIID_IN_BOUND(biid)
            #endif
            const bool result = hasKey(biid);
            if (biid < 64) lower &= ~(1ull << biid);
            else if (biid < 128) middle &= ~(1ull << (biid - 64));
            else if (biid < 192) higher &= ~(1ull << (biid - 128));
            else [[unlikely]] return false;
            return result;
        }
        //This function returns whether a success removal is made.
        [[nodiscard]] bool removeKey_strict(BoolInputID biid) noexcept {
            #if CG_DEBUG
                ASSERT_BIID_IN_BOUND(biid)
            #endif
            if (!hasKey(biid)) return false;
            if (biid < 64) lower &= ~(1ull << biid);
            else if (biid < 128) middle &= ~(1ull << (biid - 64));
            else if (biid < 192) higher &= ~(1ull << (biid - 128));
            else [[unlikely]] return false;
            return true;
        }

        void reset() noexcept { memset(this, 0, sizeof(KeyCombo)); }

        [[nodiscard]] auto operator|(const KeyCombo& other) const noexcept {
            return KeyCombo{
                lower | other.lower,
                middle | other.middle,
                higher | other.higher,
            };
        }
        [[nodiscard]] auto operator&(const KeyCombo& other) const noexcept {
            return KeyCombo{
                lower & other.lower,
                middle & other.middle,
                higher & other.higher,
            };
        }
        [[nodiscard]] auto operator^(const KeyCombo& other) const noexcept {
            return KeyCombo{
                lower ^ other.lower,
                middle ^ other.middle,
                higher ^ other.higher,
            };
        }

        //Fuck operator~. We don't need it.
        //Fuck every numerical operator also.

        auto& operator|=(const KeyCombo& other) noexcept {
            lower |= other.lower;
            middle |= other.middle;
            higher |= other.higher;
            return *this;
        }
        auto& operator&=(const KeyCombo& other) noexcept {
            lower &= other.lower;
            middle &= other.middle;
            higher &= other.higher;
            return *this;
        }
        auto& operator^=(const KeyCombo& other) noexcept {
            lower ^= other.lower;
            middle ^= other.middle;
            higher ^= other.higher;
            return *this;
        }

        [[nodiscard]] bool operator==(const KeyCombo& other) const noexcept { return lower == other.lower && middle == other.middle && higher == other.higher; }
        //[[nodiscard]] bool operator!=(const KeyCombo& other) const noexcept { return !operator==(other); }

        [[nodiscard]] partial_ordering operator<=>(const KeyCombo& other) const noexcept {
            const KeyCombo aAndB = *this & other;
            if (aAndB == *this && aAndB == other) return partial_ordering::equivalent;
            else if (aAndB == *this) return partial_ordering::less;
            else if (aAndB == other) return partial_ordering::greater;
            else return partial_ordering::unordered;
        }

        friend ostream& operator<<(ostream& os, const KeyCombo& data) noexcept {
            os << "KeyCombo(" << data.lower << ", " << data.middle << ", " << data.higher << ")";
            return os;
        }
        friend Logger::Logger& operator<<(Logger::Logger& logger, const KeyCombo& data) noexcept {
            logger << "KeyCombo(" << data.lower << ", " << data.middle << ", " << data.higher << ")";
            return logger;
        }
    };
}

IMPL_HASH_FOR(InputHandler::BoolInput, KeyCombo, 0,
    seed = (input.lower ^ 3'25'20'9'12'21ull) ^ std::rotl(input.middle, 21) ^ std::rotl(input.higher, 38);
)

namespace glz {
    using InputHandler::BoolInput::KeyCombo;

    GLAZE_DYNAMIC_FROM_START(KeyCombo)
        //todo: Codex, do not implement this!
    GLAZE_DYNAMIC_FROM_END
}

template <>
struct glz::meta<InputHandler::BoolInput::KeyCombo> {
    using T = InputHandler::BoolInput::KeyCombo;
    static constexpr auto value = glz::array(&T::lower, &T::middle, &T::higher);
};