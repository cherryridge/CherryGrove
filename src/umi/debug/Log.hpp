#pragma once
#include <string_view>

#include "../../debug/Logger.hpp"

namespace Umi {
    using std::string_view;

    //todo:
    inline void message(const string_view str) noexcept {
        lout << str << '\n';
    }

    inline void warning(const string_view str) noexcept {
        lout << "Warning: " << str << '\n';
    }

    inline void error(const string_view str) noexcept {
        lerr << "[]" << str << '\n';
    }
}