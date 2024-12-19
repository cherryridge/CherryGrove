#pragma once
#include <glm/glm.hpp>

using glm::vec2, glm::vec3;

class Camera {
public:
	Camera();
	~Camera();

private:
	vec3 position;
	vec2 rotation;
};