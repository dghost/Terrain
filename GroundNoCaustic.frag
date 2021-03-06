#version 130

in vec2 texCoords;
in vec2 skyCoords;
in vec3 light_dir;

uniform sampler2D cloudTexture;
uniform sampler2D groundTexture;
uniform mat4 viewMatrix;

out vec4 outColor;

vec4 green = vec4(34.0,139.0,34.0,255.0);
vec4 dark_green = vec4(0.0,100.0,0.0,255.0);
vec4 brown = vec4(210.0,180.0,140.0,255.0);
vec4 white = vec4(245.0,245.0,245.0,255.0);
vec4 light_blue = vec4(0.0,0.0,200.0,255.0);
vec4 dark_blue = vec4(0.0,0.0,112.0,255.0);


void main(void)
{

    // read in offset

    float displacement = texture2D(groundTexture,texCoords).a;
    //displacement = displacement * displacement + displacement;
    displacement *= 125.0;

    float cloudRaw = smoothstep(-1.0,1.0,texture2D(cloudTexture,skyCoords).r);
    float cloudWeight = 1.0 - cloudRaw * 0.5;


    vec4 color;

    if (displacement >= 175.0)
    {
        // blend between green and mountain tops
        vec4 diff = white - dark_green;
        color = dark_green + diff * smoothstep(175.0,225.0,displacement);
    } else if (displacement > 50.0) {
        vec4 diff = dark_green - green;
        color = green + diff * smoothstep(50.0,175.0,displacement);
    } else {
        vec4 diff = green - brown;
        color = brown + diff * smoothstep(-50.0,50.0,displacement);
    }
    color /= 255.0;

    vec3 normal = mat3(viewMatrix) * texture2D(groundTexture,texCoords).rgb;
    vec3 L = normalize(light_dir);
    vec3 N = normalize(normal);
    float diffuse = max(dot(N,L),0.0);

    outColor = color * 0.25 + color * diffuse * cloudWeight;

}
