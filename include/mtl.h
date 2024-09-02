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
#include "mtl_opts.h"
#include "refl.h"
#include "utils.h"

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
		/** The "halo" flag behind the "d" command. Specifies that the 
		* material's transparency should be dependent on a surface's orientation
		* with regards to the world camera. */
		int halo;
	} dissolve;				
	/** Specifies the specular exponent (Ns) for this material. */
    uint32_t specular_exponent;
	/** Specifies the sharpness of reflections (sharprness) of this material. */
    uint32_t sharpness;
	/** Specifies the optical density (Ni) for this material. Also known as 
	 * "index of refraction". */
    float optical_density;
	/** Ambient map. */
	map_Ka_t map_Ka;
	/** Diffuse map. */
	map_Kd_t map_Kd;
	/** Specular map. */
	map_Ks_t map_Ks;
	/** Specular exponent (shininess) map. */
	map_Ns_t map_Ns;
	/** Dissolve (transparency) map. */
	map_d_t	map_d;
	/** Antialiasing map. */
	map_aat_t antialias;
	/** Texture decaling. */
	decal_t	decal;
	/** Texture distortion map. */
	disp_t disp;
	/** Bump map. */
	bump_t bump;
	/** Reflection map. */
	refl_t refl_map;
} mtl_t;

// -----------------------------------------------------------------------------
// 		mtl_t.
// -----------------------------------------------------------------------------

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

#endif