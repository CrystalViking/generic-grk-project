#include "Texture.h"

#include <fstream>
#include <iterator>
#include <vector>
#include "picopng.h"
#include "stb_image.h"

typedef unsigned char byte;


GLuint Core::LoadTexture(const char* filename)
{
    //std::string path = std::string(filename);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Core::SetActiveTexture(GLuint textureID, const char* shaderVariableName, GLuint programID, int textureUnit)
{
    glUniform1i(glGetUniformLocation(programID, shaderVariableName), textureUnit);
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}


//GLuint Core::LoadTexture( const char * filepath )
//{
//	GLuint id;
//	glGenTextures(1, &id);
//	glBindTexture(GL_TEXTURE_2D, id);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	std::ifstream input( filepath, std::ios::binary );
//	std::vector<char> buffer((
//		std::istreambuf_iterator<char>(input)), 
//		(std::istreambuf_iterator<char>()));
//
//	unsigned long w, h;
//	std::vector<unsigned char> decoded;
//	decodePNG(decoded, w, h, (unsigned char*)&buffer[0], buffer.size(), true);
//	
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &decoded[0]);
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	return id;
//}
//
//void Core::SetActiveTexture(GLuint textureID, const char * shaderVariableName, GLuint programID, int textureUnit)
//{
//	glUniform1i(glGetUniformLocation(programID, shaderVariableName), textureUnit);
//	glActiveTexture(GL_TEXTURE0 + textureUnit);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//}
