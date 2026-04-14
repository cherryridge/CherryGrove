#pragma once
#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid.hpp>

#include "../util/macros.hpp"

namespace Pack {
    typedef uint32_t u32;
    using boost::uuids::uuid;

    //This is the simplistic, **hashable** identifier of a pack. Reach out `Manifest_v*` for more information.
    //note: It seems that we only need uuid for pack identification because we disallow multiple versions of the same pack to be loaded at the same time. So we disabled this.
    //struct PackIdentifier {
    //    uuid id;
    //    u32 version;
    //
    //    [[nodiscard]] bool operator==(const PackIdentifier& other) const noexcept { return id == other.id && version == other.version; }
    //};

    //[min, max], both inclusive. If `max` is `0`, it means there is no upper bound. Use `min == 1` for the same intent.
    struct PackVersionRange {
        uuid id;
        u32 min, max;

        [[nodiscard]] bool operator==(const PackVersionRange& other) const noexcept {
            return id == other.id && min == other.min && max == other.max;
        }

        [[nodiscard]] bool contains(const uuid& packId, u32 packVersion) const noexcept {
            if (id != packId) return false;
            if (max == 0) return packVersion >= min;
            else return packVersion >= min && packVersion <= max;
        }

        //[[nodiscard]] bool contains(const PackIdentifier& pack) const noexcept {
        //    if (max == 0) return id == pack.id && pack.version >= min;
        //    else return id == pack.id && pack.version >= min && pack.version <= max;
        //}
    };
}

//IMPL_HASH_FOR(Pack::PackIdentifier, 12914,
//    boost::hash_combine(seed, input.id);
//    boost::hash_combine(seed, input.version);
//)
//#undef IMPL_HASH_FOR