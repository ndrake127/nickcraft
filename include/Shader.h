#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader{
	public:
		void load(const char *vPath, const char *fPath);
		void use();
		const unsigned int & getID() const;
		void setBool(const std::string &name, bool value) const;
		void setInt(const std::string &name, int value) const;
		void setFloat(const std::string &name, float value) const;
		void setVec3(const std::string &name, float value1, float value2, float value3) const;

	private:
		unsigned int ID;
};

#endif
