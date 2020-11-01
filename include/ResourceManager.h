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
		const unsigned int& getTexture(int id) const;

	private:
		std::vector<unsigned int> textureList;
};

#endif
