#version 330 core
out vec4 FragColor;

in vec2 bTex;
in float bID;

uniform sampler2D tex;

void main(){
	vec2 cTex = bTex;
	float id = bID;

	cTex.x += mod(id, 16);
	cTex.y += 15 - floor((id+(1.0f/32.0f))/16.0f);
	cTex /= 16.0f;	
	
	FragColor = texture(tex, cTex);
}
