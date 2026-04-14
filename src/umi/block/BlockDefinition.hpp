#pragma once
#include <boost/uuid/uuid.hpp>

#include "../../intrinsics/enttcomps/identity/NameAndSpace.hpp"

namespace Umi::Block {
    using boost::uuids::uuid, EnttComps::Identity::NameAndSpace;

    struct BlockDefinition {
        NameAndSpace nameAndSpace;
        uuid fromPack;

        //TextureDefinition texture;
        //
    };
}