#version 130

in vec3 texCoords;
uniform float in_Time;
uniform vec2 in_Offsets;
uniform sampler2D texture0;

out vec4 outColor;

vec4 light_sky = vec4(0.0,102.0,255.0,255.0);
vec4 dark_sky = vec4(0.0,0.0,255.0,255.0);
vec4 diff_sky = dark_sky - light_sky;

void main(void)
{

    // read the cloud value
    float value = smoothstep(-1.0,1.0,texture2D(texture0,texCoords.xy).r);

    float r = texCoords.z * 2.0 - 1.0;

    // never black out the bottom;
    r = abs(r);

    vec4 color = light_sky;
    if (r > 0.6)
    {
        // blend the blue color
        color += diff_sky * smoothstep(0.6,1.0,r);
    }

    color /= 255.0;
    color = vec4(0.96,0.96,0.96,1.0) * (value) + color * (1.0 - value);
    outColor = color;

}
