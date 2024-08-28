#include "mtllib.h"

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

mtllib_t mtllib_create(void) {
	mtllib_t lib = {
		.name = NULL,
		.map = (mat_map) {0}
	};
	return lib;
}

void mtllib_destroy(mtllib_t* lib) {
	map_destroy(&lib->map);
	free(lib->name);
}

void mtllib_print(mtllib_t* lib) {
	printf("Library Name: \"%s\"", lib->name);
	uint32_t used = map_size(&lib->map);
	printf("Used: %ud", used);
	keys_list_t list = map_keys(&lib->map);
	for (uint32_t i = 0; i < list.used; i++) {
		printf("\t\"%s\"", list.keys[i]);
	}
	keys_list_destroy(&list);
}

void mtllib_fprint(FILE* file, mtllib_t* lib) {
	fprintf(file, "Library Name: \"%s\"", lib->name);
	uint32_t used = map_size(&lib->map);
	fprintf(file, "Used: %ud", used);
	keys_list_t list = map_keys(&lib->map);
	for (uint32_t i = 0; i < list.used; i++) {
		mtl_fprint(file, list.keys[i]);
	}
}

int mtllib_read(const char* fn, mtllib_t* lib) {
    mtllib_destroy(lib);
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
        mtl_t* curr_mat;
        // TODO: bounds checks on certain values
        if (strequ(type, "newmtl")) {
			char* name = strtok(line_buffer, NULL);
			mtl_t new_mat = mtl_create();
			if (!map_insert(&lib->map, name, new_mat)) {
				return MEMORY_REFUSED;
			}
			map_at(&lib->map, name, curr_mat);
        } else if (strequ(type, "Ka")) {
            init_3f(curr_mat.ambient);
        } else if (strequ(type, "Kd")) {
            init_3f(curr_mat.diffuse);
        } else if (strequ(type, "Ks")) {
            init_3f(curr_mat.specular);
        } else if (strequ(type, "Tf")) {
            init_3f(curr_mat.tm_filter);
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

    return SUCCESS;
}