//#version 330

//layout (location = 0) in vec2 position;


//void main(void)
//{
//    gl_Position = vec4(position, 0, 1);
//}


#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

smooth out vec4 theColor;

void main()
{
	gl_Position = position;
	theColor = color;
}