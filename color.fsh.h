#version 330

uniform vec4 color;

out vec4 outputColor;
//in float distance
void main(void)
{
    outputColor = color;
}