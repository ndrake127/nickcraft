#include "Game.h"
#include <iostream>
#include <iomanip>

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
	
	// glad: load all OpenGL function pointers
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD\n";
		exit(0);
	}

	// this just makes things work
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);  
	glFrontFace(GL_CW);  	
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// locks cursor to center of screen, enables raw mouse motion
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if(glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	deltaTime = 0.0f;
	currentFrame = 0.0f;
	lastFrame = 0.0f;

	// keeps track of when to end the game loop in main
	close = false;
}

void Game::load(){
	shader.load("Shaders/shader.vs", "Shaders/shader.fs");
	shader.use();
	
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 10000.0f);
	projectionLoc = glGetUniformLocation(shader.getID(), "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	camera.init(glm::vec3(0.0f, 0.0f, -3.0f), glGetUniformLocation(shader.getID(), "view"), &deltaTime);

	manager.loadTexture("textures/atlas.png");
	manager.setTexture();

	world.init(&face, &shader, &camera, glGetUniformLocation(shader.getID(), "model"));

	std::cout << "init done\n";
}

void Game::update(){
	currentTick = glfwGetTime();
	deltaTime = currentTick - lastTick;
	lastTick = currentTick;
	//std::cout << (int)(1.0f/deltaTime) << '\n';

	glClearColor(0.45f, 0.74f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render here
	shader.use();
	camera.update();

	currentFrame = glfwGetTime();
	deltaDrawTime = currentFrame - lastFrame;
	if(1.0f/deltaDrawTime <= 144.0f){
		std::cout << "\n\n" << "Draw FPS: " << (float)(1.0f/deltaDrawTime) << "\n\n";
		
		world.draw();
		lastFrame = currentFrame;
		
		glfwSwapBuffers(window);
	}

	world.update();

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
				camera.set(FRONT);
				break;
			case GLFW_KEY_A:
				camera.set(LEFT);
				break;
			case GLFW_KEY_S:
				camera.set(BACK);
				break;
			case GLFW_KEY_D:
				camera.set(RIGHT);
				break;
			case GLFW_KEY_SPACE:
				camera.set(UP);
				break;
			case GLFW_KEY_C:
				camera.set(DOWN);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				camera.sprint();
				break;
		}
	}
}

void Game::mouseEvent(GLFWwindow *window, double x, double y){
	camera.rotate(x, y);
}

void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
	Game* game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
	if(game) game->keyEvent(window, key, scancode, action, mods);
}

void Game::mouseCallback(GLFWwindow *window, double x, double y){
	Game* game = reinterpret_cast<Game *>(glfwGetWindowUserPointer(window));
	if(game) game->mouseEvent(window, x, y);

}
void Game::framebufferSizeCallback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}
