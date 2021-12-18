#pragma once

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include "Texture.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "stb_image.h"
#include <vector>

#include "Shader.h"


namespace grk {

	void initializeSkybox(Core::Shader_Loader loader);
	void renderSkybox(glm::mat4 cameraMatrix, glm::mat4 projectionMatrix);
	void deleteSkybox();
}