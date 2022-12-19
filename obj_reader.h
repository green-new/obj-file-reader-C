#ifndef OBJ_READER_H_INCLUDED
#define OBJ_READER_H_INCLUDED

/************************************************
 * Author: green
 * Date: November 3rd, 2022
 * Description: This single header C library reads .obj files.
 * It determines the dim of each primitive and generates
 * the required memory needed.
*************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <errno.h>

#define X   0
#define Y   1
#define Z   2
#define W   3
#define U   0
#define V   1
#define R   0
#define G   1
#define B   2

#define POS_INDEX       0
#define VT_INDEX        1
#define VN_INDEX        2
#define MAX_LINE_LEN    255
#define MAX_BUFFER_SIZE 255

#define POSIDX                  0b1
#define TEXIDX                  0b10
#define NORMIDX                 0b100

#define EQU(s0, s1)     strcmp(s0, s1) == 0
#define SEEK_START(f)   fseek(f, 0, SEEK_SET)
// Positions
#define GET_X(mesh, i)      mesh.vertex_data[i].pos[X]
#define GET_Y(mesh, i)      mesh.vertex_data[i].pos[Y]
#define GET_Z(mesh, i)      mesh.vertex_data[i].pos[Z]
// Normals
#define GET_NORM_X(mesh, i) mesh.normal_data[i].norm[X]
#define GET_NORM_Y(mesh, i) mesh.normal_data[i].norm[Y]
#define GET_NORM_Z(mesh, i) mesh.normal_data[i].norm[Z]
// Textures
#define GET_TEX_X(mesh, i)  mesh.texture_data[i].tex[X]
#define GET_TEX_Y(mesh, i)  mesh.texture_data[i].tex[Y]
#define GET_TEX_Z(mesh, i)  mesh.texture_data[i].tex[Z]

#define CLEAR_BUFFER(buffer) memset(buffer, 0, sizeof *buffer);

#define VERTEX_MEMSIZE          data->num_vertices * sizeof(vertex_t)
#define TEXTURE_MEMSIZE         data->num_textures * sizeof(texture_t)
#define FACE_MEMSIZE            data->num_faces * sizeof(face_t)
#define NORMAL_MEMSIZE          data->num_normals * sizeof(normal_t)

typedef unsigned int uint;
typedef float f32;
typedef const char* string;
typedef char* mutable_string;

#define NEW(size) malloc_impl(size, __LINE__, __FUNCTION__)
#define DELETE(ptr) free_impl(ptr, __LINE__, __FUNCTION__)

typedef struct vertex {
    f32* pos;
} vertex_t;

typedef struct texture {
    f32* tex;
} texture_t;

typedef struct normal {
    f32* norm;
} normal_t;

typedef struct face {
    uint* indices;
    uint* texs;
    uint* norms;
} face_t;

/* Material data format as defined from http://paulbourke.net/dataformats/mtl/. */
typedef struct {
    /* The name of this material. */
    string name;

    /* The ambient color of light on this material. */
    f32 ambient[3];

    /* The diffuse color of light on this material. */
    f32 diffuse[3];

    /* The specular color of light on this material. */
    f32 specular[3];

    /* The transmission filter of light on this material (what colors of light are passed through this object?) */
    f32 transmission_filter[3];

    /* The illumination model to use for this material. */
    uint illum;

    /* Determines how much this material dissolves into the background; 0.0 is fully opaque, 1.0 is fully transparent. */
    f32 transparency;

    /* The specular exponent for specular lighting. */
    uint specular_exponent;

    /* Specifies the sharpness of the reflections from the local reflection map. Default is 60 */
    uint sharpness;

    /* The optical density of this material. Otherwise known as refraction. (Ni) */
    f32 optical_density;
} mtl_t;

typedef struct mesh {
    uint face_dim;
    uint vertex_dim;
    uint tex_dim;

    vertex_t* vertex_data;
    normal_t* normal_data;
    texture_t* texture_data;
    face_t* face_data;

    uint num_vertices;
    uint num_normals;
    uint num_textures;
    uint num_faces;

    mutable_string mtl_filename;
    mutable_string name;

    union { // Little endian
        uint flag;
        enum {
            // Little endian. LSB
            pos_flag    = (1 << 0),
            tex_flag    = (1 << 1),
            norm_flag   = (1 << 2)
            // MSB
        } flags;
    } face_flag;
} mesh_t;

