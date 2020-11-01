#include "Mesh.h"

Mesh::Mesh(){	
	// create the array and buffer objects, id's stored in private member variables
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	// bind the vao
	glBindVertexArray(VAO);

	// bind the vbo and send buffer data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// this says that each vertex is defined by 3 floats. the first attribute, position, is the only one enabled. color is defined by shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw(){
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);	
}

void Mesh::draw2(){
	glBindVertexArray(VAO);
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
