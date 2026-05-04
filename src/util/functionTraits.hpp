#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

namespace Util {
    using std::remove_cvref_t, std::function;
    
    namespace detail {
        using std::tuple, std::tuple_element_t;

        template <typename Ret, typename... Args>
        struct FunctionTraitsBase {
            using Return    = Ret;
            using Arguments = tuple<Args...>;
        
            static constexpr size_t argCount = sizeof...(Args);
        
            template <size_t N>
            using Argument = tuple_element_t<N, Arguments>;
        };

        template <typename fn>
        struct FunctionTraits : FunctionTraits<decltype(&remove_cvref_t<fn>::operator())> {};

        template <typename Ret, typename... Args>
        struct FunctionTraits<Ret(Args...)> : FunctionTraitsBase<Ret, Args...> {};

        template <typename Ret, typename... Args>
        struct FunctionTraits<Ret(Args...) noexcept> : FunctionTraitsBase<Ret, Args...> {};

        template <typename Ret, typename... Args>
        struct FunctionTraits<Ret(*)(Args...)> : FunctionTraitsBase<Ret, Args...> {};

        template <typename Ret, typename... Args>
        struct FunctionTraits<Ret(*)(Args...) noexcept> : FunctionTraitsBase<Ret, Args...> {};

        template <typename Ret, typename... Args>
        struct FunctionTraits<function<Ret(Args...)>> : FunctionTraitsBase<Ret, Args...> {};

        #define MAKE_MEMBER_FUNCTION_TRAITS(CV, REF, NOEXCEPT) \
        template <typename Ret, typename Struct, typename... Args> \
        struct FunctionTraits<Ret(Struct::*)(Args...) CV REF NOEXCEPT> : FunctionTraitsBase<Ret, Args...> { \
            using StructType = Struct; \
        };

        #define MAKE_NOEXCEPT_VARIANTS(CV, REF) \
            MAKE_MEMBER_FUNCTION_TRAITS(CV, REF, ) \
            MAKE_MEMBER_FUNCTION_TRAITS(CV, REF, noexcept)

        #define MAKE_REF_VARIANTS(CV) \
            MAKE_NOEXCEPT_VARIANTS(CV, ) \
            MAKE_NOEXCEPT_VARIANTS(CV, &) \
            MAKE_NOEXCEPT_VARIANTS(CV, &&)

        MAKE_REF_VARIANTS()
        MAKE_REF_VARIANTS(const)
        MAKE_REF_VARIANTS(volatile)
        MAKE_REF_VARIANTS(const volatile)

        #undef MAKE_REF_VARIANTS
        #undef MAKE_NOEXCEPT_VARIANTS
        #undef MAKE_MEMBER_FUNCTION_TRAITS
    }

    template <typename fn>
    using ReturnType = typename detail::FunctionTraits<fn>::Return;

    template <typename fn>
    using ArgumentTypes = typename detail::FunctionTraits<fn>::Arguments;

    template <typename fn, size_t N>
    using ArgumentType = typename detail::FunctionTraits<fn>::template Argument<N>;

    template <typename fn>
    inline constexpr size_t ArgumentCount = detail::FunctionTraits<fn>::argCount;
}