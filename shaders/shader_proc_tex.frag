#version 430 core

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 vecPosition;
in vec3 vecProc;

void main()
{	
	vec3 color1 = vec3(0.0, 0.8, 0.9);
	vec3 color0 = vec3(0.9, 0.8, 0.0);

	float temp = (1+ sin(3.14 * vecProc.x / 0.5)) / 2;

	vec3 Color = (1-temp) * color0 + temp * color1;
	vec3 normal = normalize(interpNormal);
	vec3 V = normalize(cameraPos - vecPosition);
	vec3 lightDir = normalize(vecPosition - lightPos);
	
	float diffuse = max(dot(normal, -lightDir), 0.0);
	vec3 R = reflect(lightDir, normal);
	float specular = pow(max(dot(V, R), 0.0), 2.0);
	
	gl_FragColor = vec4(Color * diffuse + Color * specular, 1.0);
}