typedef enum {
    OBJ_SUCCESS = 0,
    OBJ_INVALID_DIMS,
    OBJ_OUT_OF_MEMORY,
    OBJ_FILE_INACCESSIBLE,
} obj_return_code;

typedef enum {
    TYPE_UINT,
    TYPE_STR,
    TYPE_FLOAT
} type_t;

// Function declarations
uint get_dim(char buffer[MAX_LINE_LEN]);
void load_buffer(mesh_t* data, uint limit, void* bufferdata, FILE* file, uint dim, const string type, void* dest, type_t dataformat);
uint init_buffer(char buffer[MAX_LINE_LEN], void* data, const uint dim, const type_t dataformat);
uint sizeof_typet(type_t t);
void mesh_print_element(const void* data, type_t type, uint dim, uint index, uint limit);
int read_mesh(string fn, mesh_t* data);
void triangulate(mesh_t* data);
void mesh_write_to_file(const mesh_t* mesh_obj, string fn);

void* malloc_impl(size_t size, int line, const char* func) {
    void *p = malloc(size);
    //printf("Allocated memory [function = %s, line = %d, pointer = %p[%llu]]\n", func, line, p, size);
    return p;
}

void free_impl(void* ptr, int line, const char* func) {
    free(ptr);
    //printf("Freed memory [function = %s, line = %d, pointer = %p]\n", func, line, ptr);
}

uint sizeof_typet(type_t t) {
    switch(t) {
        case TYPE_UINT:
            return sizeof(uint);
        break;
        case TYPE_FLOAT:
            return sizeof(f32);
        break;
        case TYPE_STR:
            return sizeof(string);
        break;
        default: return 0;
    }
    return 0;
}

uint init_buffer(char buffer[MAX_LINE_LEN], void* data, const uint dim, const type_t dataformat) {
    mutable_string token = strtok(buffer, " ");
    for (uint o = 0; o < dim; o++) {
        token = strtok(NULL, " ");
        token[strcspn(token, "\n")] = '\0';

        switch(dataformat) {
            case TYPE_FLOAT:
                f32 fconv = atof(token);
                ((f32*)data)[o] = fconv;
            break;
            case TYPE_UINT:
                uint uconv = atoi(token);
                ((uint*)data)[o] = uconv;
            break;
            case TYPE_STR:
                // ((mutable_string*)data)[o] = calloc(1, strlen(token) + 1);
                ((mutable_string*)data)[o] = realloc(((mutable_string*)data)[o], strlen(token) + 1);
                CLEAR_BUFFER(((mutable_string*)data)[o]);
                strcpy(((mutable_string*)data)[o], token);
            break;
            default: break;
        }
    }
    return 1;
}

uint get_dim(char buffer[MAX_LINE_LEN]) {
    // Faces, vertices, and normals have dim 3
    // Textures have dim 2.
    uint dim = 0;
    mutable_string token = strtok(buffer, " ");
    do {
        token = strtok(NULL, " ");
        dim++;
    }  while (token);
    dim--;
    return dim;
}

void mesh_print_element(const void* data, type_t type, uint dim, uint index, uint limit) {
    if (index > limit) return;
    printf("[");
    for (uint i = 0; i < dim; i++) {
        switch(type) {
            case TYPE_FLOAT:
                float f = ((f32*)data)[i];
                printf("%f", f);
            break;
            case TYPE_UINT:
                printf("%d", ((uint*)data)[i]);
            break;
            case TYPE_STR:
                printf("%s", ((string*)data)[i]);
            break;
            default: break;
        }
        if (i != dim - 1) printf(", ");
    }
    printf("]\n");
}

void mesh_print_vertex(const mesh_t* data, uint vert_index) {
    mesh_print_element(data->vertex_data[vert_index].pos, TYPE_FLOAT, data->vertex_dim, vert_index, data->num_vertices);
}

