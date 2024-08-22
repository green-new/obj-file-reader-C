/**
 * @file light.h
 * @author green
 * @date 8/20/2024
 * @brief Light data structure.
 * Header file containing the definitions of lights and their use with the 
 * .mtl file format.
 * @see http://paulbourke.net/dataformats/mtl/
 */

#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include "vec.h"
#include "color.h"
 
typedef struct {
	vec_t position;
	vec_t normal;
	color_t color;
	float intensity;
} light_t;

#ifndef ILLUM_H_INCLUDED
#define ILLUM_H_INCLUDED