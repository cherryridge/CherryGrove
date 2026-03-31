#pragma once
#include <concepts>
#include <filesystem>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Util {
    using std::string, std::string_view, std::filesystem::path, std::decay_t, std::constructible_from, std::convertible_to, std::is_unsigned_v, std::is_signed_v, std::is_floating_point_v, std::derived_from, std::same_as, std::remove_cvref_t, std::make_index_sequence, std::index_sequence, std::is_aggregate_v;

    template <typename T>
    concept SInt = is_signed_v<T>;

    template <typename T>
    concept UInt = is_unsigned_v<T>;

    template <typename T>
    concept Int = SInt<T> || UInt<T>;

    template <typename T>
    concept FP = is_floating_point_v<T>;

    template <typename T, typename U>
    concept Equal = same_as<remove_cvref_t<T>, remove_cvref_t<U>>;

    template <typename T, typename U>
    concept EqualStrict = same_as<T, U>;

    template <typename T, typename U>
    concept EqualLoose = same_as<decay_t<T>, decay_t<U>>;

    template <typename T, typename... Us>
    concept OneOf = (Equal<T, Us> || ...);

    template <typename T, typename... Us>
    concept OneOfStrict = (EqualStrict<T, Us> || ...);

    template <typename T, typename... Us>
    concept OneOfLoose = (EqualLoose<T, Us> || ...);

    template <typename T, typename... Us>
    concept AllIs = (Equal<T, Us> && ...);

    template <typename T, typename... Us>
    concept AllIsStrict = (EqualStrict<T, Us> && ...);

    template <typename T, typename... Us>
    concept AllIsLoose = (EqualLoose<T, Us> && ...);

    template <typename T, typename U>
    concept Like = constructible_from<U, T> || convertible_to<T, U>;

    template <typename T, typename U>
    concept DerivedFrom = derived_from<T, U>;

    namespace MemberCount {
        struct UniversalType {
            template <typename T>
            operator T() const noexcept;
        };

        template <typename T, size_t N, typename Indices = make_index_sequence<N>>
        struct CanInitialize;

        template <typename T, size_t N, size_t... Indices>
        struct CanInitialize<T, N, index_sequence<Indices...>> {
            static constexpr bool value = requires { T{ (void(Indices), UniversalType{})... }; };
        };

        template <typename T, size_t min, size_t max>
        [[nodiscard]] consteval size_t findMemberCount() noexcept {
            if constexpr (min == max) return min;
            constexpr size_t mid = (min + max + 1) >> 1;
            if constexpr (CanInitialize<T, mid>::value) return findMemberCount<T, mid, max>();
            else return findMemberCount<T, min, mid - 1>();
        }
    }

    //DO NOT USE THIS FOR COUNTING STRUCTS WITH C-STYLE ARRAYS! It will expand the array and count each element IN THE ARRAY as a member, which is not what you want in most cases.
    //But thankfully C-style arrays are banned from CG codebase anyway, so we are safe.
    template <typename T, size_t N>
    concept HasNMembers = MemberCount::CanInitialize<T, N>::value && !MemberCount::CanInitialize<T, N + 1>::value;

    //DO NOT USE THIS FOR COUNTING STRUCTS WITH C-STYLE ARRAYS! It will expand the array and count each element IN THE ARRAY as a member, which is not what you want in most cases.
    //But thankfully C-style arrays are banned from CG codebase anyway, so we are safe.
    template <typename T, size_t maxSearchRange = 64>
    constexpr size_t memberCount = MemberCount::findMemberCount<T, 0, maxSearchRange>();

    template <typename T, typename U>
    concept DistinctHandleOf = requires(T t) {
        { move(t.value) } -> EqualStrict<U&&>;
    } && HasNMembers<T, 1>;

    //Usage:
    //template <FilePath T>
    //fn(T&& arg);
    template <typename T>
    concept FilePath = same_as<decay_t<T>, const char*> || Equal<T, string> || Equal<T, string_view> || Equal<T, path>;
}