// Author: green
// Date: 8/23/24
// Description: .mtl format specification - Illumination model 1 "illum 1"
// Specifications:
//	- Intended to be used for deferred rendering - can be used elsewhere however
// 	- 'MAX_LIGHTS' is 32 - increase if needed
// 	- 'mtl_t', 'light_t' only contain as much data as needed to compute the 
//		illumination model
//	- Input is ['FragPos', 'Normal']
// 	- Output is ['FragColor']
// 	- Uniforms/Requirements:
//		- mtl_t material 				-> The material.
//		- uint lights_size 				-> Numbers of light sources.
//		- light_t[MAX_LIGHTS] lights	-> The lights array.
//		- vec3 g_ambient 				-> The global light ambience.
//	- Formula for this model is as follows:
//		- ```color = KaIa + Kd { SUM j=1..ls, (N * Lj)Ij }```
//		- where, Ka is the material's ambient reflectance (vec3);
//		- Ia is the is the global ambient light (vec3);
//		- Kd is the material's diffuse reflectance (vec3);
//		- ls is the number of light sources (uint);
//		- N is the surface normal (vec3, normalized/unit vector);
//		- Lj is the jth light source's direction (vec3, normalized/unit vector);
// 			and,
//		- Ij is the jth light source's intensity (float).

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 32
#endif
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

// struct mtl_t
// Contains: ambient color and diffuse color.
struct mtl_t {
	vec3 ambient;
	vec3 diffuse;
};

// struct light_t
// Contains: position, diffuse color, normalized direction, and scalar 
// intensity.
struct light_t {
	vec3 position;
	vec3 diffuse;
	vec3 direction;
	float intensity;
};

uniform mtl_t material;
uniform uint lights_size;
uniform light_t lights[MAX_LIGHTS];
uniform vec3 g_ambient;

void main() {
    // Ka * Ia (ambient component)
	vec3 diffuse_constant = material.ambient * g_ambient;
	Normal = normalize(Normal);
	
	// { SUM j=1..ls, (N * Lj)Ij }
	float sum = 0.0;
	for (uint i = 0; i < lights_size; i++) {
		sum += dot(Normal, lights[i].direction) * lights[i].intensity;
	}
	// Kd * { SUM }
	vec3 shading = material.diffuse * sum;
	
	return vec4(diffuse_constant + shading, 1.0);
}