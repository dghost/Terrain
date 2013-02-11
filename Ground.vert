uniform sampler2D texture1;

varying vec2 skyCoords;
varying vec2 texCoords;
varying vec3 normal;
varying vec3 light_dir;


vec3 light_pos = vec3(800.0,900.0,500.0);

void main(void)
{
    texCoords = gl_MultiTexCoord0.st;
    skyCoords =  ((gl_Vertex.xy  + 3000.0 ) / 6000.0);
/*
    vec3 neigh0 = gl_Vertex.xyz + vec3(1.0,0.0,0.0);
    vec3 neigh1 = gl_Vertex.xyz - vec3(1.0,0.0,0.0);
    vec3 neigh2 = gl_Vertex.xyz + vec3(0.0,1.0,0.0);
    vec3 neigh3 = gl_Vertex.xyz - vec3(0.0,1.0,0.0);
*/

    vec2 nCoords = texCoords + vec2(0.25/512.0,0.000);
    vec2 sCoords = texCoords - vec2(0.25/512.0,0.000);
    vec2 eCoords = texCoords + vec2(0.0000,0.25/512.0);
    vec2 wCoords = texCoords - vec2(0.0000,0.25/512.0);

    vec4 t;
    t.x = texture2D(texture1,nCoords).r;
    t.y = texture2D(texture1,sCoords).r;
    t.z = texture2D(texture1,eCoords).r;
    t.w = texture2D(texture1,wCoords).r;

      t = t * t + t;

    float nsDiff = t.x - t.y;
    float ewDiff = t.z - t.w;
    vec3 vector1 = vec3(2.0,0.0,nsDiff * 125.0);
    vec3 vector2 = vec3(0.0,2.0,ewDiff * 125.0);


    normal = gl_NormalMatrix * cross(vector1,vector2);
    vec4 pos = gl_Vertex;
    light_dir = vec3(gl_ModelViewMatrix * vec4(light_pos,0.0));
    float offset = texture2D(texture1,texCoords).x;
    float zPos = (offset * offset + offset) * 125.0;
    pos.z += zPos;
    gl_Position = gl_ModelViewProjectionMatrix * pos;
}