void mesh_print_normal(const mesh_t* data, uint normal_index) {
    mesh_print_element(data->normal_data[normal_index].norm, TYPE_FLOAT, data->vertex_dim, normal_index, data->num_normals);
}

void mesh_print_texcoord(const mesh_t* data, uint tex_index) {
    mesh_print_element(data->texture_data[tex_index].tex, TYPE_FLOAT, data->tex_dim, tex_index, data->num_textures);
}

void mesh_print_face(const mesh_t* data, uint face_index) {
    printf("Position Indices: \n");
    mesh_print_element(data->face_data[face_index].indices, TYPE_UINT, data->face_dim, face_index, data->num_faces);
    printf("Texture Indices: \n");
    mesh_print_element(data->face_data[face_index].texs, TYPE_UINT, data->face_dim, face_index, data->num_faces);
    printf("Normal Indices: \n");
    mesh_print_element(data->face_data[face_index].norms, TYPE_UINT, data->face_dim, face_index, data->num_faces);
}

void mesh_write_to_file(const mesh_t* mesh_obj, string fn) {

    FILE* file = fopen(fn, "w");
    if (!file)
        return;

    fprintf(file, "*** Object Name: %s ***\n", mesh_obj->name);
    fprintf(file, "*** Vertex Dimension: %d ***\n", mesh_obj->vertex_dim);
    fprintf(file, "*** Texture Dimension: %d ***\n", mesh_obj->tex_dim);
    fprintf(file, "*** Face Dimension: %d ***\n", mesh_obj->face_dim);

    fprintf(file, "*** Vertex Positions ***\n");
    if (mesh_obj->num_vertices <= 0) {
        fprintf(file, "none\n");
    }
    for (uint i = 0; i < mesh_obj->num_vertices; i++) {
        fprintf(file, "[");
        for (uint j = 0; j < mesh_obj->vertex_dim; j++) {
            fprintf(file, "%f", mesh_obj->vertex_data[i].pos[j]);
            if (j != mesh_obj->vertex_dim - 1)
                fprintf(file, ", ");
        }
        fprintf(file, "]\n");
    }

    fprintf(file, "*** Vertex Normals ***\n");
    if (mesh_obj->num_normals <= 0) {
        fprintf(file, "none\n");
    }
    for (uint i = 0; i < mesh_obj->num_normals; i++) {
        fprintf(file, "[");
        for (uint j = 0; j < mesh_obj->vertex_dim; j++) {
            fprintf(file, "%f", mesh_obj->normal_data[i].norm[j]);
            if (j != mesh_obj->vertex_dim - 1)
                fprintf(file, ", ");
        }
        fprintf(file, "]\n");
    }

    fprintf(file, "*** Texture Coordinates ***\n");
    if (mesh_obj->num_textures <= 0) {
        fprintf(file, "none\n");
    }
    for (uint i = 0; i < mesh_obj->num_textures; i++) {
        fprintf(file, "[");
        for (uint j = 0; j < mesh_obj->tex_dim; j++) {
            fprintf(file, "%f", mesh_obj->texture_data[i].tex[j]);
            if (j != mesh_obj->tex_dim - 1)
                fprintf(file, ", ");
        }
        fprintf(file, "]\n");
    }

    fprintf(file, "*** Face Data ***\n");
    for (uint i = 0; i < mesh_obj->num_faces; i++) {
        fprintf(file, "*** Face %d ***\n", i+1);
        if (mesh_obj->face_flag.flag & pos_flag) {
            fprintf(file, "Position Indices -> ");
            fprintf(file, "[");
            for (uint j = 0; j < mesh_obj->face_dim; j++) {
                fprintf(file, "%d", mesh_obj->face_data[i].indices[j]);
                if (j != mesh_obj->face_dim - 1)
                    fprintf(file, ", ");
            }
            fprintf(file, "]\n");
        }

        if (mesh_obj->face_flag.flag & tex_flag) {
            fprintf(file, "Texture Indices ->");
            fprintf(file, "[");
            for (uint j = 0; j < mesh_obj->face_dim; j++) {
                fprintf(file, "%d", mesh_obj->face_data[i].texs[j]);
                if (j != mesh_obj->face_dim - 1)
                    fprintf(file, ", ");
            }
            fprintf(file, "]\n");
        }

        if (mesh_obj->face_flag.flag & norm_flag) {
            fprintf(file, "Normal Indices ->");
            fprintf(file, "[");
            for (uint j = 0; j < mesh_obj->face_dim; j++) {
                fprintf(file, "%d", mesh_obj->face_data[i].norms[j]);
                if (j != mesh_obj->face_dim - 1)
                    fprintf(file, ", ");
            }
            fprintf(file, "]\n");
        }
    }

    fclose(file);
}

