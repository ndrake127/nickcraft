#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in float aID;

out vec2 bTex;
out float bID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(aPos - 0.5f, 1.0f);
	bTex = aTex;
	bID = aID;
}
