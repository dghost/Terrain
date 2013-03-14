in vec4 in_Vertex;
out vec4 TexCoord0;

void main(void)
{
    // pass vertex position and generate the texture coordinate
    gl_Position = in_Vertex;
    TexCoord0 = in_Vertex / 2.0 + 0.5;
}