// Gets and maintains the face flag for a particular element.
uint get_face_flag(const char* face_str_element, uint line) {
    const char* ptr = face_str_element;
    uint first_pass = 1;
    uint size = 0;
    uint flag = 0;
    uint shift = 0;
    uint prev_flag = 0;
    uint curr_flag = 0;
    while (*ptr != '\0' && *ptr != '\n') {
        while (*ptr != ' ' && *ptr != '\n' && *ptr != '\0') {
            flag = 1 << shift;
            size = 0;
            // Keep counting even if we are on a space, and the last character is 'f'.
            while (*ptr != '/' && *ptr != '\0' && *ptr != '\n' && (*ptr != ' ' || *(ptr - 1) == 'f')) {
                ptr++;
                size++;
            }
            if (size > 0) {
                curr_flag |= flag;
            } else {
                shift++;
                ptr++;
            }
        }
        if (first_pass) {
            first_pass = !first_pass;
            prev_flag = curr_flag;
        }
        if (prev_flag != curr_flag) {
            printf("Error: inconsistent face definitions at line %d, flag was %d, expected %d\n", line, curr_flag, prev_flag);
            return 0;
        }
        prev_flag = curr_flag;
        curr_flag = 0;
        flag = 0;
        shift = 0;
        ptr++;
    }
    return prev_flag;
}
void destroy_mesh(mesh_t* data) {
    for (uint i = 0; i < data->num_vertices; i++) {
        DELETE(data->vertex_data[i].pos);
    }
    DELETE(data->vertex_data);
    for (uint i = 0; i < data->num_normals; i++) {
        DELETE(data->normal_data[i].norm);
    }
    DELETE(data->normal_data);
    for (uint i = 0; i < data->num_textures; i++) {
        DELETE(data->texture_data[i].tex);
    }
    DELETE(data->texture_data);
    for (uint i = 0; i < data->num_faces; i++) {
        if(data->face_flag.flag & pos_flag)
            DELETE(data->face_data[i].indices);
        if(data->face_flag.flag & tex_flag)
            DELETE(data->face_data[i].texs);
        if(data->face_flag.flag & norm_flag)
            DELETE(data->face_data[i].norms);
    }

    DELETE(data->face_data);
    if (data->mtl_filename && data->mtl_filename != "none")
        DELETE(data->mtl_filename);
    if (data->name && data->name != "none")
        DELETE(data->name);
}

void initialize_mesh(mesh_t* data) {
    data->face_dim = 0;
    data->vertex_dim = 0;
    data->tex_dim = 0;

    data->num_faces = 0;
    data->num_normals = 0;
    data->num_textures = 0;
    data->num_vertices = 0;

    data->vertex_data = 0;
    data->face_data = 0;
    data->normal_data = 0;
    data->texture_data = 0;

    data->face_flag.flag = 0;

    data->name = "none";
    data->mtl_filename = "none";
}

