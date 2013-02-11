attribute vec4 in_Vertex;
varying vec4 TexCoord0;

void main(void)
{
    // pass vertex position and generate the texture coordinate
    gl_Position = gl_Vertex;
    TexCoord0 = gl_Vertex / 2.0 + 0.5;
}
