#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <stb_image.h>
#include <vector>
#include <string>

class ResourceManager{
	public:
		ResourceManager();

		// texture management
		void loadTexture(const std::string &path);
		void setTexture() const;

	private:
		unsigned int textureID;
};

#endif
