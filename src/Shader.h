#pragma once

#include "glew.h"
#include "glm.hpp"
#include "ext.hpp"
#include "freeglut.h"
#include "Shader_Loader.h"

namespace grk {

	class Shader {
	public:
		void load(Core::Shader_Loader loader, char* vertexPath, char* fragmentPath);
		void use();
		GLint getUniform(char* name);
		GLuint getShader();
		void setInt(char* uniformName, int value);
		void setMat4(char* name, glm::mat4& value);

	private:
		GLuint program;
	};
}
