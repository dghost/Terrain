#version 400

in vec3 inVertex;
in vec2 inTexCoord;

out vec3 controlPoint;
out vec2 controlTexCoords;


void main(void)
{
    controlPoint = inVertex;
    controlTexCoords = inTexCoord;
}
