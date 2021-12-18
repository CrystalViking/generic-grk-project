#include "Shader.h"

namespace grk {

	void Shader::load(Core::Shader_Loader loader, char* vertexPath, char* fragmentPath) {
		program = loader.CreateProgram(vertexPath, fragmentPath);
	}

	void Shader::use() {
		glUseProgram(program);
	}

	GLint Shader::getUniform(char* name) {
		return glGetUniformLocation(program, name);
	}

	GLuint Shader::getShader() {
		return program;
	}

	void Shader::setInt(char* uniformName, int value) {
		glUniform1i(getUniform(uniformName), value);
	}

	void Shader::setMat4(char* name, glm::mat4& value) {
		glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, glm::value_ptr(value));
	}
}