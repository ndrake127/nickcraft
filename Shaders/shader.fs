#version 330 core
out vec4 FragColor;

in vec3 bPos;
in vec2 bTex;
in vec3 bNorm;
in float bID;

uniform sampler2D tex;
uniform vec3 lightPos;

void main(){
	vec2 cTex = bTex;
	float id = bID;
	
	cTex.x += mod(id, 16);
	cTex.y += 15 - floor((id+(1.0f/32.0f))/16.0f);
	cTex /= 16.0f;
	
	vec4 objectTexture = texture(tex, cTex);
	
	vec3 objectColor;
	objectColor.x = objectTexture.x;
	objectColor.y = objectTexture.y;
	objectColor.z = objectTexture.z;

	vec3 norm = normalize(bNorm);
	vec3 lightDir = normalize(lightPos - bPos);
	vec3 lightDir2 = normalize(vec3(-1.5, -2.0, -1.0));
	vec3 lightColor = vec3(1.0, 1.0, 1.0);

	float ambientFactor = 0.2;
	float diffuseFactor = abs(dot(norm, lightDir2));
	
	vec3 ambient = ambientFactor * lightColor;
	vec3 diffuse = diffuseFactor * lightColor;

	vec3 color = (ambient + diffuse) * objectColor;

	FragColor = vec4(color, objectTexture.a);
}
