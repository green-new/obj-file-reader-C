/**
 * @file mtl.h
 * @author green
 * @date 7/30/2024
 * @brief Definitions of material (.mtl) related structures and functions.
 * Header file containing the material structure (mtl_t), the material library
 * structure (mtllib_t), and their related functions. Imports "map.h" to create
 * a map of [name, material] values, which may impact any files that import this
 * header file.
 * @see http://paulbourke.net/dataformats/mtl/
 */
#ifndef MTL_READER_H_INCLUDED
#define MTL_READER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "utils.h"

#include "material_map.h"

/** @struct mtl_t
 * @brief Material data format as defined from
 * http://paulbourke.net/dataformats/mtl/.
 */
typedef struct {
	/** Name of this material type. */
    const char* name;
	/** Specifies the ambient reflectivity (Ka) using rgb values 
	* scaled from [0, 1]. */
    float ambient[3];
	/** Specifies the diffuse reflectivity (Kd) using rgb values scaled from 
	* [0, 1]. */
    float diffuse[3];
	/** Specifies the specular reflectivity (Ks) using rgb values scaled from 
	* [0, 1]. */
    float specular[3];
	/** Specifies the transmission filter (Tf) of this material using rgb values 
	* scaled from [0, 1]. */
    float tm_filter[3];
	/** Specifies the illumination model (illum) to use. Value between 0-10, see 
	* complete descriptions in illum.h. */
    uint32_t illum;
	/** Specifies the "dissolve" (d), or more well known as transparency, of 
	* this material. */
	struct {
		/** The dissolve or transparency value. */
		float value;	
		/** The "halo" flag behind the "d" command. Specifies that the material's 
		* transparency should be dependent on a surface's orientation with regards 
		* to the world camera. */
		int halo;
	} dissolve;				
	/** Specifies the specular exponent (Ns) for this material. */
    uint32_t specular_exponent;
	/** Specifies the sharpness of reflections (sharprness) of this material. */
    uint32_t sharpness;
	/** Specifies the optical density (Ni) for this material. Also known as 
	* "index of refraction". */
    float optical_density;
	/** Specifies that a color texture filter or color procedural texture file
	is linked to the diffuse reflectivity of the material.	*/
	struct {
		/** Name of the color texture filter or color procedural texture 
		file. */
		const char* filename;
		/** Turns texture blending in the horizontal direction on or off. 
		Default is 1 (on). */
		int blendu;
		/** Turns texture blending in the vertical direction on or off. Default
		is 1 (on). */
		int blendv;
		/** Enables color correction. Default is 0 (off). */
		int cc;
		/** Turns texture clamping on or off. If clamping is on, texture 
		coordinates are restricted to [0, 1] in the u/v/w range. Default is 
		off. */
		int clamp;
		/** Specifies the range over which scalar or color textures may vary. */
		struct {
			/** Specifies a base value to texture colors. Positive values 
			increase brightness, negative values decrease brightness. Default is
			0. */
			float base;
			/** Specifies a range of the texture values. Increasing the number
			increases contrast. Default is 1. */
			float gain;
		} mm;
		/** Offsets the position of the texture map. Default is {0, 0, 0}. Order
		is [u, v, w]. */
		float o[3];
		/** Scales the size of the texture pattern. Default is {1, 1, 1}. Order
		is [u, v, w]. */
		float s[3];
		/** Specifies and enables turbulence for textures. Intended to add 
		variance to original textures and allows simple images to be repeated
		over large areas. */
		float t[3];
		/** Specifies the resolution of the texture created when an image is 
		used. The default texture size is the largest power of 2 that does not 
		exceed the original image size. */
		struct { 
			/** The width of the image. */
			uint32_t w;
			/** The height of the image. */
			uint32_t h;
		} texres;
	} map_Ka;
	struct {
		
	} map_Ks;
	struct {
		
	} map_Ns;
	struct {
		
	} map_d;
	struct {
		
	} map_aat;
	struct {
		
	} decal;
	struct {
		
	} disp;
	/** Specifies a bump map. */
	struct {
		
	} bump;
	/** Specifies a reflection map. */
	struct {
		
	} map_refl;
} mtl_t;

/** A library of material types. Non-opaque. */
typedef struct {
	/** Name of this material library. */
    const char* name;
	/** Map of [material name, material] key/value pairs. */
    mat_map map;
} mtllib_t;

/** @brief Creates a new, empty material object.
 * @returns The material.
 */
mtl_t 
mtl_create(void);

/** @brief Destroy the material.
 * Destroys any heap allocated data in this material object, and sets all 
 * values to 0.
 * @param material The material object.
 */
void 
mtl_destroy(mtl_t* material);

/** @brief Prints the material's properties to standard output.
 * @param mat The material to print.
 */
void 
mtl_print(mtl_t* mat);

/** @brief Prints the material's properties to file. 
 * Prints the material's properties to the provided file. Assumes the file is
 * opened.
 * @param file The file pointer.
 * @param mat The material to print.
 * @return void
 */
void 
mtl_fprint(FILE* file, mtl_t* mat);

/** @brief Creates a new, empty material library. 
 * @return A material library.
 */
mtllib_t 
mtllib_create(void);

/** @brief Destroys this material library.
 * @param lib The material library.
 */
void 
mtllib_destroy(mtllib_t* lib);

/** @brief Prints the material library to standard output.
 * @param lib The material library.
 */
void 
mtllib_print(mtllib_t* lib);

/** @brief Print the material library to file. 
 * Prints the material library to the provided file. Assumes the file is
 * opened.
 * @param file The file pointer.
 * @param lib The material library.
 */
void 
mtllib_fprint(FILE* file, mtllib_t* lib);

/** @brief Read a material library from file. 
 * Reads the material from the provided filename into the provided material
 * library.
 * @param fn Filename for the material file.
 * @param lib Pointer to the material library.
 * @return Can return either: [MTL_SUCCESS, MTL_COULDNT_OPEN_FILE, 
 * MTL_OUT_OF_MEMORY].
 */
int 
mtllib_read(const char* fn, mtllib_t* lib);

#endif