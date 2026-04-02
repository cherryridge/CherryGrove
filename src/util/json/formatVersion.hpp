#pragma once
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>

#include "../../debug/Logger.hpp"
#include "../functionTraits.hpp"
#include "JSONKind.hpp"
#include "rw.hpp"

inline constexpr uint32_t INVALID_FORMAT_VERSION = 0;

namespace Util::Json {
    typedef uint32_t u32;
    using std::span, std::tuple, std::tuple_size_v, std::make_tuple, std::get, std::move, std::forward, std::is_same_v, std::is_nothrow_move_constructible_v, std::remove_reference_t, std::is_rvalue_reference_v, std::make_index_sequence, std::index_sequence, std::tuple_element_t;

    [[nodiscard]] inline u32 probeFormatVersion(const span<const u8> input) noexcept {
        struct VersionProbe {
            u32 formatVersion{INVALID_FORMAT_VERSION};
        } fv;
        if(!readJSON(fv, input)) return INVALID_FORMAT_VERSION;
        return fv.formatVersion;
    }

    namespace detail {
        template <auto singleUpgrader>
        inline consteval void validateUpgraders() noexcept {
            static_assert(
                ArgumentCount<decltype(singleUpgrader)> == 1,
                "Upgraders must take exactly one argument."
            );
            static_assert(
                is_rvalue_reference_v<ArgumentType<decltype(singleUpgrader), 0>>,
                "The only argument must be an rvalue reference (&&) to enforce move semantics."
            );
            static_assert(
                is_nothrow_move_constructible_v<ReturnType<decltype(singleUpgrader)>>,
                "Upgraders must return a type that is zero-overhead convertable."
            );
        }

        template <auto first, auto second, auto... rest>
        inline consteval void validateUpgraders() noexcept {
            static_assert(
                ArgumentCount<decltype(first)> == 1,
                "Upgraders must take exactly one argument."
            );
            static_assert(
                is_rvalue_reference_v<ArgumentType<decltype(first), 0>>,
                "The only argument must be an rvalue reference (&&) to enforce move semantics."
            );
            static_assert(
                is_nothrow_move_constructible_v<ReturnType<decltype(first)>>,
                "Upgraders must return a type that is zero-overhead convertable."
            );

            static_assert(
                is_same_v<ReturnType<decltype(first)>, remove_reference_t<ArgumentType<decltype(second), 0>>>,
                "Chain broken. Current upgrader's return type does not match next upgrader's argument type."
            );

            if constexpr (sizeof...(rest) > 0) validateUpgraders<second, rest...>();
        }

        template <auto fn>
        struct WrapInFunctor {
            template <typename... Args>
            decltype(auto) operator()(Args&&... args) const noexcept {
                return fn(forward<Args>(args)...);
            }
        };
    }

    template <auto... upgraders>
    inline consteval auto packUpgraders() noexcept {
        if constexpr (sizeof...(upgraders) > 0) detail::validateUpgraders<upgraders...>();
        return make_tuple(detail::WrapInFunctor<upgraders>{}...);
    }

    template <typename... Ts>
    struct TypeList {
        using types = tuple<Ts...>;
    };
    template <typename T>
    concept IsTypeList = requires(T* p) {
        []<typename... Ts>(TypeList<Ts...>*){}(p);
    };

    template <typename List> requires IsTypeList<List>
    using LastType = tuple_element_t<tuple_size_v<typename List::types> - 1, typename List::types>;

