#pragma once
#include "../schemaTypes.hpp"

namespace Pack {
	//configFlags map
	inline constexpr u64 PACK_CONFIG_WARN_ON_NS_DUPE = 0x1u;
	inline constexpr u64 PACK_CONFIG_ALWAYS_LOAD = 0x2u;
	inline constexpr u64 PACK_CONFIG_LICENSE_LIBRE = 0x4u;
	inline constexpr u64 PACK_CONFIG_ALLOW_UNSAFE_EXECUTION = 0x8u;
}

namespace ManifestProcs {
	ProcessorPtr<ManifestJSON> getProcessors();
}