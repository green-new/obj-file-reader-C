/**
 * @file mtllib.h
 * @author green
 * @date 8/27/2024
 * @brief Definition of the material library. 
 * @see http://paulbourke.net/dataformats/mtl/
 */
#ifndef MTL_LIB_H_INCLUDED
#define MTL_LIB_H_INCLUDED

#include "material_map.h"

/** A library of material types. Non-opaque. */
typedef struct {
	/** Name of this material library. */
    const char* name;
	/** Map of [material name, material] key/value pairs. */
    mat_map map;
} mtllib_t;

// -----------------------------------------------------------------------------
// 		mtllib_t.
// -----------------------------------------------------------------------------

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