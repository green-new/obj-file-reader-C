#ifndef MTL_READER_H_INCLUDED
#define MTL_READER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "utils.h"

/** Material data format as defined from http://paulbourke.net/dataformats/mtl/. 
*/
typedef struct {
    char* name;
    double ambient[3];
    double diffuse[3];
    double specular[3];
    double transmission_filter[3];
    uint32_t illum;
    double transparency;
    uint32_t specular_exponent;
    uint32_t sharpness;
    double optical_density;
} mtl_t;

/** A library of material types.
*/
typedef struct {
    char* name;
    mtl_t* mtl_arr;
    uint8_t used;
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