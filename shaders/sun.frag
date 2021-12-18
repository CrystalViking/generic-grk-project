#version 430 core

uniform sampler2D colorTexture;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 vecPosition;
in vec2 vecTexCoord;

void main()
{	
	vec4 textureColor = texture2D(colorTexture, vecTexCoord);
	vec3 objectColor = textureColor.xyz;
	
	vec3 normal = normalize(interpNormal);
	vec3 V = normalize(cameraPos - vecPosition);
	vec3 lightDir = normalize(vecPosition - cameraPos);
	
	float diffuse = max(dot(normal, -lightDir), 0.0);
	

	// red outline? :
	gl_FragColor = vec4(objectColor, 1.0);
}