int set_mesh_info(mesh_t* data, FILE* file, char* err_msg) {
    uint tmp_num_verts = 0;
    uint tmp_num_faces = 0;
    uint tmp_num_norms = 0;
    uint tmp_num_texs = 0;
    char buffer[MAX_LINE_LEN] = {0};
    uint line_number = 0;

    int name_defined = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        line_number++;
        string type;

        char original_string_for_face[sizeof(buffer)];
        char original_string_for_dim[sizeof(buffer)];
        strcpy(original_string_for_face, buffer);
        strcpy(original_string_for_dim, buffer);

        type = strtok(buffer, " ");

        if (EQU(type, "v")) {
            tmp_num_verts++;

            uint tmp_dim = get_dim(original_string_for_dim);

            if (data->vertex_dim != 0 && tmp_dim != data->vertex_dim) {
                sprintf(err_msg, "Error: mismatch of vertex dimension at line %d: expected %d, got %d vertices at function %s\n",
                        line_number, data->vertex_dim, tmp_dim, __FUNCTION__);
                return OBJ_INVALID_DIMS;
            } else {
                data->vertex_dim = tmp_dim;
            }
        } else if (EQU(type, "vn")) {
            tmp_num_norms++;

            uint tmp_dim = get_dim(original_string_for_dim);

            if (data->vertex_dim != 0 && tmp_dim != data->vertex_dim) {
                sprintf(err_msg, "Error: mismatch of vertex normal dimension at line %d: expected %d, got %d vertex normals (must be same dimensions as vertex dimension) at function %s\n",
                        line_number, data->vertex_dim, tmp_dim, __FUNCTION__);
                return OBJ_INVALID_DIMS;
            } else {
                data->vertex_dim = tmp_dim;
            }
        } else if (EQU(type, "vt")){
            tmp_num_texs++;

            uint tmp_dim = get_dim(original_string_for_dim);

            if (data->tex_dim != 0 && tmp_dim != data->tex_dim) {
                sprintf(err_msg, "Error: mismatch of vertex texture dimension at line %d: expected %d, got %d vertex texture components at function %s\n",
                        line_number, data->tex_dim, tmp_dim, __FUNCTION__);
                return OBJ_INVALID_DIMS;
            } else {
                data->tex_dim = tmp_dim;
            }
        } else if (EQU(type, "f")) {
            tmp_num_faces++;

            uint tmp_dim = get_dim(original_string_for_dim);

            if (data->face_dim != 0 && tmp_dim != data->face_dim) {
                sprintf(err_msg, "Error: mismatch of face dimension at line %d: expected %d, got %d face components at function %s\n",
                        line_number, data->tex_dim, tmp_dim, __FUNCTION__);
                return OBJ_INVALID_DIMS;
            } else {
                data->face_dim = tmp_dim;
            }

            // Special case: determine the face flag (what geometric components describe each face?):
            data->face_flag.flag = get_face_flag(original_string_for_face, line_number);
        } else if (EQU(type, "o") && !name_defined) {
            char* tmp_name = strtok(NULL, "\n");
            data->name = calloc(1, strlen(tmp_name));
            strcpy(data->name, tmp_name);
            name_defined = !name_defined;
        } else if (EQU(type, "mtllib")) {
            char* tmp_name = strtok(NULL, "\n");
            data->mtl_filename = calloc(1, strlen(tmp_name));
            strcpy(data->mtl_filename, tmp_name);
        }
    }
    data->num_vertices = tmp_num_verts;
    data->num_faces = tmp_num_faces;
    data->num_normals = tmp_num_norms;
    data->num_textures = tmp_num_texs;

    return OBJ_SUCCESS;
}


