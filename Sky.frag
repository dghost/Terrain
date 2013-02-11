varying vec3 texCoords;
uniform float in_Time;
uniform vec2 in_Offsets;
uniform sampler2D texture0;


vec4 light_sky = vec4(0.0,102.0,255.0,255.0);
vec4 dark_sky = vec4(0.0,0.0,255.0,255.0);
vec4 diff_sky = dark_sky - light_sky;

void main(void)
{

    // read the cloud value
    float value = smoothstep(-1.0,1.0,texture2D(texture0,texCoords.xy).r);

    // normalize the height of the sphere
    vec3 r = texCoords * 2.0 - 1.0;

    // never black out the bottom;
    r.z = abs(r.z);

    vec4 color = light_sky;
    if (r.z > 0.6)
    {
        // blend the blue color
        color += diff_sky * smoothstep(0.6,1.0,r.z);
        color /= 255.0;

        // blend the clouds in
        color = vec4(0.96,0.96,0.96,1.0) * (value) + color * (1.0 - value);
    } else if (r.z >= 0.0)
    {
        // blend clouds into light blue
        color /= 255.0;
        color = vec4(0.96,0.96,0.96,1.0) * (value) + color * (1.0 - value);
    } else
    {
        // black out the rest
        color = vec4(0.0);
    }


    gl_FragColor = color;

}
