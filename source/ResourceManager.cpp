#include "ResourceManager.h"
#include "glad/glad.h"

#include <iostream>

ResourceManager::ResourceManager(){

}

void ResourceManager::loadTexture(const std::string &path){
	stbi_set_flip_vertically_on_load(true);	
	int width, height, channels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if(!data) exit(0);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // enable for mipmapping	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);	
}

void ResourceManager::setTexture() const {
	glBindTexture(GL_TEXTURE_2D, textureID);
}
