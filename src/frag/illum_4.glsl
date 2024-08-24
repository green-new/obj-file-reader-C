// Author: green
// Date: 8/24/24
// Description: .mtl format specification - Illumination model 4 "illum 4"
// Specifications:
//	- Intended to be used for deferred rendering - can be used elsewhere however
// 	- 'MAX_LIGHTS' is 32 - increase if needed
// 	- 'mtl_t', 'light_t' only contain as much data as needed to compute the 
//		illumination model
//	- Input is ['i_FragPos', 'i_Normal', 'i_ViewPos', 'i_reflMapIntensity']
// 	- Output is ['FragColor']
// 	- Uniforms/Requirements:
//		- u_material 	-> The material.
//		- u_numlights 	-> Numbers of light sources.
//		- u_lights		-> The lights array.
//		- u_ambient 	-> The global light ambience.
//	- Formula for this model is as follows:
//		- ```color = KaIa
// 			+ Kd { SUM j=1..ls, (N*Lj)Ij }
// 			+ Ks ({ SUM j=1..ls, ((H*Hj)^Ns)Ij } + Ir)```
//		- where, Ka is the material's ambient reflectance (vec3);
//		- Ia is the is the global ambient light (vec3);
//		- Kd is the material's diffuse reflectance (vec3);
//		- ls is the number of light sources (uint);
//		- N is the surface normal (vec3, normalized/unit vector);
//		- Lj is the jth light source's direction (vec3, normalized/unit vector);
//		- Ij is the jth light source's intensity (float);
//		- Ks is the material's specular reflectance;
// 		- H is the unit bisector between the camera's view vector and the 
// 			light's	direction (vec3, normalized/unit vector);
//		- Hj is the unit bisector between the camera's view vector and the jth
// 			light's direction (vec3, normalized/unit vector); and
//		- Ns is the material's specular exponent (uint);

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 32
#endif
#version 330 core

out vec4 FragColor;

in vec3 i_ViewPos;
in vec3 i_FragPos;
in vec3 i_Normal;
in float i_reflMapIntensity;

// struct mtl_t
// Contains: ambient color, diffuse color, specular color, and specular 
// exponent.
struct mtl_t {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	uint specular_exp;
};

// struct light_t
// Contains: position and intensity.
struct light_t {
	vec3 	position;
	float 	intensity;
};

uniform mtl_t 	u_material;
uniform uint 	u_numlights;
uniform light_t u_lights[MAX_LIGHTS];
uniform vec3 	u_ambient;

// Blinn-Phong model
vec3 specular(in vec3 N, in vec3 L, in vec3 V, in float Li) {
	float specular = 0.0;
	// Could just be a clamp, test this later @todo
	if (dot(N, L) > 0.0) {
		vec3 H = normalize(L + V);
		specular = dot(N, H);
		for (uint i = 0; i < u_material.specular_exp; i++) {
			specular *= specular;
		}
	}
	return u_material.specular * Li * specular;
}

// Lambertian diffuse shading. Li is light intensity
vec3 diffuse(in vec3 N, in vec3 L, in float Li) {
	float diffuse = clamp(dot(N, L), 0, 1);
	return u_material.diffuse * Li * u_material.diffuse;
}

void main() {
	Normal = normalize(Normal);
	vec3 view_dir = normalize(ViewPos - FragPos);
    // Ka * Ia (ambient component)
	vec3 ambient = material.ambient * g_ambient;
	// { SUM j=1..ls, (N * Lj)Ij }
	float diffuse_sum = 0.0;
	for (uint i = 0; i < lights_size; i++) {
		diffuse_sum += diffuse(
			i_Normal, 
			normalize(lights[i].position - FragPos), 
			lights[i].intensity);
	}
	// Kd * { SUM }
	vec3 diffuse = material.diffuse * diffuse_sum;
	// { SUM j=1..ls, ((H*Hj)^Ns)Ij }
	float specular_sum = 0.0;
	for (uint i = 0; i < lights_size; i++) {
		specular_sum += specular(
			i_Normal, 
			normalize(u_lights[i].position - FragPos), 
			normalize(view_dir),
			u_lights[i].intensity);
	}
	// Ks * { SUM }
	vec3 specular = material.specular * specular_sum;
	specular += i_reflMapIntensity;
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}