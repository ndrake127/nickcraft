#include "Face.h"
#include <iostream>

void Face::getVertices(faceDirection dir, const glm::vec3 &position, vertex returnArray[]){
	for(int i = 0; i < 6; i++){
		returnArray[i].position[0] = vertices[dir][5*i] + position.x;
		returnArray[i].position[1] = vertices[dir][5*i+1] + position.y;
		returnArray[i].position[2] = vertices[dir][5*i+2] + position.z;
		returnArray[i].textureCoord[0] = vertices[dir][5*i+3];
		returnArray[i].textureCoord[1] = vertices[dir][5*i+4];
	}	
}
