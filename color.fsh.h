//#version 330

//uniform vec4 color;

//out vec4 outputColor;
//in float distance
//void main(void)
//{
//    outputColor = color;
//}

#version 330

smooth in vec4 theColor;

out vec4 outputColor;

void main()
{
	outputColor = theColor;
}

