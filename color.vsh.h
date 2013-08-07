//#version 330

//layout (location = 0) in vec2 position;


//void main(void)
//{
//    gl_Position = vec4(position, 0, 1);
//}


#version 330

layout (location = 0) in vec4 position;
in vec2 offset;

void main()
{
	vec4 totalOffset = vec4(offset.x, offset.y, 0.0, 0.0);
    gl_Position = position + totalOffset;
}