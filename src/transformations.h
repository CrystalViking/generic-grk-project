#pragma once

#include "freeglut.h"
#include "ext.hpp"

namespace grk
{
	glm::mat4 orbitalSpeed(float angle);
    glm::mat4 scaling(float size);
    glm::mat4 moonRotation(float angle, float deviation);
    glm::mat4 cometRotation(float angle, glm::vec3 rotationAxis);
}
