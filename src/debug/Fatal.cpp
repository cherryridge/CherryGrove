
#include "Fatal.hpp"

namespace Fatal {
    void exit(i32 code) {
        lerr << "[Fatal] Exit code: " << code << endl;
        std::abort();
    }
}