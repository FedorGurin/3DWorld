#version 330

layout (location = 0) in vec2 position;

//uniform mat4 modelViewProjectionMatrix;
//uniform int;
//uniform float;
//uniform vec2;

//out vec4 gl_Position;
//out float distance;

void main(void)
{
    gl_Position = vec4(position, 0, 1);
}