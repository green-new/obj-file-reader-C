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

// Definitions and includes for map.h
#ifndef MAP_NAME 
#define MAP_NAME mat_map
#endif
#ifndef VALUE_TYPE
#define VALUE_TYPE mtl_t
#endif
#include "map.h"

/** @struct mtl_t
 * @brief Material data format as defined from
 * http://paulbourke.net/dataformats/mtl/.
 */
typedef struct {
	/** Name of this material type. */
    char* name;
	/** Specifies the ambient reflectivity using rgb values 
	* scaled from [0, 1]. */
    float ambient[3];
	/** Specifies the diffuse reflectivity using rgb values scaled from 
	* [0, 1]. */
    float diffuse[3];
	/** Specifies the specular reflectivity using rgb values scaled from 
	* [0, 1]. */
    float specular[3];
	/** Specifies the transmission filter of this material using rgb values 
	* scaled from [0, 1]. */
    float tm_filter[3];
	/** Specifies the illumination model to use. Value between 0-10, see 
	* complete descriptions in illum.h. */
    uint32_t illum;
	/** Specifies the "dissolve", or more well known as transparency, of this 
	* material. */
    float transparency;
	/** The "halo" flag behind the "d" command. Specifies that the material's 
	* transparency should be dependent on a surface's orientation with regards 
	* to the world camera. */
	int halo;
	/** Specifies the specular exponent for this material. */
    uint32_t specular_exponent;
	/** Specifies the sharpness of reflections of this material. */
    uint32_t sharpness;
	/** Specifies the optical density for this material. Also known as "index 
	* of refraction". */
    float optical_density;
} mtl_t;

/** A library of material types. Non-opaque. */
typedef struct {
	/** Value of this material library. */
    char* name;
	/** Map of [material name, material] key/value pairs. */
    mat_map map;
} mtllib_t;

/** @brief Creates a new, empty material object.
 * @returns The material.
 */
mtl_t mtl_create(void);

/** @brief Destroy the material.
 * Destroys any heap allocated data in this material object, and sets all 
 * values to 0.
 * @param material The material object.
 */
void mtl_destroy(mtl_t* material);

/** @brief Prints the material's properties to standard output.
 * @param mat The material to print.
 */
void mtl_print(mtl_t* mat);

/** @brief Prints the material's properties to file. 
 * Prints the material's properties to the provided file. Assumes the file is
 * opened.
 * @param file The file pointer.
 * @param mat The material to print.
 * @return void
 */
void mtl_fprint(FILE* file, mtl_t* mat);

/** @brief Creates a new, empty material library. 
 * @return A material library.
 */
mtllib_t mtllib_create(void);

/** @brief Destroys this material library.
 * @param lib The material library.
 */
void mtllib_destroy(mtllib_t* lib);

/** @brief Prints the material library to standard output.
 * @param lib The material library.
 */
void mtllib_print(mtllib_t* lib);

/** @brief Print the material library to file. 
 * Prints the material library to the provided file. Assumes the file is
 * opened.
 * @param file The file pointer.
 * @param lib The material library.
 */
void mtllib_fprint(FILE* file, mtllib_t* lib);

/** @brief Read a material library from file. 
 * Reads the material from the provided filename into the provided material
 * library.
 * @param fn Filename for the material file.
 * @param lib Pointer to the material library.
 * @return Can return either: [MTL_SUCCESS, MTL_COULDNT_OPEN_FILE, 
 * MTL_OUT_OF_MEMORY].
 */
int mtllib_read(const char* fn, mtllib_t* lib);

#endif