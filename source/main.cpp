#include "Game.h"

int main(){
	Game game;
	game.load();
 
	while(!game.shouldClose()){
		game.update();	
	}

	glfwTerminate();
	return 0;
}
