#include "ResourceManager.h"
#include "glad/glad.h"

#include <iostream>

ResourceManager::ResourceManager(){
	textureID.resize(0);
	textureIndex = 0;
}

void ResourceManager::loadTexture(const std::string &path, int animationCount){
	stbi_set_flip_vertically_on_load(true);	
	
	for(int i = 0; i < animationCount; i++){
		int width, height, channels;
		std::string tempPath = path + std::to_string(i+1) + ".png";
		std::cout << "Loading texture: " << tempPath << '\n';
		unsigned char *data = stbi_load(tempPath.c_str(), &width, &height, &channels, 0);
	
		if(!data) exit(0);
		
		textureID.resize(textureID.size()+1);
		glGenTextures(1, &(textureID[textureID.size()-1]));
		glBindTexture(GL_TEXTURE_2D, textureID[textureID.size()-1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
}

void ResourceManager::setTexture(){
	glBindTexture(GL_TEXTURE_2D, textureID[textureIndex++]);
	if(textureIndex >= textureID.size()) textureIndex = 0;
}
