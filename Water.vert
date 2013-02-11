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
    //    ground *= 125.0;
    // get the coordinates for the clouds
    skyCoords =  ((gl_Vertex.xy + 3000.0)/6000.0);


    // calculate four adjacent vertices
    vec2 nCoords = texCoords + vec2(0.25/512.0,0.000);
    vec2 sCoords = texCoords - vec2(0.25/512.0,0.000);
    vec2 eCoords = texCoords + vec2(0.0000,0.25/512.0);
    vec2 wCoords = texCoords - vec2(0.0000,0.25/512.0);

    /*
    vec3 neigh0 = gl_Vertex.xyz + vec3(1.0,0.0,0.0);
    vec3 neigh1 = gl_Vertex.xyz - vec3(1.0,0.0,0.0);
    vec3 neigh2 = gl_Vertex.xyz + vec3(0.0,1.0,0.0);
    vec3 neigh3 = gl_Vertex.xyz - vec3(0.0,1.0,0.0);

    neigh0.z += texture2D(texture2,nCoords).r * 15.0;
    neigh1.z += texture2D(texture2,sCoords).r * 15.0;
    neigh2.z += texture2D(texture2,eCoords).r * 15.0;
    neigh3.z += texture2D(texture2,wCoords).r * 15.0;

    // find the vectors between them
    vec3 vector1 = neigh0 - neigh1;
    vec3 vector2 = neigh2 - neigh3;

*/

    vec4 dampening;
    dampening.x = texture2D(texture1,nCoords).r;
    dampening.y = texture2D(texture1, sCoords).r;
    dampening.z = texture2D(texture1,eCoords).r;
    dampening.w = texture2D(texture1, wCoords).r;

    dampening = dampening * dampening + dampening;

   dampening = max(0.15 - dampening, 0.0);
   dampening *= dampening;

    float nsDiff = texture2D(texture2,nCoords).r * dampening.x - texture2D(texture2,sCoords).r * dampening.y ;
    float ewDiff = texture2D(texture2,eCoords).r * dampening.z - texture2D(texture2,wCoords).r * dampening.w ;
    vec3 vector1 = vec3(2.0,0.0,nsDiff * 20.0);
    vec3 vector2 = vec3(0.0,2.0,ewDiff * 20.0);

    // calculate the normal
    normal = gl_NormalMatrix * cross(vector1,vector2);

    // generate adjusted position
    vec4 pos = gl_Vertex;
    // float offset = tex;
    pos.z += tex * 20.0 * ground;

    // generate view vector
    eye = -vec3(gl_ModelViewMatrix * pos);

    // generate light vector
    light_dir = vec3(gl_ModelViewMatrix * vec4(light_pos,0.0));

    // generate the clip space coordinate
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
