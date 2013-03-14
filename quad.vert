#version 130

in vec4 inVertex;
out vec4 TexCoord0;

void main(void)
{
    // pass vertex position and generate the texture coordinate
    gl_Position = inVertex;
    TexCoord0 = in_Vertex / 2.0 + 0.5;
}
