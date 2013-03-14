#version 130

in vec3 inVertex;
in vec3 inNormal;
in vec2 inTexCoord;


uniform sampler2D texture1;

uniform vec3 light_pos;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 skyCoords;
out vec2 texCoords;
out vec3 light_dir;




void main(void)
{
    texCoords = inTexCoord;
    skyCoords =  ((inVertex.xy  + 3000.0 ) / 6000.0);


    vec4 pos = vec4(inVertex,1.0);
    light_dir = vec3(viewMatrix * vec4(light_pos,0.0));
    float offset = texture2D(texture1,inTexCoord).x;
    float zPos = (offset * offset + offset) * 125.0;
    pos.z += zPos;
    gl_Position = projMatrix * viewMatrix * pos;
}
