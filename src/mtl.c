#include "mtl.h"

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

typedef struct {
	/** Name of this material type. */
    char* name;
	/** Specifies the ambient reflectivity using rgb values 
	* scaled from [0, 1]. */
    double ambient[3];
	/** Specifies the diffuse reflectivity using rgb values scaled from 
	* [0, 1]. */
    double diffuse[3];
	/** Specifies the specular reflectivity using rgb values scaled from 
	* [0, 1]. */
    double specular[3];
	/** Specifies the transmission filter of this material using rgb values 
	* scaled from [0, 1]. */
    double tm_filter[3];
	/** Specifies the illumination model to use. Value between 0-10, see 
	* complete descriptions in illum.h. */
    uint32_t illum;
	/** Specifies the "dissolve", or more well known as transparency, of this 
	* material. */
    double transparency;
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
    double optical_density;
} mtl_t;

typedef struct {
	/** Value of this material library. */
    char* name;
	/** Map of [material name, material] key/value pairs. */
    mat_map map;
} mtllib_t;

mtl_t mtl_create(void) {
    mtl_t material = {
        .name = "",
        .ambient = {0.0f, 0.0f, 0.0f},
        .diffuse = {0.0f, 0.0f, 0.0f},
        .specular = {0.0f, 0.0f, 0.0f},
        .tm_filter = {0.0f, 0.0f, 0.0f},
        .illum = 0,
        .transparency = 0,
        .specular_exponent = 0,
        .sharpness = 0,
        .optical_density = 0
    };
    return material;
}

void mtl_destroy(mtl_t* mat) {
	if (mat) {
		free(material->name);
	}
}

void mtl_print(mtl_t* mat) {
	if (mat) {
		#define x 0
		#define y 1
		#define z 2
		printf("Material Name: \"%s\"", mat->name);
		printf("Ambient: [%f, %f, %f]", 
			mat->ambient[x], mat->ambient[y], mat->ambient[z]);
		printf("Diffuse: [%f, %f, %f]", 
			mat->diffuse[x], mat->diffuse[y], mat->diffuse[z]);
		printf("Specular: [%f, %f, %f]", 
			mat->specular[x], mat->specular[y], mat->specular[z]);
		printf("Transmission filter: [%f, %f, %f]", 
			mat->tm_filter[x], mat->tm_filter[y], mat->tm_filter[z]);
		printf("Illum: %ud", mat->illum);
		printf("Transparency: %f", mat->transparency);
		printf("Specular exponent: %d", mat->specular_exponent);
		printf("Sharpness: %d", mat->sharpness);
		printf("Optical density: %d", mat->optical_density);		
		#undef x
		#undef y
		#undef z
	} else {
		// TODO: Error callback
		printf("Error: attempted to output material properties to standard \
		output, but material object was invalid or deleted");
	}
}

void mtl_fprint(FILE* file, mtl_t* mat) {
	if (mat && file) {
		#define x 0
		#define y 1
		#define z 2
		fprintf(file, "Material Name: \"%s\"", mat->name);
		fprintf(file, "Ambient: [%f, %f, %f]",
			mat->ambient[x], mat->ambient[y], mat->ambient[z]);
		fprintf(file, "Diffuse: [%f, %f, %f]", 
			mat->diffuse[x], mat->diffuse[y], mat->diffuse[z]);
		fprintf(file, "Specular: [%f, %f, %f]", 
			mat->specular[x], mat->specular[y], mat->specular[z]);
		fprintf(file, "Transmission filter: [%f, %f, %f]", 
			mat->tm_filter[x], mat->tm_filter[y], mat->tm_filter[z]);
		fprintf(file, "Illum: %ud", mat->illum);
		fprintf(file, "Transparency: %f", mat->transparency);
		fprintf(file, "Specular exponent: %d", mat->specular_exponent);
		fprintf(file, "Sharpness: %d", mat->sharpness);
		fprintf(file, "Optical density: %d", mat->optical_density);		
		#undef x
		#undef y
		#undef z
	} else {
		// TODO: Error callback
		printf("Error: attempted to output material properties to file, but \
		material object was invalid or deleted");
	}
}

mtllib_t mtllib_create(void) {
	mtllib_t lib = {
		.name = "",
		.mtl_arr = 0,
		.used = 0
	};
	return lib;
}

void mtllib_destroy(mtllib_t* lib) {
	if (lib) {
		if (lib->used > 0) {
			for (uint8_t i = 0; i < lib->used; i++) {
				mtl_destroy(&lib->lib[i]);
			}
		}
		lib->used = 0;
		free(lib->name);
	}
}

void mtllib_print(mtllib_t* lib) {
	if (lib) {
		printf("Library Name: \"%s\"", lib->name);
		printf("Used: %ud", used);
		for (uint8_t i = 0; i < used; i++) {
			mtl_print(&lib->mtl_arr[i]);
		}
	} else {
		// TODO: Error callback
		printf("Error: attempted to output material library properties to \
		standard output, but material library object was invalid or deleted");
	}
}

void mtllib_fprint(FILE* file, mtllib_t* lib) {
	if (lib) {
		printf("Library Name: \"%s\"", lib->name);
		printf("Used: %ud", used);
		for (uint8_t i = 0; i < used; i++) {
			mtl_fprint(file, &lib->mtl_arr[i]);
		}
	} else {
		// TODO: Error callback
		printf("Error: attempted to output material library properties to \
		file, but material library object was invalid or deleted");
	}
}

int mtllib_read(const char* fn, mtllib_t* arr) {
    mtllib_destroy(arr);
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
            arr->material_array[arr->used] = mtl_create();
			char* name = strtok(line_buffer, NULL);
			arr->material_array[arr->used].name = name;
			curr_mat = &arr->material_array[arr->used];
            arr->used++;
        } else if (strequ(type, "Ka")) {
            // TODO: support "xyz" input, not just "x y z"
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

    return RETURN_CODE;
}