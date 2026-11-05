#pragma once
#include <flecs.h>
#include <vector>

#include "RecordSchema.hpp"

namespace World::Record {
    typedef uint8_t u8;
    using std::vector;

    struct RecordInstance {
        flecs::entity data;
        vector<RecordSchema*> schemas;

        [[nodiscard]] RecordInstance(const vector<RecordSchema*>& schemas) noexcept : schemas(schemas) {
            u64 sizeSum = 0;
            for (u64 i = 0; i < schemas.size(); i++) {
                sizeSum += schemas[i]->size();
            }
        }
    };
}