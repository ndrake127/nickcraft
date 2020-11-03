#include "ResourceManager.h"
#include "glad/glad.h"

#include <iostream>

ResourceManager::ResourceManager(){
	textureList.push_back(0);
}

void ResourceManager::loadTexture(const std::string &path){
	int width, height, channels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if(!data) exit(0);

	unsigned int id;
	glGenTextures(1, &id);
	textureList.push_back(id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);	
}

const unsigned int & ResourceManager::getTexture(int id) const {
	return textureList.at(id);
}
