#ifndef MTL_READER_H_INCLUDED
#define MTL_READER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "utils.h"

// Definitions and includes for map.h
#define MAP_NAME mat_map
#define VALUE_TYPE mtl_t
#include "map.h"

/** Material data format as defined from http://paulbourke.net/dataformats/mtl/. 
*/
typedef struct {
	/** Name of this material type. */
    char* name;
	/** Specifies the ambient reflectivity using rgb values scaled from [0, 1]. */
    double ambient[3];
	/** Specifies the diffuse reflectivity using rgb values scaled from [0, 1]. */
    double diffuse[3];
	/** Specifies the specular reflectivity using rgb values scaled from [0, 1]. */
    double specular[3];
	/** Specifies the transmission filter of this material using rgb values scaled from [0, 1]. */
    double transmission_filter[3];
	/** Specifies the illumination model to use. Value between 0-10, see complete descriptions in illum.h. */
    uint32_t illum;
	/** Specifies the "dissolve", or more well known as transparency, of this material. */
    double transparency;
	/** The "halo" flag behind the "d" command. Specifies that the material's transparency should be dependent on a surface's orientation with regards to the world camera. */
	int halo;
	/** Specifies the specular exponent for this material. */
    uint32_t specular_exponent;
	/** Specifies the sharpness of reflections of this material. */
    uint32_t sharpness;
	/** Specifies the optical density for this material. Also known as "index of refraction". */
    double optical_density;
} mtl_t;

/** A library of material types.
*/
typedef struct {
	/** Value of this material library. */
    char* name;
	/** Map of [material name, material] key/value pairs. */
    mat_map map;
} mtllib_t;

/** Creates a new, empty material object.
 * 
 * @returns The material.
 */
mtl_t mtl_create(void);

/** Destroys any heap allocated data in this material object, and sets all values to 0.
 * 
 * @param material The material object.
 * @return void 
 */
void mtl_destroy(mtl_t* material);

/** Prints the material's properties to standard output.
 * 
 * @param mat The material to print.
 * @return void
 */
void mtl_print(mtl_t* mat);

/** Prints the material's properties to the provided file.
 * 
 * @param fptr The file pointer.
 * @param mat The material to print.
 * @return void
 */
void mtl_fprint(FILE* fptr, mtl_t* mat);

/** Creates a new, empty material library. 
 *
 * @return A material library.
 */
mtllib_t mtllib_create(void);

/** Destroys this material library.
 *
 * @param mat_lib The material library.
 * @return void
 */
void mtllib_destroy(mtllib_t* lib);

/** Prints the material library to standard output.
 * 
 * @param arr The material library.
 * @return void
 */
void mtllib_print(mtllib_t* lib);

/** Prints the material library to the provided file.
 * 
 * @param fptr The file pointer.
 * @param arr The material library.
 * @return void
 */
void mtllib_fprint(FILE* fptr, mtllib_t* lib);

/** Reads the material from the provided filename.
 * @param fn  Filename for the material file.
 * @param mat_lib Pointer to the material library.
 * @return Can return either: [MTL_SUCCESS, MTL_COULDNT_OPEN_FILE, MTL_OUT_OF_MEMORY].
 */
int mtllib_read(const char* fn, mtllib_t* lib);

#endif