// Generates the mesh given a .obj file.
int read_mesh(const char* fn, mesh_t* data) {
    obj_return_code RETURN_CODE = OBJ_SUCCESS;
    char* err_msg = "\0";

    initialize_mesh(data);

    FILE* file = fopen(fn, "r");
    if (!file) {
        printf("Error: invalid, inaccessible, unavailable, or nonexistent file \"%s\" in function %s\n", fn, __FUNCTION__);
        RETURN_CODE = OBJ_FILE_INACCESSIBLE;
        goto cleanup_file;
    }

    if ((RETURN_CODE = set_mesh_info(data, file, err_msg))) {
        printf("%s", err_msg);
        goto cleanup_file;
    }
    data->vertex_data = NEW(data->num_vertices * sizeof(vertex_t));
    data->normal_data = NEW(data->num_normals * sizeof(normal_t));
    data->texture_data = NEW(data->num_textures * sizeof(texture_t));
    data->face_data = NEW(data->num_faces * sizeof(face_t));

    // Reusable buffers
    uint* pos_idx_buffer = NEW(data->face_dim * sizeof *pos_idx_buffer),
        *tex_idx_buffer = NEW(data->face_dim * sizeof *tex_idx_buffer),
        *norm_idx_buffer = NEW(data->face_dim * sizeof *norm_idx_buffer);
    mutable_string* face_str_buffer = NEW(data->face_dim * sizeof *face_str_buffer);
    for (uint i = 0; i < data->face_dim; i++) {
        face_str_buffer[i] = NEW(32);
    }
    void* generic_buffer = NEW(MAX_LINE_LEN);

    // Error check for memory
    if (!pos_idx_buffer ||
        !tex_idx_buffer ||
        !norm_idx_buffer ||
        !face_str_buffer ||
        !data->vertex_data ||
        !data->normal_data ||
        !data->texture_data ||
        !data->face_data) {
            printf("Error: unable to service memory allocation request for buffers OR mesh components in function %s. Kernel error was [%d]: %s", __FUNCTION__, errno, strerror(errno));
            RETURN_CODE = OBJ_OUT_OF_MEMORY;
            goto cleanup_buffers;
    } else {
        CLEAR_BUFFER(face_str_buffer);
        CLEAR_BUFFER(pos_idx_buffer);
        CLEAR_BUFFER(tex_idx_buffer);
        CLEAR_BUFFER(norm_idx_buffer);
        CLEAR_BUFFER(generic_buffer);
    }

    // Allocate memory for the data components of the mesh
    for (uint i = 0; i < data->num_vertices; i++) {
        data->vertex_data[i].pos = NEW(data->vertex_dim * sizeof *data->vertex_data[i].pos);
        CLEAR_BUFFER(data->vertex_data[i].pos);
    }
    for (uint i = 0; i < data->num_normals; i++) {
        data->normal_data[i].norm = NEW(data->vertex_dim * sizeof *data->normal_data[i].norm);
        CLEAR_BUFFER(data->normal_data[i].norm);
    }
    for (uint i = 0; i < data->num_textures; i++) {
        data->texture_data[i].tex = NEW(data->tex_dim * sizeof *data->texture_data[i].tex);
        CLEAR_BUFFER(data->texture_data[i].tex);
    }
    for (uint i = 0; i < data->num_faces; i++) {
        if (data->face_flag.flag & pos_flag) {
            data->face_data[i].indices = NEW(data->face_dim * sizeof *data->face_data[i].indices);
            CLEAR_BUFFER(data->face_data[i].indices);
        }
        if (data->face_flag.flag & norm_flag) {
            data->face_data[i].norms = NEW(data->face_dim * sizeof *data->face_data[i].norms);
            CLEAR_BUFFER(data->face_data[i].norms);
        }
        if (data->face_flag.flag & tex_flag) {
            data->face_data[i].texs = NEW(data->face_dim * sizeof *data->face_data[i].texs);
            CLEAR_BUFFER(data->face_data[i].texs);
        }
    }

    // Read the file contents
    char line_buffer[MAX_LINE_LEN];
    char line_buffer_cpy[sizeof line_buffer];
    uint vi = 0, ti = 0, ni = 0, fi = 0;

    SEEK_START(file);
    while (fgets(line_buffer, sizeof line_buffer, file)) {
        CLEAR_BUFFER(generic_buffer);
        void** mesh_member;
        strcpy(line_buffer_cpy, line_buffer);
        string type = strtok(line_buffer, " ");
        uint dim;
        type_t dataformat;
        if (EQU(type, "v")) {
            if (vi >= data->num_vertices)
                continue;
            mesh_member = (void**)(&data->vertex_data[vi].pos);
            dim = data->vertex_dim;
            dataformat = TYPE_FLOAT;
            vi++;
        } else if (EQU(type, "vt")) {
            if (ti >= data->num_textures)
                continue;
            mesh_member = (void**)(&data->texture_data[ti].tex);
            dim = data->tex_dim;
            dataformat = TYPE_FLOAT;
            ti++;
        } else if (EQU(type, "vn")) {
            if (ni >= data->num_normals)
                continue;
            mesh_member = (void**)(&data->normal_data[ni].norm);
            dim = data->vertex_dim;
            dataformat = TYPE_FLOAT;
            ni++;
        } else if (EQU(type, "f")) {
            if (fi >= data->num_faces)
                continue;

            if (!init_buffer(line_buffer_cpy, face_str_buffer, data->face_dim, TYPE_STR))
                continue;

            for (uint j = 0; j < data->face_dim; j++) {
                mutable_string buffer = strtok(face_str_buffer[j], "/");
                if (data->face_flag.flag & pos_flag) {
                    int pos_index = atoi(buffer);
                    pos_idx_buffer[j] = pos_index;
                    buffer = strtok(NULL, "/");
                } else {
                    pos_idx_buffer[j] = 0;
                }
                if (data->face_flag.flag & tex_flag) {
                    int tex_index = atoi(buffer);
                    tex_idx_buffer[j] = tex_index;
                    buffer = strtok(NULL, "/");
                } else {
                    tex_idx_buffer[j] = 0;
                }
                if(data->face_flag.flag & norm_flag) {
                    int norm_index = atoi(buffer);
                    norm_idx_buffer[j] = norm_index;
                    buffer = strtok(NULL, "/");
                } else {
                    norm_idx_buffer[j] = 0;
                }
            }
            if (data->face_flag.flag & pos_flag)
                memcpy(data->face_data[fi].indices, pos_idx_buffer, sizeof *pos_idx_buffer * data->face_dim);
            if (data->face_flag.flag & tex_flag)
                memcpy(data->face_data[fi].texs, tex_idx_buffer, sizeof *tex_idx_buffer * data->face_dim);
            if(data->face_flag.flag & norm_flag)
                memcpy(data->face_data[fi].norms, norm_idx_buffer, sizeof *norm_idx_buffer * data->face_dim);

            CLEAR_BUFFER(face_str_buffer);
            CLEAR_BUFFER(pos_idx_buffer);
            CLEAR_BUFFER(tex_idx_buffer);
            CLEAR_BUFFER(norm_idx_buffer);

            fi++;
            continue;
        } else {
            continue;
        }

        generic_buffer = realloc(generic_buffer, sizeof_typet(dataformat) * dim);
        if (!generic_buffer) {
            RETURN_CODE = OBJ_OUT_OF_MEMORY;
            goto cleanup_buffers;
        }

        if (!init_buffer(line_buffer_cpy, generic_buffer, dim, dataformat))
            continue;

        memcpy(*mesh_member, generic_buffer, sizeof_typet(dataformat) * dim);
    }

    /*
    * Label for cleanup (after errors or logical flow)
    */

    cleanup_buffers:
    if (generic_buffer)
        DELETE(generic_buffer);
    for (uint i = 0; i < data->face_dim; i++) {
        if (face_str_buffer[i]) {
            DELETE(face_str_buffer[i]);
        }
    }
    if (face_str_buffer)
        DELETE(face_str_buffer);
    if (pos_idx_buffer)
        DELETE(pos_idx_buffer);
    if (tex_idx_buffer)
        DELETE(tex_idx_buffer);
    if (norm_idx_buffer)
        DELETE(norm_idx_buffer);

    cleanup_file:
    if (file)
        fclose(file);

    // Completed.
    return RETURN_CODE;
}

// Uses a simple fan triangluation method.
void triangulate(mesh_t* data) {
    if (data->face_dim <= 3) return;
    // If a face has 4 indices, then break it into 2 faces with 3 elements.
    // If a face has 5 indices, then break it into 3 faces with 3 elements.
//uint required_faces = data->face_dim - 2;
//    uint* pos_buffer, *tex_buffer, *norm_buffer;
//    type_t dataformat = TYPE_UINT;

    //const uint memsize = data->face_dim * sizeof_typet(dataformat);

    //for (uint i = 0; i < data->num_faces; i++) {
      //  memcpy(pos_buffer, &data->face_data->indices[i], memsize);
      //  memcpy(tex_buffer, &data->face_data->texs[i], memsize);
      //  memcpy(norm_buffer, &data->face_data->norms[i], memsize);
   // }
}


#endif // OBJ_READER_H_INCLUDED
