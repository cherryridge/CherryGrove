#pragma once

#include "NBTKind.hpp"

namespace Util::NBT {

    template <NBTKind T>
    struct Latest_NBT_ {
        using type = void;
    };

    template <NBTKind T>
    using Latest_NBT = Latest_NBT_<T>::type;

    #define REGISTER_LATEST_NBT(kind, type_) \
    template <> struct Util::NBT::Latest_NBT_<Util::NBT::NBTKind::kind> { using type = type_; }
}