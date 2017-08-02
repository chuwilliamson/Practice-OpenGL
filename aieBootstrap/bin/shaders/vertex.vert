#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 colour;
out vec4 vColour;
uniform mat4 mvp;
uniform float time;

void main()
{
	int bob = 3;
	float dylan;
	vColour = colour;
	vec4 newpos = position;
	
	gl_Position =  mvp * newpos;
}