#ifndef GAME_H
#define GAME_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <string>

class ResourceManager;
class Shader;
class Face;
class Camera;
class World;

const unsigned int SCR_WIDTH = 1440;
const unsigned int SCR_HEIGHT = 900;


class Game{
	public:
		struct metadata{
			std::string name;
			bool transparent;
			unsigned int faceTextureID[6];
		};
		metadata blockdata[256];
		
		Game();
		~Game();

		void load(std::string texture, int seed, std::string worldpath, int animationCount, bool showFPS, int renderDistance, bool showPos);
		void update();

		bool shouldClose() const {return close;}

	private:
		GLFWwindow *window;
		Shader *shader;
		ResourceManager *manager;
		Camera *camera;
		Face *face;
		World *world;

		void loadMetadata();

		glm::mat4 projection;
		unsigned int projectionLoc;

		static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
		void keyEvent(GLFWwindow *window, int key, int scancode, int action, int mods);
		
		static void mouseCallback(GLFWwindow *window, double x, double y);
		void mouseEvent(GLFWwindow *window, double x, double y);

		static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
		void mouseButtonEvent(GLFWwindow *window, int button, int action, int mods);

		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

		bool close;

		float deltaTime;
		float currentTick;
		float lastTick;

		bool showFPS;
		float deltaDrawTime;
		float currentFrame;
		float lastFrame;

		float animationTime;

		glm::vec3 lightPos;
		int lightPosLoc;
};

#endif
