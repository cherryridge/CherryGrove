#include <entt/entt.hpp>
#include <mutex>

#include "../../MainGame.hpp"
#include "../Components.hpp"

namespace Components::Velocity {
	typedef uint32_t u32;

	void updateVByA(const entt::entity& entity, u32 deltaTick) {
		const auto* acceleration = MainGame::gameRegistry.try_get<AccelerationComponent>(entity);
		if (acceleration && MainGame::gameRegistry.all_of<VelocityComponent>(entity)) {
			std::lock_guard lock(entity == MainGame::playerEntity ? MainGame::playerMutex : MainGame::registryMutex);
			MainGame::gameRegistry.patch<VelocityComponent>(entity, [&acceleration, &deltaTick](VelocityComponent& velocity) {
				u32 _del = deltaTick ? deltaTick : 1;
				velocity.dx += acceleration->d2x * _del;
				velocity.dx += acceleration->d2y * _del;
				velocity.dx += acceleration->d2z * _del;
			});
		}
	}
}