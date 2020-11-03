#include "Chunkmesh.h"
#include <algorithm>

void Chunkmesh::setShader(Shader *shader){
	this->shader = shader;
}

void Chunkmesh::setupMesh(){	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(face), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_BYTE, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, textureCoord));
        glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Chunkmesh::pushFace(vertex *vertices, unsigned char id){
	face temp;
	for(int i = 0; i < 6; i++){
		vertices[i].id = id;
		temp.vertices[i] = vertices[i];
	}
	this->vertices.push_back(temp);
}

void Chunkmesh::sort(){
	std::sort( vertices.begin(), vertices.end(), []( const face &left, const face &right ){ return ( left.vertices[0].id < right.vertices[0].id ); } );
	unsigned char prev = vertices.at(0).vertices[0].id;
	textures.push_back(prev);
	toRender.push_back(1);
	for(unsigned int i = 1; i < vertices.size(); i++){
		unsigned char curr = vertices.at(i).vertices[0].id;
		if(curr != prev){
			textures.push_back((unsigned int)curr);
			toRender.push_back(1);
			prev = curr;
			continue;
		}
		toRender.back()++;
	}
}

void Chunkmesh::draw() const {
	glBindVertexArray(VAO);
	unsigned int rendered = 0;

	for(unsigned int i = 0; i < textures.size(); i++){
		glBindTexture(GL_TEXTURE_2D, textures.at(i));
		glDrawArrays(GL_TRIANGLES, rendered, toRender.at(i)*6);
		rendered += toRender.at(i)*6;
	}
	
	glBindVertexArray(0);
}
