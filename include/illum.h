/**
 * @file illum.h
 * @author green
 * @date 7/30/2024
 * @brief Enumeration of the 10 "illum" types specified by the .mtl standard.
 * Header file containing the definitions of mathematical functions and enums
 * for the 10 illum types provided by the .mtl and .obj standard.
 * @see http://paulbourke.net/dataformats/mtl/
 */

#ifndef ILLUM_H_INCLUDED
#define ILLUM_H_INCLUDED

#include "mtl.h"
#include "color.h"

enum illum_t {
	CONSTANT_COLOR = 0,
	DIFFUSE_LAMBERTIAN = 1,
	DIFFUSE_SPECULAR_LAMBERTIAN = 2,
	DIFFUSE_SPECULAR_LREFLECTION = 3,
	DIFFUSE_SPECULAR_GLASS = 4,
	DIFFUSE_SPECULAR_FRESNEL = 5,
	DIFFUSE_SPECULAR_REFRACT = 6,
	DIFFUSE_SPECULAR_REFRACT_FRESNEL = 7,
	DIFFUSE_SPECULAR_REFLECTION_NORAYTRACE = 8,
	DIFFUSE_SPECULAR_GLASS_NORAYTRACE = 9,
	SHADOWMATTE = 10
};

// Enumerates the 0 - 10 possible illumination models.
// Stores the mathematical equations that "represent various material lighting 
// and shading effects".

void
illum_0(mtl_t* mat, float* rgb);

void
illum_1(mtl_t* mat, float* rgb);

#endif