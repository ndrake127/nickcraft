#ifndef GAME_H
#define GAME_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <vector>

#include "World.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

class Game{
	public:
		Game();
		~Game();

		void load();
		void update();

		bool shouldClose() const {return close;}

	private:
		GLFWwindow *window;
		Shader shader;
		ResourceManager manager;
		Camera camera;
		Mesh *mesh;
		World world;

		glm::mat4 projection;
		unsigned int projectionLoc;

		static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
		void keyEvent(GLFWwindow *window, int key, int scancode, int action, int mods);
		
		static void mouseCallback(GLFWwindow *window, double x, double y);
		void mouseEvent(GLFWwindow *window, double x, double y);

		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

		bool close;

		float deltaTime;
		float currentFrame;
		float lastFrame;
};

#endif
