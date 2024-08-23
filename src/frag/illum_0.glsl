// Author: green
// Date: 8/23/24
// Description: .mtl format specification - Illumination model 0 "illum 0". The
// constant color illumination model
// Specifications:
//	- Intended to be used for deferred rendering - can be used elsewhere however
//	- Input is []
// 	- Output is ['FragColor']
// 	- Uniforms/Requirements:
//		- vec3 diffuse -> The material's diffuse color.
//	- Formula for this model is as follows:
//		- ```color = Kd```
//		- where, Kd is the material's diffuse reflectance.
#version 330 core

out vec4 FragColor;

uniform vec3 diffuse;

void main() {
    FragColor = vec4(diffuse, 1.0);
}