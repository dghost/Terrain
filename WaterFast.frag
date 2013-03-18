#version 130

in vec2 texCoords;
in vec2 skyCoords;
in vec3 eye;
in vec3 light_dir;
uniform sampler2D cloudTexture;
//uniform sampler2D groundTexture;
uniform sampler2D waterTexture;
uniform mat4 viewMatrix;

out vec4 outColor;
vec4 light_blue = vec4(50.0,50.0,185.0,150.0);

void main(void)
{


    // value is 1 if no cloud, 0 if cloud
    float value = 1.0 - smoothstep(-1.0, 1.0 ,texture2D(cloudTexture,skyCoords).r) * 0.5;

    vec3 n =  texture2D(waterTexture,texCoords).rgb;

    // calculate the normal
    vec3 normal = mat3(viewMatrix) * n;

    // normalize vectors for per pixel lighting
    vec3 L = normalize(light_dir);
    vec3 N = normalize(normal);


    vec4 color = light_blue / 255.0;
    // generate the diffuse component
    float diffuse = clamp(dot(N,L),0.0,1.0);
    color *= (0.25 + diffuse * value * 1.0);
    float specular = 0.0;
    // generate the specular component


    // write the final color
    // Ambient light is 0.4, specular and diffuse are multiplied by value indicating if cloud is overhead or not
    outColor = color;
    // set the transparency based on how much ambient light is hitting it
    outColor.a = (0.7 + specular - max(value * 0.3,0.0));
}
