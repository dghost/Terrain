varying vec2 texCoords;
varying vec2 skyCoords;
//varying vec3 normal;
varying vec3 eye;
varying vec3 light_dir;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

vec3 light_blue = vec3(50.0,50.0,185.0);

const float R0 = ((1.0 - 1.333) / (1.0 + 1.333)) * ((1.0 - 1.333) / (1.0 + 1.333));

void main(void)
{


    // value is 1 if no cloud, 0 if cloud
    float value = 1.0 - smoothstep(-1.0, 1.0 ,texture2D(texture0,skyCoords).r) * 0.5;
    vec3 n =  texture2D(texture2,texCoords).rgb;

    // calculate the normal
    vec3 normal = gl_NormalMatrix * n;
    // normalize vectors for per pixel lighting
    vec3 L = normalize(light_dir);
    vec3 N = normalize(normal);


    vec3 color = light_blue / 255.0;
    // generate the diffuse component
    float diffuse = clamp(dot(N,L),0.0,1.0);
    // color *= (0.25 + diffuse * value * 1.0);
    float specular = 0.0;
    // generate the specular component


    float fresnel = R0 + (1.0 - R0) * pow(1.0 - dot(N,L), 5.0);
    vec3 E = normalize(eye);
    vec3 R = reflect(-L, N);
    specular = pow( max(dot(R, E), 0.0),
                    15.0 + 25.0 * (value));

    vec3 light = mix (color.rgb * diffuse, vec3(specular), fresnel);
    color *= 0.25 + light * value;



    // write the final color
    // Ambient light is 0.4, specular and diffuse are multiplied by value indicating if cloud is overhead or not
    gl_FragColor.rgb = color;
    // set the transparency based on how much ambient light is hitting it
    gl_FragColor.a = (0.6 - max((value - fresnel) * 0.2,0.0));

    //   gl_FragColor = texture2D(texture2,texCoords);
}
