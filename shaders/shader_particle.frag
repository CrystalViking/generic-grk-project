#version 330 core

in float lifetime;

out vec4 gl_FragColor;

void main()
{
   float lifetimeNormalize = lifetime/2;
   gl_FragColor = vec4(0.0f, lifetimeNormalize, 1.0f, lifetimeNormalize);
};