attribute vec3 inVertex;

varying vec3 texCoords;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main(void)
{
    // scale the unit sphere
    vec4 pos = vec4(inVertex,1.0);
    pos.z *= 1000.0;
    pos.xy *= 3000.0;
    pos.z -= 600.0;

    // write final position
    gl_Position = projMatrix * viewMatrix * pos;

    // generate texture coordinates
    texCoords = (inVertex + 1.0) * 0.5;
}
