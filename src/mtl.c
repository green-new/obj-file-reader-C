#include "mtl.h"
// --------------------------------------------------------------
// Implementation
// --------------------------------------------------------------
mtl_t mtl_create(void) {
    mtl_t material = {
        .ambient = {0},
        .diffuse = {0},
        .illum = 0,
        .name = "",
        .optical_density = 0,
        .sharpness = 0,
        .specular = {0},
        .specular_exponent = 0,
        .transmission_filter = {0},
        .transparency = 0
    };
    return material;
}

void mtl_destroy(mtl_t* material) {
    free(material->name);
}

void mtlarr_destroy(mtlarr_t* arr) {
    if (arr->used > 0) {
        for (uint8_t i = 0; i < arr->used; i++) {
            mtl_destroy(&arr->material_array[i]);
        }
    }
    arr->used = 0;
    free(arr->mtllib_name);
}

void mtl_print(mtl_t* mat) {
    printf("Name: %s", mat->name);

}

int mtlarr_read(const char* fn, mtlarr_t* arr) {
    mtlarr_destroy(arr);
    int RETURN_CODE = SUCCESS;
    FILE* file = fopen(fn, "r");
    if (!file) {
        return INVALID_FILE;
    }

    #define init_3f(buf) buffer_init(line_buffer, buf, 3, TYPE_FLOAT)
    #define init_1u(buf) buffer_init(line_buffer, buf, 1, TYPE_UINT);
    #define init_1f(buf) buffer_init(line_buffer, buf, 1, TYPE_FLOAT);
    char line_buffer[MAX_LINE_LEN];
    while (fgets(line_buffer, sizeof line_buffer, file)) {
        const char* type = strtok(line_buffer, " ");
        mtl_t curr_mat;
        // TODO: bounds checks on certain values
        if (strequ(type, "newmtl")) {
            arr->material_array[arr->used] = curr_mat;
            arr->used++;
            curr_mat = mtl_create(strtok(line_buffer, NULL));
        } else if (strequ(type, "Ka")) {
            // TODO: support "xyz" input, not just "x y z"
            init_3f(curr_mat.ambient);
        } else if (strequ(type, "Kd")) {
            init_3f(curr_mat.diffuse);
        } else if (strequ(type, "Ks")) {
            init_3f(curr_mat.specular);
        } else if (strequ(type, "Tf")) {
            init_3f(curr_mat.transmission_filter);
        } else if (strequ(type, "illum")) {
            init_1u(&curr_mat.illum);
        } else if(strequ(type, "d")) {
            // TODO: support -halo
            init_1f(&curr_mat.transparency);
        } else if (strequ(type, "Ns")) {
            init_1u(&curr_mat.specular_exponent);
        } else if (strequ(type, "sharpness")) {
            init_1u(&curr_mat.sharpness);
        } else if(strequ(type, "Ni")) {
            init_1f(&curr_mat.optical_density);
        }
    }
    #undef init_3f
    #undef init_1u
    #undef init_1f

    return RETURN_CODE;
}