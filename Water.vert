uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 light_pos;

varying vec2 skyCoords;
varying vec2 texCoords;
varying vec3 normal;
varying vec3 eye;
varying vec3 light_dir;


void main(void)
{
    // get the coordinates for the mesh
    texCoords = gl_MultiTexCoord0.st;
    // read in the texture value
    float tex = texture2D(texture2,texCoords).a;


    // get the coordinates for the clouds
    skyCoords =  ((gl_Vertex.xy + 3000.0)/6000.0);


     // calculate the normal
    // normal = gl_NormalMatrix * cross(vector1,vector2);

    // generate adjusted position
    vec4 pos = gl_Vertex;
    // float offset = tex;
    pos.z += tex * 3.0;

    // generate view vector
    eye = -vec3(gl_ModelViewMatrix * pos);

    // generate light vector
    light_dir = vec3(gl_ModelViewMatrix * vec4(light_pos,0.0));

    // generate the clip space coordinate
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
