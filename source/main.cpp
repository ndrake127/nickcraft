#include "Game.h"

#include <iostream>
#include <string>
#include <stdlib.h>

int main(int argc, char** argv){
	std::string texture = "atlas";
	int seed = 1337;
	std::string world = "nosave";
	int textureCount = 1;
	bool showFPS = false;

	if(argc > 1){
		for(int i = 1; i < argc-1; i+=2){
			if(strcmp("--texture=", argv[i]) == 0){
				texture = argv[i+1];
			}else if(strcmp("--seed=", argv[i]) == 0){
				seed = atoi(argv[i+1]);
			}else if(strcmp("--world=", argv[i]) == 0){
				world = argv[i+1];
			}else if(strcmp("--maps=", argv[i]) == 0){
				textureCount = atoi(argv[i+1]);
			}else if(strcmp("--showFPS=", argv[i]) == 0){
				showFPS = (strcmp("true", argv[i+1]) == 0);
			}
		}
	}

	std::cout << "Texture Pack: " << texture << '\n'
		  << "Texture Count: " << textureCount << '\n'
		  << "Seed: " << seed << '\n'
		  << "World: " << world << '\n';

	Game game;
	game.load(texture, seed, world, textureCount, showFPS);
	
	while(!game.shouldClose()){
		game.update();	
	}

	glfwTerminate();
	return 0;
}
