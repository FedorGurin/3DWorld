//#version 330

//layout (location = 0) in vec2 position;


//void main(void)
//{
//    gl_Position = vec4(position, 0, 1);
//}


/*#version 330

layout (location = 0) in vec4 position;
in vec2 offset;

void main()
{
	vec4 totalOffset = vec4(offset.x, offset.y, 0.0, 0.0);
    gl_Position = position + totalOffset;
}
*/


#version 330
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

smooth out vec4 theColor;

uniform vec2 offset;
uniform mat4 perspectiveMatrix;

//uniform float zNear;
//uniform float zFar;
//uniform float frustumScale;

void main()
{
    vec4 cameraPos = position + vec4(offset.x,offset.y,0.0,0.0);
    /*vec4 clipPos;

    clipPos.xy=cameraPos.xy*frustumScale;

    clipPos.z = cameraPos.z * (zNear + zFar)/(zNear-zFar);
    clipPos.z += 2 * zNear * zFar / (zNear - zFar);

    clipPos.w = -cameraPos.z;
    */
    gl_Position = perspectiveMatrix * cameraPos;
    theColor = color;
}
