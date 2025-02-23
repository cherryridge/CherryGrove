#pragma once
#include <cstdint>

namespace Components {
	struct BlockCoordinatesComponent {
		int32_t x;
		int32_t y;
		int32_t z;
		uint32_t dimensionId;
	};
}