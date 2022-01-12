#pragma once

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include <vector>
class SimpleEmitter
{
public:
	SimpleEmitter();
	~SimpleEmitter();

	void update(const float dt);

	void draw();

private:
	struct Particle
	{
		glm::vec3 position;
		float lifetime;
	};
	std::vector< Particle > particles;

	float positions[400];
	GLuint vertexBuffer;
	GLuint positionBuffer;
	float randomFloat(float min, float max);
};

