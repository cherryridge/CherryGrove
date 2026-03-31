#pragma once

namespace Util::Json {
    #define GLAZE_CONSTRAINT_ASSERT(condition, message) \
    if (!(condition)) {                                 \
        ctx.error = error_code::constraint_violated;    \
        ctx.errorMessage = message;                     \
        return;                                         \
    }
}