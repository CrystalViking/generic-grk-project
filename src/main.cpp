#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <ctime>

#include "Texture.h"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "transformations.h"
#include "skybox.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace grk {

	int mainWindow;
	float windowWidth = 600.0;
	float windowHeight = 600.0;

	// Shaders
	Shader program;
	Shader programSunTexturing;
	Shader programTexturing;
	Shader programProceduralTexturing;	//  for spaceship

	// Objects
	obj::Model shipModel;
	obj::Model sphereModel;

	// Textures
	GLuint textureSun, textureMercury, textureVenus, textureEarth, textureComet;
	GLuint programTexture;
	GLuint programTextureSpecular;

	Core::Shader_Loader shaderLoader;
	Core::RenderContext shipContext;
	Core::RenderContext sphereContext;

	float cameraAngle = 0;
	glm::vec3 cameraPos = glm::vec3(0, 0, 7);
	glm::vec3 cameraDir; // Wektor "do przodu" kamery
	glm::vec3 cameraSide; // Wektor "w bok" kamery
	glm::mat4 cameraMatrix, perspectiveMatrix;

	// grk7 - quaternions and camera movement
	glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));
	glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
	float zOffset = 0.0;
	float xOffset, yOffset;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// variables for fps check
	int myframe;
	long mytime, mytimebase;


	

	void keyboard(unsigned char key, int x, int y)
	{
		float angleSpeed = 2.0f;
		float moveSpeed = 0.1f;
		switch (key)
		{
		case 27: exit(0);	// ESC key to exit the window
		case 'z': zOffset -= angleSpeed; break;
		case 'x': zOffset += angleSpeed; break;
		case 'w': cameraPos += cameraDir * moveSpeed; break;
		case 's': cameraPos -= cameraDir * moveSpeed; break;
		case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
		case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
		case 'e': cameraPos += glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
		case 'q': cameraPos -= glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
		}
	}

	void mouse(int x, int y)
	{
		const float mouseSensitivity = 1.0f;
		xOffset = (x - (windowWidth / 2)) * mouseSensitivity;
		yOffset = (y - (windowHeight / 2)) * mouseSensitivity;
	}

	glm::mat4 createCameraMatrix(float xOffset, float yOffset)
	{
		glm::quat xAxisQuaternion = glm::angleAxis(glm::radians(xOffset), glm::vec3(0, 1, 0));
		glm::quat yAxisQuaternion = glm::angleAxis(glm::radians(yOffset), glm::vec3(1, 0, 0));
		glm::quat rotationChange = yAxisQuaternion * xAxisQuaternion;

		rotation = glm::normalize(rotationChange * rotation);
		cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
		cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

		glutWarpPointer(windowWidth / 2, windowHeight / 2);	// locks cursor inside window

		return Core::createViewMatrixQuat(cameraPos, rotation);
	}

	void drawObject(Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color, Shader& program)
	{
		program.use();
		glUniform3f(program.getUniform("objectColor"), color.x, color.y, color.z);

		glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;

		glUniformMatrix4fv(program.getUniform("transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(program.getUniform("modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		Core::DrawContext(context);
		glUseProgram(0);
	}

	// drawObject for renderRecursive
	void drawObject(GLuint program, Core::RenderContext context, glm::mat4 modelMatrix)
	{
		glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;


		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		context.render();
	}

	void drawObjectTexture(Core::RenderContext context, glm::mat4 modelMatrix, GLuint textureID, Shader& program)
	{
		program.use();
		Core::SetActiveTexture(textureID, "colorTexture", program.getShader(), 0);

		glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
		glUniformMatrix4fv(program.getUniform("transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(program.getUniform("modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		Core::DrawContext(context);
		glUseProgram(0);
	}

	void renderScene()
	{
		cameraMatrix = createCameraMatrix(xOffset, yOffset);
		perspectiveMatrix = Core::createPerspectiveMatrix();

		// per-frame time logic
		float currentFrame = glutGet(GLUT_ELAPSED_TIME);
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// per-frame time logic [z zajec]
		float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.3f, 0.3f, 1.0f);


		// Macierz statku "przyczepia" go do kamery.
		//glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0, -0.25f, 0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
		glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0, -0.25f, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(zOffset), glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(0.25f));
		glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * shipInitialTransformation;


		program.use();
		glUniform3f(program.getUniform("lightPos"), 0, 0, 0);
		glUniform3f(program.getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform3f(program.getUniform("spotlightPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform3f(program.getUniform("spotlightDir"), cameraDir.x, cameraDir.y, cameraDir.z);
		glUniform1f(program.getUniform("spotlightCutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(program.getUniform("spotlightOuterCutOff"), glm::cos(glm::radians(17.5f)));

		programTexturing.use();
		glUniform3f(programTexturing.getUniform("lightPos"), 0, 0, 0);
		glUniform3f(programTexturing.getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform3f(programTexturing.getUniform("spotlightPos"), cameraPos.x, cameraPos.y, cameraPos.z);
		glUniform3f(programTexturing.getUniform("spotlightDir"), cameraDir.x, cameraDir.y, cameraDir.z);
		glUniform1f(programTexturing.getUniform("spotlightCutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(programTexturing.getUniform("spotlightOuterCutOff"), glm::cos(glm::radians(17.5f)));

		programProceduralTexturing.use();
		glUniform3f(programProceduralTexturing.getUniform("lightPos"), 0, 0, 0);
		glUniform3f(programProceduralTexturing.getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

		programSunTexturing.use();
		glUniform3f(programSunTexturing.getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

		drawObject(shipContext, shipModelMatrix, glm::vec3(0.6f), programProceduralTexturing);

		// Sun
		drawObjectTexture(sphereContext, glm::translate(glm::vec3(0, 0, 0)) * glm::scale(glm::vec3(0.95, 0.95, 0.95)), textureSun, programSunTexturing);
		// Mercury
		drawObjectTexture(sphereContext, orbitalSpeed(300) * glm::translate(glm::vec3(1.5f, 0.f, 0.f)) * scaling(0.20), textureMercury, programTexturing);
		// Venus
		drawObjectTexture(sphereContext, orbitalSpeed(150) * glm::translate(glm::vec3(2.f, 0.f, 0.f)) * scaling(0.30), textureVenus, programTexturing);
		// Earth
		drawObjectTexture(sphereContext, orbitalSpeed(120) * glm::translate(glm::vec3(3.f, 0.f, 0.f)) * scaling(0.35), textureEarth, programTexturing);
		// Moon
		drawObject(sphereContext, orbitalSpeed(120) * glm::translate(glm::vec3(3.f, 0.f, 0.f)) * moonRotation(65, 0.005) * glm::translate(glm::vec3(0.5f, 0.f, 0.f)) * scaling(0.05), glm::vec3(0.3), program);
		// Comet
		drawObjectTexture(sphereContext, cometRotation(200, glm::vec3(1.f, -0.5f, 0.7f)) * glm::translate(glm::vec3(0.f, 4.f, 0.f)) * scaling(0.20), textureComet, programTexturing);

		renderSkybox(cameraMatrix, perspectiveMatrix);


		/*
		// Code to check fps (simply uncomment to use)
		myframe++;
		time = glutGet(GLUT_ELAPSED_TIME);
		if (time - mytimebase > 1000) {
			printf("FPS:%4.2f\n", myframe * 1000.0 / (time - mytimebase));
			mytimebase = time;
			myframe = 0;
		}
		*/

		glutSwapBuffers();
	}

	void renderRecursive(std::vector<Core::Node>& nodes) {
		for (auto node : nodes) {
			if (node.renderContexts.size() == 0) {
				continue;
			}



			glm::mat4 transformation = glm::mat4();
			// dodaj odwolania do nadrzednych zmiennych
			Core::Node extraNode = node;

			transformation = extraNode.matrix;

			while (extraNode.parent != -1)
			{
				transformation = nodes[extraNode.parent].matrix * transformation;
				//transformation =  transformation * nodes[node.parent].matrix;
				extraNode = nodes[extraNode.parent];
			}

			for (auto context : node.renderContexts) {
				auto program = context.material->program;
				glUseProgram(program);
				glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
				context.material->init_data();
				drawObject(program, context, transformation);
			}
		}

	}
	
	Core::Material* loadDiffuseMaterial(aiMaterial* material) {
		aiString colorPath;
		// use for loading textures

		material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), colorPath);
		if (colorPath == aiString("")) {
			return nullptr;
		}
		Core::DiffuseMaterial* result = new Core::DiffuseMaterial();
		result->texture = Core::LoadTexture(colorPath.C_Str());
		result->program = programTexture;
		result->lightDir = lightDir;

		return result;
	}

	Core::Material* loadDiffuseSpecularMaterial(aiMaterial* material) {
		aiString colorPath;
		// use for loading textures

		material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), colorPath);
		Core::DiffuseSpecularMaterial* result = new Core::DiffuseSpecularMaterial();
		result->texture = Core::LoadTexture(colorPath.C_Str());


		aiString specularPath;
		material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), specularPath);
		result->textureSpecular = Core::LoadTexture(specularPath.C_Str());
		result->lightDir = lightDir;
		result->program = programTextureSpecular;

		return result;
	}

	void loadRecusive(const aiScene* scene, aiNode* node, std::vector<Core::Node>& nodes, std::vector<Core::Material*> materialsVector, int parentIndex) {
		int index = nodes.size();
		nodes.push_back(Core::Node());
		nodes[index].parent = parentIndex;
		nodes[index].matrix = Core::mat4_cast(node->mTransformation);
		for (int i = 0; i < node->mNumMeshes; i++) {
			Core::RenderContext context;
			context.initFromAssimpMesh(scene->mMeshes[node->mMeshes[i]]);
			context.material = materialsVector[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
			nodes[index].renderContexts.push_back(context);
		}
		for (int i = 0; i < node->mNumChildren; i++) {
			loadRecusive(scene, node->mChildren[i], nodes, materialsVector, index);
		}
	}

	void loadRecusive(const aiScene* scene, std::vector<Core::Node>& nodes, std::vector<Core::Material*> materialsVector) {

		loadRecusive(scene, scene->mRootNode, nodes, materialsVector, -1);
	}

	void init()
	{
		glEnable(GL_DEPTH_TEST);

		program.load(shaderLoader, "shaders/shader.vert", "shaders/shader.frag");
		programSunTexturing.load(shaderLoader, "shaders/sun.vert", "shaders/sun.frag");
		programTexturing.load(shaderLoader, "shaders/shader_tex.vert", "shaders/shader_tex.frag");
		programProceduralTexturing.load(shaderLoader, "shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");

		sphereModel = obj::loadModelFromFile("models/sphere.obj");
		shipModel = obj::loadModelFromFile("models/spaceship.obj");

		textureSun = Core::LoadTexture("textures/sun.png");
		textureEarth = Core::LoadTexture("textures/earth.png");
		textureMercury = Core::LoadTexture("textures/mercury.png");
		textureVenus = Core::LoadTexture("textures/venus.png");
		textureComet = Core::LoadTexture("textures/comet.png");

		shipContext.initFromOBJ(shipModel);
		sphereContext.initFromOBJ(sphereModel);

		initializeSkybox(shaderLoader);
	}

	void shutdown()
	{
		shaderLoader.DeleteProgram(program.getShader());
		shaderLoader.DeleteProgram(programSunTexturing.getShader());
		shaderLoader.DeleteProgram(programTexturing.getShader());
		shaderLoader.DeleteProgram(programProceduralTexturing.getShader());

		deleteSkybox();
	}

	/*
	void idle()
	{
		glutPostRedisplay();
	}
	*/

	void timer(int) {
		glutPostRedisplay();
		glutTimerFunc(1000 / 60, timer, 0);
	}
}

using namespace grk;
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windowWidth, windowHeight);
	mainWindow = glutCreateWindow("OpenGL project");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutDisplayFunc(renderScene);
	//glutIdleFunc(idle);	// CPU usage goes up to ~99% with this; new solution below
	timer(0);				// restricts program to ~60 fps

	glutMainLoop();

	shutdown();

	return 0;
}
