#ifndef MTL_READER_H_INCLUDED
#define MTL_READER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "utils.h"

#define MTLARRAY_MAX 16

/* Material data format as defined from http://paulbourke.net/dataformats/mtl/. */
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

typedef struct {
    char* mtllib_name;
    mtl_t material_array[MTLARRAY_MAX];
    uint8_t used;
} mtlarr_t;

/**
 * Creates a new material object by value.
 * 
 * @returns The material by value.
 */
mtl_t mtl_create();

/**
 * Destroys any heap allocated data in this material object.
 * 
 * @param material The material object.
 * @return void 
 */
void mtl_destroy(mtl_t* material);

/**
 * Prints the material to standard output.
 * 
 * @param mat The material to print.
 * @returns void
 */
void mtl_print(mtl_t* mat);

/**
 * Prints the material array to standard output.
 * 
 * @param arr The material array.
 * @returns void
 */
void mtlarr_print(mtlarr_t* arr);

/** 
 * Reads the material from the provided filename.
 * @param fn  Filename for the material file.
 * @param material_array Pointer to the material array.
 * @returns Can return either: [MTL_SUCCESS, MTL_COULDNT_OPEN_FILE, MTL_OUT_OF_MEMORY].
 */
int mtlarr_read(const char* fn, mtlarr_t* material_array);

#endif