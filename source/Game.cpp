#include "Game.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Face.h"
#include "Camera.h"
#include "World.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <boost/filesystem.hpp>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

Game::Game(){	
	// glfw: initialize glfw context
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window and exit if it is not created
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "NickCraft", NULL, NULL);
	if(window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(0);
	}

	// makes window current and sets window user pointer to this instance of Game
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);

	// set callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	
	// glad: load all OpenGL function pointers
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD\n";
		exit(0);
	}

	// this just makes things work
	glEnable(GL_DEPTH_TEST);

	// enable blending for transparency	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// enable face culling for performance reasons
	glEnable(GL_CULL_FACE);  
	
	// enable wireframe, commented out by default
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// locks cursor to center of screen, enables raw mouse motion
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if(glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	// initializing member variables
	
	// used to measure how fast the game loop runs
	deltaTime = 0.0f;
	currentTick = 0.0f;
	lastTick = 0.0f;

	// used to measure how FPS and cap it
	deltaDrawTime = 0.0f;
	currentFrame = 0.0f;
	lastFrame = 0.0f;

	// used to determine how frequently animations should change, higher is slower
	animationTime = 0.0f;

	showFPS = false;
	
	// keeps track of when to end the game loop in main
	close = false;

	lightPos = glm::vec3(0.0f, 256.0f, 0.0f);

	// allocating dynamic memory for child objects
	// pointers used so the Game.h can prototype classes and not explicitly include their headers, makes compilation better
	shader = new Shader;
	manager = new ResourceManager;
	camera = new Camera;
	face = new Face;
	world = new World;
}

Game::~Game(){
	delete shader;
	delete manager;
	delete camera;
	delete face;
	delete world;
}

