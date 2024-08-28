#include "mtl.h"

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

mtl_t mtl_create(void) {
    mtl_t material = {
        .name = NULL,
        .ambient = {0.0f, 0.0f, 0.0f},
        .diffuse = {0.0f, 0.0f, 0.0f},
        .specular = {0.0f, 0.0f, 0.0f},
        .tm_filter = {0.0f, 0.0f, 0.0f},
        .illum = 0,
        .dissolve = {.value = 0, .halo = 0},
        .specular_exponent = 0,
        .sharpness = 0,
        .optical_density = 0,
		.map_Ka = {.filename = NULL, .blendu = 0, .blendv = 0, .cc = 0, 
			.clamp = 0, .mm = {.base = 0.0f, .gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.map_Kd = {.filename = NULL, .blendu = 0, .blendv = 0, .cc = 0, 
			.clamp = 0, .mm = {.base = 0.0f, .gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.map_Ks = {.filename = NULL, .blendu = 0, .blendv = 0, .cc = 0, 
			.clamp = 0, .mm = {.base = 0.0f, .gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.map_Ns = {.filename = NULL, .blendu = 0, .blendv = 0, .clamp = 0, 
			.imfchan = (imfchan_t) l, .mm = {.base = 0.0f, .gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.map_d = {.filename = NULL, .blendu = 0, .blendv = 0, .clamp = 0, 
			.imfchan = (imfchan_t) l, .mm = {.base = 0.0f, .gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.decal = {.filename = NULL, .blendu = 0, .blendv = 0, .clamp = 0, 
			.imfchan = (imfchan_t) l, .mm = {.base = 0.0f, .gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.disp = {.filename = NULL, .blendu = 0, .blendv = 0, .clamp = 0, 
			.imfchan = (imfchan_t) l, .mm = {.base = 0.0f, .gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.bump = {.filename = NULL, .bm = 0.0f, .blendu = 0, .blendv = 0, 
			.clamp = 0, .imfchan = (imfchan_t) l, .mm = {.base = 0.0f, 
				.gain = 0.0f}, 
			.offset = {0.0f, 0.0f, 0.0f}, .scale = {0.0f, 0.0f, 0.0f}, 
			.turbulence = {0.0f, 0.0f, 0.0f}, .texres = {.w = 0, .h = 0}
		},
		.refl_map = { .head = NULL, .used = 0 }
    };
    return material;
}

void mtl_destroy(mtl_t* material) {
	free(material->name);
}

void mtl_print(mtl_t* mat) {
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
}

void mtl_fprint(FILE* file, mtl_t* mat) {
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
}