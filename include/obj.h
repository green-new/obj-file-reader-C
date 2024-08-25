/** C .obj file reader - v1.0.0
 * Author: green 
 */

#ifndef COBJ_READER_H_INCLUDED
#define COBJ_READER_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "mtl.h"
#include "utils.h"
#include "defs.h"

/** Represents a geometric vertex.
 */
typedef struct vertex {
    /* The position of this vertex. */
    double* pos;
} vertex_t;

/** Represents geometric texture coordinates.
 */
typedef struct texture {
    /* The texture coordinates. */
    double* tex;
} texture_t;

/** Represents geometric normal coordinates.
 */
typedef struct normal {
    /* The normal vector. */
    double* norm;
} normal_t;

/** Represents a geometric face with a list of indices.
 */
typedef struct face {
    /* The material this face uses. NULL for nothing. */
    mtl_t* material;

    /* Array of positional indices. */
    uint32_t* indices;
    /* Array of texture coordinates. */
    uint32_t* texs;
    /* Array of normal vectors. */
    uint32_t* norms;
} face_t;

/** Entirely represents a geometric object as a list of vertices, normals, texture coordinates and faces.
 */
typedef struct mesh {
    /* The dimension of every face component. */
    uint32_t face_dim;
    /* The dimension of every vertex AND normal component. Vertices and normal vectors have the same dimension. */
    uint32_t vertex_dim;
    /* The dimension of every texture component. */
    uint32_t tex_dim;

    /* Array of vertex structures. */
    vertex_t* vertex_data;
    /* Array of normal structures. */
    normal_t* normal_data;
    /* Array of texture structures. */
    texture_t* texture_data;
    /* Array of face structures. */
    face_t* face_data;

    /* Number of vertices. */
    uint32_t num_vertices;
    /* Number of normals. */
    uint32_t num_normals;
    /* Number of textures. */
    uint32_t num_textures;
    /* Number of faces. */
    uint32_t num_faces;

    /* C-string name of the object. */
    char* name;

    /* Map of material libraries. */
	mtllib_t mtllib;

    /* The face flag. Determines what attributes are used in every face definition. */
    union u_flags {
        /* The integer face flag. Shares the same memory as each flag enumeration. */
        uint8_t flag;
        /* Enumeration of each bit to be set to determine the face flag. */
        enum {
            /* 0b1. */
            pos_flag = (1 << 0),
            /* 0b10. */
            tex_flag = (1 << 1),
            /* 0b100. */
            norm_flag = (1 << 2)
        } flags;
    } face_flag;
} mesh_t;

/** Prints the object's contents  to standard output.
 *
 * @param data Pointer to the object to print to screen.
 * @return void
 */
void obj_print(const mesh_t* mesh);

/** Writes the mesh's contents to a provided file. Will overwrite or create a new file.
 *
 * @param fn Filename for the output file.
 * @param mesh_obj The mesh to write to the file.
 * @returns void
 */
void obj_fwrite(const mesh_t* mesh, const char* fn);

/** Reads a .obj file from the provided filename and stores the relevant data to a mesh_t object.
 *
 * @param fn Filename to the .obj file.
 * @param data Pointer to the stack-allocated mesh object.
 * @return Can return either: [OBJ_SUCCESS, OBJ_INVALID_DIMS, OBJ_OUT_OF_MEMORY, OBJ_FILE_INACCESSIBLE].
 */
int obj_read(const char* fn, mesh_t* mesh);

/** Initializes all values of the mesh object to 0.
 *
 * @param data Mesh object.
 * @return void
 */
void obj_init(mesh_t* mesh);

/** Frees all data within this mesh object.
 *
 * @param data Mesh object.
 * @return void
 */
void obj_destroy(mesh_t* mesh);


#endif // COBJ_READER_H_INCLUDED