void Game::load(std::string texture, int seed, std::string worldpath, int animationCount, bool showFPS, int renderDistance, bool showPos){
	std::string path = "textures/" + texture + '/';
	path += texture;

	worldpath = "./worlds/" + worldpath;

	shader->load("Shaders/shader.vs", "Shaders/shader.fs");
	shader->use();

	lightPosLoc = glGetUniformLocation(shader->getID(), "lightPos");

	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 10000.0f);
	projectionLoc = glGetUniformLocation(shader->getID(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	camera->init(glm::vec3(0.0f, 0.0f, -3.0f), glGetUniformLocation(shader->getID(), "view"), glGetUniformLocation(shader->getID(), "lightPos"), &deltaTime, world, showPos);
	
	manager->loadTexture(path, animationCount);
	manager->setTexture();

	boost::filesystem::path worldDir(worldpath);

	if(worldpath != "./worlds/nosave" && boost::filesystem::exists(worldDir) == false){
		boost::filesystem::create_directory(worldpath);
	}

	loadMetadata();

	world->init(this, &lightPos, face, shader, camera, glGetUniformLocation(shader->getID(), "model"), worldpath, seed, renderDistance);

	this->showFPS = showFPS;

	std::cout << "init done\n";
}

// constructs a hash table from blockdata.txt that allows constant time determination of transparency, texture id for each face, etc without storing that for each block
void Game::loadMetadata(){
	std::string path = "./config/blockdata.txt";
	
	std::fstream file;
	std::string line = "";
	file.open(path);

	if(file){
		unsigned int id;
		metadata temp;
		int faceTextureID[6];

		while(std::getline(file, line)){
			std::stringstream instream(line);
			line = "";

			instream >> id
				 >> temp.name
				 >> temp.transparent
				 >> faceTextureID[0]
				 >> faceTextureID[1]
				 >> faceTextureID[2]
				 >> faceTextureID[3]
				 >> faceTextureID[4]
				 >> faceTextureID[5];
			
			temp.faceTextureID[0] = faceTextureID[0];
			temp.faceTextureID[1] = faceTextureID[1];
			temp.faceTextureID[2] = faceTextureID[2];
			temp.faceTextureID[3] = faceTextureID[3];
			temp.faceTextureID[4] = faceTextureID[4];
			temp.faceTextureID[5] = faceTextureID[5];

			blockdata[id] = temp;;
			std::cout << id << ' '
				  << blockdata[id].name << ' '
				  << blockdata[id].transparent << ' '
				  << (int)blockdata[id].faceTextureID[0] << ' '
				  << (int)blockdata[id].faceTextureID[1] << ' '
				  << (int)blockdata[id].faceTextureID[2] << ' '
				  << (int)blockdata[id].faceTextureID[3] << ' '
				  << (int)blockdata[id].faceTextureID[4] << ' '
				  << (int)blockdata[id].faceTextureID[5] << '\n';
		}
	}else{
		exit(0);
	}
}

void Game::update(){
	currentTick = glfwGetTime();
	deltaTime = currentTick - lastTick;
	lastTick = currentTick;
	//std::cout << (int)(1.0f/deltaTime) << '\n';

	glClearColor(0.45f, 0.74f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render here
	shader->use();
	camera->update();

	// day/night cycle
	lightPos = glm::rotate(lightPos, glm::radians(3.0f)*deltaTime, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	//std::cout << lightPos.x << ' ' << lightPos.y << ' ' << lightPos.z << '\n';

	// switches texture atlas at every specified interval, used to animate blocks
	if(glfwGetTime()-animationTime > 0.5f){
		animationTime = glfwGetTime();
		manager->setTexture();
	}

	// keeps track of FPS
	currentFrame = glfwGetTime();
	deltaDrawTime = currentFrame - lastFrame;
	
	// limits fps to 144Hz
	if(1.0f/deltaDrawTime <= 144.0f){
		if(showFPS) std::cout << "\n\n" << "Draw FPS: " << (int)(1.0f/deltaDrawTime) << "\n\n";
		
		world->draw();
		lastFrame = currentFrame;
		
		glfwSwapBuffers(window);
	}

	// updates the world so it will be drawn properly next frame
	world->update();

	glfwPollEvents();	
}

void Game::keyEvent(GLFWwindow *window, int key, int scancode, int action, int mods){
	if(key == GLFW_KEY_ESCAPE){
		glfwSetWindowShouldClose(window, true);
		close = true;
	}
	
	if(action != GLFW_REPEAT){
		switch(key){
			case GLFW_KEY_W:
				camera->set(FRONT);
				break;
			case GLFW_KEY_A:
				camera->set(LEFT);
				break;
			case GLFW_KEY_S:
				camera->set(BACK);
				break;
			case GLFW_KEY_D:
				camera->set(RIGHT);
				break;
			case GLFW_KEY_SPACE:
				camera->set(UP);
				break;
			case GLFW_KEY_C:
				camera->set(DOWN);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera->sprint();
				break;
		}
	}

	if(action == GLFW_PRESS){
		switch(key){
			case GLFW_KEY_1:
				camera->setBlock(1);
				break;
			case GLFW_KEY_2:
				camera->setBlock(2);
				break;
			case GLFW_KEY_3:
				camera->setBlock(6);
				break;
			case GLFW_KEY_4:
				camera->setBlock(32);
				break;
		}
	}
}

void Game::mouseEvent(GLFWwindow *window, double x, double y){
	camera->rotate(x, y);
}

void Game::mouseButtonEvent(GLFWwindow *window, int button, int action, int mods){
	if(action == GLFW_PRESS){
		switch(button){
			case GLFW_MOUSE_BUTTON_LEFT:
				camera->rayCast(true);
				break;
			case GLFW_MOUSE_BUTTON_RIGHT:
				camera->rayCast(false);
				break;
		}
	}
}

void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
	Game* game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
	if(game) game->keyEvent(window, key, scancode, action, mods);
}

void Game::mouseCallback(GLFWwindow *window, double x, double y){
	Game* game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
	if(game) game->mouseEvent(window, x, y);
}

void Game::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods){
	Game* game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
	if(game) game->mouseButtonEvent(window, button, action, mods);
}

void Game::framebufferSizeCallback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}
