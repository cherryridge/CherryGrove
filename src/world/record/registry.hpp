#pragma once
#include <vector>

#include "RecordDesc.hpp"
#include "../../util/SlotTable.hpp"
#include "../../util/RuntimeBijection.hpp"

namespace World::Record {
    using std::vector, Util::SlotTable, Util::RuntimeBijection;

    MAKE_DISTINCT_HANDLE(SchemaHandle)

    namespace detail {
        inline SlotTable<RecordDesc, SchemaHandle> schemaRegistry;

    }
}