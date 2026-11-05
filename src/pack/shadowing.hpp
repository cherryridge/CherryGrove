#pragma once
#include <string>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>

#include "../util/wrappers/uuid.hpp"
#include "registry.hpp"

namespace Pack::detail {
    using std::string, std::vector, boost::unordered_flat_map, Util::Wrapper::uuid_JSON;

    //todo: Actually sort by topological order instead of just inserting, when we implement dependency tracking.
    inline void updateShadowing() noexcept {
        shadowingInfo.clear();
        for (const auto& [packUUID, info] : packInfo) {
            const auto it = shadowingInfo.find(info.manifest.nameSpace);
            if (it == shadowingInfo.end()) shadowingInfo.emplace(info.manifest.nameSpace, vector<uuid_JSON>{packUUID});
            else it->second.push_back(packUUID);
        }
    }
}