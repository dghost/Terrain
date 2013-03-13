uniform sampler2D texture1;
uniform vec3 light_pos;

varying vec2 skyCoords;
varying vec2 texCoords;
varying vec3 light_dir;




void main(void)
{
    texCoords = gl_MultiTexCoord0.st;
    skyCoords =  ((gl_Vertex.xy  + 3000.0 ) / 6000.0);


    vec4 pos = gl_Vertex;
    light_dir = vec3(gl_ModelViewMatrix * vec4(light_pos,0.0));
    float offset = texture2D(texture1,texCoords).x;
    float zPos = (offset * offset + offset) * 125.0;
    pos.z += zPos;
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
