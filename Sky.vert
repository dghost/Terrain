varying vec3 texCoords;

void main(void)
{
    // scale the unit sphere
    vec4 pos = gl_Vertex;
    pos.z *= 1000.0;
    pos.xy *= 3000.0;
    pos.z -= 600.0;

    // write final position
    gl_Position = gl_ModelViewProjectionMatrix * pos;

    // generate texture coordinates
    texCoords = (gl_Vertex.xyz / gl_Vertex.w + 1.0) * 0.5;
}
