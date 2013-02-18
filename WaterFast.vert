uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
varying vec2 skyCoords;
varying vec2 texCoords;
varying vec3 normal;
varying vec3 eye;
varying vec3 light_dir;

vec3 light_pos = vec3(800.0,900.0,500.0);

void main(void)
{
    // get the coordinates for the mesh
    texCoords = gl_MultiTexCoord0.st;
    // read in the texture value
    float tex = texture2D(texture2,texCoords).r;
    float ground = texture2D(texture1,texCoords).r;

    ground = ground * ground + ground;
    ground = max(0.15 - ground,0.0);
    ground *= ground;

    // get the coordinates for the clouds
    skyCoords =  ((gl_Vertex.xy + 3000.0)/6000.0);


    // calculate four adjacent vertices
    vec2 nCoords = texCoords + vec2(0.5/512.0,0.000);
    vec2 sCoords = texCoords - vec2(0.5/512.0,0.000);
    vec2 eCoords = texCoords + vec2(0.0000,0.5/512.0);
    vec2 wCoords = texCoords - vec2(0.0000,0.5/512.0);

    float nsDiff = texture2D(texture2,nCoords).r  - texture2D(texture2,sCoords).r ;
    float ewDiff = texture2D(texture2,eCoords).r  - texture2D(texture2,wCoords).r;
    vec3 vector1 = vec3(4.0,0.0,nsDiff * 10.0 * ground);
    vec3 vector2 = vec3(0.0,4.0,ewDiff * 10.0 * ground);

    // calculate the normal
    normal = gl_NormalMatrix * cross(vector1,vector2);

    // generate adjusted position
    vec4 pos = gl_Vertex;
    // float offset = tex;
    pos.z += tex * 10.0 * ground;

    // generate view vector
    eye = -vec3(gl_ModelViewMatrix * pos);

    // generate light vector
    light_dir = vec3(gl_ModelViewMatrix * vec4(light_pos,0.0));

    // generate the clip space coordinate
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
