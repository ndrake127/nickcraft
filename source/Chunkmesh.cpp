#include "Chunkmesh.h"
#include <algorithm>

Chunkmesh::Chunkmesh(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);	
}

Chunkmesh::~Chunkmesh(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Chunkmesh::setShader(Shader *shader){
	this->shader = shader;
}

void Chunkmesh::resetMesh(){
	if(!vertices.empty()) vertices.clear();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(face), NULL, GL_STATIC_DRAW);
}

void Chunkmesh::setupMesh(){
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(face), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, textureCoord));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 1, GL_BYTE, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, id));
        glEnableVertexAttribArray(3);
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
}

void Chunkmesh::draw() const {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size()*6);
	glBindVertexArray(0);
}
