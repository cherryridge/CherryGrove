#pragma once
#include <exception> // IWYU pragma: keep

namespace Debug::detail {
    using std::terminate;

    struct DummyStructForManipsToTakeAsAParameterSoNothingWillCollideWithManipTemplatesOrBeAbleToCallTheManip {
    private:
        DummyStructForManipsToTakeAsAParameterSoNothingWillCollideWithManipTemplatesOrBeAbleToCallTheManip() noexcept = default;
    };

    using Manip = void(*)(DummyStructForManipsToTakeAsAParameterSoNothingWillCollideWithManipTemplatesOrBeAbleToCallTheManip) noexcept;

    //New Line And Flush.
    //DO NOT CALL. This is meant to be used as a special pass-in argument.
    [[noreturn]] inline void nlaf(DummyStructForManipsToTakeAsAParameterSoNothingWillCollideWithManipTemplatesOrBeAbleToCallTheManip) noexcept { terminate(); }

    //New Line Only. Use `nlaf` if you also want to flush.
    //DO NOT CALL. This is meant to be used as a special pass-in argument.
    [[noreturn]] inline void newLineOnly(DummyStructForManipsToTakeAsAParameterSoNothingWillCollideWithManipTemplatesOrBeAbleToCallTheManip) noexcept { terminate(); }

    //Flush Only. Use `nlaf` if you also want to add a new line.
    //DO NOT CALL. This is meant to be used as a special pass-in argument.
    [[noreturn]] inline void flushOnly(DummyStructForManipsToTakeAsAParameterSoNothingWillCollideWithManipTemplatesOrBeAbleToCallTheManip) noexcept { terminate(); }
}