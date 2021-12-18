#version 430 core

uniform sampler2D colorTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 spotlightPos;
uniform vec3 spotlightDir;
uniform float spotlightCutOff;
uniform float spotlightOuterCutOff;

in vec3 interpNormal;
in vec3 vecPosition;
in vec2 vecTexCoord;

vec3 spotLight(vec3 objectColor, vec3 normal, vec3 V){
	vec3 Dir = normalize(spotlightPos - vecPosition);
	float theta = dot(Dir, normalize(-spotlightDir));
	float epsilon   = spotlightCutOff - spotlightOuterCutOff;
	float intensity = clamp((theta - spotlightOuterCutOff) / epsilon, 0.0, 1.0); 
	float diffuse = max(dot(normal, Dir), 0.0);
	vec3 R = reflect(-Dir, normal);
	float specular = pow(max(dot(V, R), 0.0), 4.0);
	vec3 Color = vec3(objectColor * diffuse *intensity+ vec3(1.0) * specular *intensity);
	return(Color);
}

vec3 sun(vec3 objectColor, vec3 normal, vec3 V){
	vec3 lightDir = normalize(vecPosition - lightPos);
	float diffuse = max(dot(normal, -lightDir), 0.0);
	vec3 R = reflect(lightDir, normal);
	float specular = pow(max(dot(V, R), 0.0), 4.0);
	
	vec3 Color = vec3(objectColor * diffuse + vec3(1.0) * specular);
	return(Color);
}

void main()
{	
	vec4 textureColor = texture2D(colorTexture, vecTexCoord);
	vec3 objectColor = textureColor.xyz;
	vec3 normal = normalize(interpNormal);
	vec3 V = normalize(cameraPos - vecPosition);
	gl_FragColor = vec4(spotLight(objectColor, normal, V) + sun(objectColor, normal, V), 1.0);
}
