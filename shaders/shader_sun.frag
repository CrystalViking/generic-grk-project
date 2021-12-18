#version 430 core

uniform vec3 objectColor;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 vecPosition;

void main()
{	
	
	vec3 normal = normalize(interpNormal);
	vec3 V = normalize(cameraPos - vecPosition);
	vec3 lightDir = normalize(vecPosition - cameraPos);
	
	float diffuse = max(dot(normal, -lightDir), 0.0);

	// red outline? :
	gl_FragColor = vec4(objectColor * (diffuse + vec3(0.9, 0.2, 0.0)), 1.0);
}

