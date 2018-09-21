#version 430 core

//mat4x4 matrix = mat4(1);

in layout(location=0) vec3 position;
in layout(location=1) vec4 color;
uniform layout(location=2) float value;
uniform layout(location=3) mat4x4 matrix;

out vec4 colorV;

void main()
{
    //matrix[1][1] = value;
    gl_Position = matrix*vec4(position, 1.0f);
    colorV = color;
}
