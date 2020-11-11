#ifndef GAME_H
#define GAME_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <unordered_map>


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
			unsigned char faceTextureID[6];
		};
		
		Game();
		~Game();

		void load(std::string texture, int seed, std::string worldpath, int animationCount, bool showFPS);
		void update();

		bool shouldClose() const {return close;}

	private:
		GLFWwindow *window;
		Shader *shader;
		ResourceManager *manager;
		Camera *camera;
		Face *face;
		World *world;

		std::unordered_map<unsigned char, metadata> blockdata;
		void loadMetadata();

		glm::mat4 projection;
		unsigned int projectionLoc;

		static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
		void keyEvent(GLFWwindow *window, int key, int scancode, int action, int mods);
		
		static void mouseCallback(GLFWwindow *window, double x, double y);
		void mouseEvent(GLFWwindow *window, double x, double y);

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
};

#endif
