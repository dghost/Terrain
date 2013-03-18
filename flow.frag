#version 130
uniform float in_Time;
uniform vec2 in_Offsets;
uniform vec2 in_texelSize;
uniform vec2 in_sideLength;

in vec4 TexCoord0;

out vec4 outColor;

// GLSL implementation of 2D "flow noise" as presented
// by Ken Perlin and Fabrice Neyret at Siggraph 2001.
// (2D simplex noise with analytic derivatives and
// in-plane rotation of generating gradients,
// in a fractal sum where higher frequencies are
// displaced (advected) by lower frequencies in the
// direction of their gradient. For details, please
// refer to the 2001 paper "Flow Noise" by Perlin and Neyret.)
//
// Author: Stefan Gustavson (stefan.gustavson@liu.se)
// Distributed under the terms of the MIT license.

// Helper constants
#define F2 0.366025403
#define G2 0.211324865
#define K 0.0243902439 // 1/41

// Permutation polynomial
float permute(float x) {
  return mod((34.0 * x + 1.0)*x, 289.0);
}

// Gradient mapping with an extra rotation.
vec2 grad2(vec2 p, float rot) {
#if 1
// Map from a line to a diamond such that a shift maps to a rotation.
  float u = permute(permute(p.x) + p.y) * K + rot; // Rotate by shift
  u = 4.0 * fract(u) - 2.0;
  return vec2(abs(u)-1.0, abs(abs(u+1.0)-2.0)-1.0);
#else
#define TWOPI 6.28318530718
// For more isotropic gradients, sin/cos can be used instead.
  float u = permute(permute(p.x) + p.y) * K + rot; // Rotate by shift
  u = fract(u) * TWOPI;
  return vec2(cos(u), sin(u));
#endif
}

float srdnoise(in vec2 P, in float rot, out vec2 grad) {

  // Transform input point to the skewed simplex grid
  vec2 Ps = P + dot(P, vec2(F2));

  // Round down to simplex origin
  vec2 Pi = floor(Ps);

  // Transform simplex origin back to (x,y) system
  vec2 P0 = Pi - dot(Pi, vec2(G2));

  // Find (x,y) offsets from simplex origin to first corner
  vec2 v0 = P - P0;

  // Pick (+x, +y) or (+y, +x) increment sequence
  vec2 i1 = (v0.x > v0.y) ? vec2(1.0, 0.0) : vec2 (0.0, 1.0);

  // Determine the offsets for the other two corners
  vec2 v1 = v0 - i1 + G2;
  vec2 v2 = v0 - 1.0 + 2.0 * G2;

  // Wrap coordinates at 289 to avoid float precision problems
  Pi = mod(Pi, 289.0);

  // Calculate the circularly symmetric part of each noise wiggle
  vec3 t = max(0.5 - vec3(dot(v0,v0), dot(v1,v1), dot(v2,v2)), 0.0);
  vec3 t2 = t*t;
  vec3 t4 = t2*t2;

  // Calculate the gradients for the three corners
  vec2 g0 = grad2(Pi, rot);
  vec2 g1 = grad2(Pi + i1, rot);
  vec2 g2 = grad2(Pi + 1.0, rot);

  // Compute noise contributions from each corner
  vec3 gv = vec3(dot(g0,v0), dot(g1,v1), dot(g2,v2)); // ramp: g dot v
  vec3 n = t4 * gv;  // Circular kernel times linear ramp

  // Compute partial derivatives in x and y
  vec3 temp = t2 * t * gv;
  vec3 gradx = temp * vec3(v0.x, v1.x, v2.x);
  vec3 grady = temp * vec3(v0.y, v1.y, v2.y);
  grad.x = -8.0 * (gradx.x + gradx.y + gradx.z);
  grad.y = -8.0 * (grady.x + grady.y + grady.z);
  grad.x += dot(t4, vec3(g0.x, g1.x, g2.x));
  grad.y += dot(t4, vec3(g0.y, g1.y, g2.y));
  grad *= 40.0;

  // Add contributions from the three corners and return
  return 40.0 * (n.x + n.y + n.z);
}

// Description : Array and textureless GLSL 2D/3D/4D simplex
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  {
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i);
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
  }
/*
float flow(vec2 p, float time)
{
     vec2 g1, g2;
    float n1 = srdnoise(p*0.5, 0.2*time, g1);
    float n2 = srdnoise(p*2.0 + g1*0.5, 0.51*time, g2);
    float n3 = srdnoise(p*4.0 + g1*0.5 + g2*0.25, 0.77*time, g2);
    return n1+ 0.75*n2 + 0.5*n3;
}
*/

float flow(vec2 p, float time)
{

    float result = snoise(vec3(p * 4.0,time)) * 0.25;
    result += snoise(vec3(p * 8.0,time)) * 0.125;
    //result += snoise(vec3(p * 16.0,time)) * 0.0625;

    vec2 g1, g2;
   float n1 = srdnoise(p*0.5, 0.2*time, g1);
   float n2 = srdnoise(p*2.0 + g1*0.5, 0.51*time, g2);
   float n3 = srdnoise(p*4.0 + g1*0.5 + g2*0.25, 0.77*time, g2);
    result +=  n1+ 0.75*n2 + 0.5*n3;

    return result;
}

void main(void)
{

    float time = in_Time * 2.0;


    // scale the texture coordinates for better noise

    vec2 texCoords = TexCoord0.xy * 2.0;
    // generate harmonics
   // float n = flow(TexCoord0.xy,time);



    // calculate four adjacent vertices
    vec2 nCoords = texCoords + vec2(in_texelSize.x,0.000);
    vec2 sCoords = texCoords - vec2(in_texelSize.x,0.000);
    vec2 eCoords = texCoords + vec2(0.0000,in_texelSize.y);
    vec2 wCoords = texCoords - vec2(0.0000,in_texelSize.y);

    float nsDiff = flow(nCoords,time) - flow(sCoords,time);
    float ewDiff = flow(eCoords,time) - flow(wCoords,time);
    vec3 vector1 = vec3(2.0 * in_texelSize.x * in_sideLength.x,0.0,nsDiff * 40.0);
    vec3 vector2 = vec3(0.0,2.0 * in_texelSize.y * in_sideLength.y,ewDiff * 40.0);


    outColor.rgb = normalize(cross(vector1,vector2));
    outColor.a = flow(texCoords,time);
}
