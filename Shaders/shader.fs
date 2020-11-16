#version 330 core
out vec4 FragColor;

in vec3 bPos;
in vec2 bTex;
in vec3 bNorm;
in float bID;
in float bLightLevel;

uniform sampler2D tex;
uniform vec3 lightPos;

void main(){
	vec2 cTex = bTex;
	
	cTex.x += mod(bID, 16);
	cTex.y += 15 - floor((bID+(1.0f/32.0f))/16.0f);
	cTex /= 16.0f;
	
	vec4 objectTexture = texture(tex, cTex);
	
	vec3 objectColor;
	objectColor.x = objectTexture.x;
	objectColor.y = objectTexture.y;
	objectColor.z = objectTexture.z;

	vec3 norm = normalize(bNorm);
	vec3 lightDir = normalize(lightPos - bPos);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	
	float ambientFactor = 0.6 + bLightLevel * 1.0/10.0;
	vec3 ambient = ambientFactor * lightColor;
	
	vec3 color = ambient * objectColor;	
	FragColor = vec4(color, objectTexture.a);
}
