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
//		- u_material 	-> The material.
//		- u_numlights 	-> Numbers of light sources.
//		- u_lights		-> The lights array.
//		- u_ambient 	-> The global light ambience.
//	- Formula for this model is as follows:
//		- ```color = KaIa + Kd { SUM j=1..ls, (N * Lj)Ij }```
//		- where, Ka is the material's ambient reflectance (vec3);
//		- Ia is the is the global ambient light (vec3);
//		- Kd is the material's diffuse reflectance (vec3);
//		- ls is the number of light sources (uint);
//		- N is the surface normal (vec3, normalized/unit vector);
//		- Lj is the jth light source's direction (vec3, normalized/unit vector);
//		- Ij is the jth light source's intensity (float);

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 32
#endif
#version 330 core

out vec4 FragColor;

in vec3 i_ViewPos;
in vec3 i_FragPos;
in vec3 i_Normal;

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
	vec3 	position;
	float 	intensity;
};

uniform mtl_t 	u_material;
uniform uint 	u_numlights;
uniform light_t u_lights[MAX_LIGHTS];
uniform vec3 	u_ambient;

// Lambertian diffuse shading. Li is light intensity
vec3 diffuse(in vec3 N, in vec3 L, in float Li) {
	float diffuse = clamp(dot(N, L), 0, 1);
	return u_material.diffuse * Li * u_material.diffuse;
}

void main() {
	i_Normal = normalize(i_Normal);
	vec3 view_dir = normalize(i_ViewPos - i_FragPos);
    // Ka * Ia (ambient component)
	vec3 ambient = u_material.ambient * u_ambient;
	
	// { SUM j=1..ls, (N * Lj)Ij }
	float diffuse_sum = 0.0;
	for (uint i = 0; i < u_numlights; i++) {
		diffuse_sum += diffuse(
			i_Normal, 
			normalize(lights[i].position - FragPos), 
			lights[i].intensity);
	}
	// Kd * { SUM }
	vec3 diffuse = u_material.diffuse * diffuse_sum;
	
	FragColor = vec4(ambient + diffuse, 1.0);
}