    namespace detail {
        template <u32 minFV, u32 currentFV, u32 latestFV, typename List, auto upgraders>
        struct ChainProcessor {
            static bool process(u32 formatVersion, const span<const u8> data, LastType<List>& result) noexcept {
                //This is mandatory for stopping the compiler from instantiating `tuple_element_t<-1, ...>` from the final case's `else` branch, which we cannot prevent because `formatVersion` is only known at runtime.
                //Real control flow will never reach this branch because we will stop at the `formatVersion == currentFV == latestFV` case, but the compiler doesn't know that.
                //To explain further, basically what we're doing here is to "convert" `formatVersion` from a runtime value to a compile-time value (`currentFV`) by guessing one by one until we get it, then we perform the parsing and upgrading. This way the compiler will generate a cascade of `if (formatVersion == X) { ... } else if (formatVersion == Y) { ... } ...` for us automatically and prevent the hell of switches full of `[[fallthrough]]` or manually chained if-else.
                if constexpr (currentFV > latestFV) return false;
                else {
                    if (formatVersion == currentFV) {
                        tuple_element_t<currentFV - minFV, typename List::types> parsedStruct;
                        if (!readJSON(parsedStruct, data)) return false;
                        result = upgradeToLatest<currentFV>(parsedStruct);
                        return true;
                    }
                    else return ChainProcessor<minFV, currentFV + 1, latestFV, List, upgraders>::process(formatVersion, data, result);
                }
            }

        private:
            template <u32 fromFV, typename CurrentStruct>
            static auto upgradeToLatest(CurrentStruct&& currentStruct) noexcept {
                if constexpr (fromFV == latestFV) return forward<CurrentStruct>(currentStruct);
                else {
                    const auto nextStruct = get<fromFV - minFV>(upgraders)(forward<CurrentStruct>(currentStruct));
                    return upgradeToLatest<fromFV + 1>(forward<decltype(nextStruct)>(nextStruct), upgraders);
                }
            }
        };

        template <typename List, typename Upgraders, size_t... Is>
        [[nodiscard]] inline consteval bool validateUpgraderReturns(index_sequence<Is...>) noexcept {
            return (... && (
                is_same_v<
                    tuple_element_t<Is + 1, List>,
                    ReturnType<tuple_element_t<Is, Upgraders>>
                > &&
                is_same_v<
                    tuple_element_t<Is, Upgraders>,
                    remove_reference_t<ArgumentType<tuple_element_t<Is, Upgraders>, 0>>
                >
            ));
        }

        template <u32 minFV, u32 latestFV, typename List, auto upgraders> requires (minFV >= 1) && (minFV <= latestFV) && IsTypeList<List>
        [[nodiscard]] inline bool process(const span<const u8> data, LastType<List>& result) noexcept {
            static_assert(
                tuple_size_v<typename List::types> == latestFV - minFV + 1,
                "Result type count must equal to (latestFV - minFV + 1)"
            );
            static_assert(
                tuple_size_v<decltype(upgraders)> == latestFV - minFV,
                "Upgrader count must equal to (latestFV - minFV)"
            );
            static_assert(
                validateUpgraderReturns<List, decltype(upgraders)>(make_index_sequence<latestFV - minFV>{}),
                "Type[i] must match upgrader[i - 1]'s return type"
            );

            const u32 formatVersion = probeFormatVersion(data);
            if (formatVersion == INVALID_FORMAT_VERSION) {
                lerr << "[Util::Json] Format version probe failed or format version is `0`." << endl;
                return false;
            }
            else if (formatVersion < minFV) {
                lerr << "[Util::Json] Format version " << formatVersion << " is less than minimum supported version: " << minFV << "." << endl;
                return false;
            }
            else if (formatVersion > latestFV) {
                lerr << "[Util::Json] Format version " << formatVersion << " is greater than latest supported version: " << latestFV << "." << endl;
                return false;
            }
            return ChainProcessor<minFV, minFV, latestFV, List, upgraders>::process(formatVersion, data, result);
        }
    }
    
    template <JSONKind kind>
    struct KindMeta;

    #define REGISTER_JSON_KIND(kind_, minFV, latestFV, List, upgraders)                         \
    template <> struct Util::Json::KindMeta<Util::Json::JSONKind::kind_> {                      \
        static constexpr auto kind = Util::Json::JSONKind::kind_;                               \
        using LatestType = Util::Json::LastType<List>;                                          \
        static bool read(const std::span<const u8> data, LatestType& result) noexcept {         \
            return Util::Json::detail::process<minFV, latestFV, List, upgraders>(data, result); \
        }                                                                                       \
        static bool write(const LatestType& input, vector<u8>& result) noexcept {               \
            return writeJSON(input, result);                                                    \
        }                                                                                       \
    };

    template <JSONKind kind>
    using Latest = KindMeta<kind>::LatestType;
}