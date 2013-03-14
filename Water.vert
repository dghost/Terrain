in vec3 inVertex;
in vec3 inNormal;
in vec2 inTexCoord;



uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 light_pos;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 skyCoords;
out vec2 texCoords;
out vec3 normal;
out vec3 eye;
out vec3 light_dir;


void main(void)
{
    // get the coordinates for the mesh
    texCoords = inTexCoord;
    // read in the texture value
    float tex = texture2D(texture2,texCoords).a;


    // get the coordinates for the clouds
    skyCoords =  ((inVertex.xy + 3000.0)/6000.0);


     // calculate the normal
    // normal = gl_NormalMatrix * cross(vector1,vector2);

    // generate adjusted position
    vec4 pos = vec4(inVertex,1.0);
    // float offset = tex;
    pos.z += tex * 3.0;

    // generate view vector
    eye = -vec3(viewMatrix * pos);

    // generate light vector
    light_dir = vec3(viewMatrix * vec4(light_pos,0.0));

    // generate the clip space coordinate
    gl_Position = projMatrix * viewMatrix * pos;
}
