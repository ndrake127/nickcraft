#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in float aID;

out vec3 bPos;
out vec2 bTex;
out vec3 bNorm;
out float bID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(aPos - 0.5, 1.0f);
	bPos = vec3(model * vec4(aPos, 1.0));
	bTex = aTex;
	bNorm = aNorm;
	bID = aID;
}
