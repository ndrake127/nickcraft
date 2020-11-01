#include "Game.h"

int main(){
	Game game;
	game.load();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
	while(!game.shouldClose()){
		game.update();	
	}

	glfwTerminate();
	return 0;
}
