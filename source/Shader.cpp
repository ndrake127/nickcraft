#include "Shader.h"

void Shader::load(const char *vPath, const char *fPath){
	std::string vCode;
	std::string fCode;
	std::ifstream vFile;
	std::ifstream fFile;

	vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try{
		vFile.open(vPath);
		fFile.open(fPath);
		std::stringstream vStream, fStream;

		vStream << vFile.rdbuf();
		fStream << fFile.rdbuf();

		vFile.close();
		fFile.close();

		vCode = vStream.str();
		fCode = fStream.str();
	}catch(std::ifstream::failure e){
		std::cout << "Error: Shader file not successfully read\n";
	}

	const char *vShaderCode = vCode.c_str();
	const char *fShaderCode = fCode.c_str();

	int success;
	char infoLog[512];

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "Error: Vertex shader compilation failed\n";
	}
	
	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "Error: Fragment shader compilation failed\n";
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Error: Shader program linking failed\n";
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use(){
	glUseProgram(ID);
}

const unsigned int & Shader::getID() const {
	return ID;
}

void Shader::setBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, float value1, float value2, float value3) const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);	
}
