#version 400

in vec3 inVertex;
in vec2 inTexCoord;

//out vec4 controlMVP;
out vec3 controlPoint;
out vec2 controlTexCoords;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform sampler2D groundTexture;

void main(void)
{
    vec3 pos = inVertex;
    float offset = texture(groundTexture,inTexCoord).a;
    //float zPos = offset * 125.0;
    float zPos = offset * 125.0;
    pos.z += zPos;

  //  controlMVP = projMatrix * viewMatrix * pos;
    controlPoint = pos;
    controlTexCoords = inTexCoord;
}
