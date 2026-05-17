#pragma once
#include <iostream>
#include <utility>

#include "../globalState.hpp"
#include "loggers.hpp"
#include "manips.hpp"

namespace Debug {
    using std::cout, std::cerr, std::endl, std::forward;

    inline void LOGGER_DYNAMIC_OUT(auto&&... ts) noexcept {
        if (GlobalState::multiThreadEra()) {
            ((lout << forward<decltype(ts)>(ts)), ...);
            lout << detail::nlaf;
        }
        else {
            ((cout << forward<decltype(ts)>(ts)), ...);
            cout << endl;
        }
    }

    inline void LOGGER_DYNAMIC_ERR(auto&&... ts) noexcept {
        if (GlobalState::multiThreadEra()) {
            ((lerr << forward<decltype(ts)>(ts)), ...);
            lerr << detail::nlaf;
        }
        else {
            cerr << "(Error)";
            ((cerr << forward<decltype(ts)>(ts)), ...);
            cerr << endl;
        }
    }
}