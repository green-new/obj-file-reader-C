#include "refl.h"
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
	printf("Material Name: \"%s\"\n", mat->name);
	printf("\tAmbient (Ka): [%f, %f, %f]\n",
		mat->ambient[x], mat->ambient[y], mat->ambient[z]);
	printf("\tDiffuse (Kd): [%f, %f, %f]\n", 
		mat->diffuse[x], mat->diffuse[y], mat->diffuse[z]);
	printf("\tSpecular (Ks): [%f, %f, %f]\n", 
		mat->specular[x], mat->specular[y], mat->specular[z]);
	printf("\tTransmission filter (Tf): [%f, %f, %f]\n", 
		mat->tm_filter[x], mat->tm_filter[y], mat->tm_filter[z]);
	printf("\tIllum: [%ud]\n", mat->illum);
	printf("\tDissolve (d): [%f], Halo: [%d]\n", mat->dissolve.value, 
		mat->dissolve.halo);
	printf("\tSpecular exponent: [%d]\n", mat->specular_exponent);
	printf("\tSharpness: [%d]\n", mat->sharpness);
	printf("\tOptical density: [%d]\n", mat->optical_density);
	printf("\tmap_Ka: [\"%s\"]\n", mat->map_Ka.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->map_Ka.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->map_Ka.blendv);
	printf("\t\tColor correction (cc): [%d]\n", mat->map_Ka.cc);
	printf("\t\tClamping (clamp): [%d]\n", mat->map_Ka.clamp);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->map_Ka.mm.base, 
		mat->map_Ka.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Ka.offset[0],
		mat->map_Ka.offset[1], mat->map_Ka.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Ka.scale[0],
		mat->map_Ka.scale[1], mat->map_Ka.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->map_Ka.turbulence[0],
		mat->map_Ka.turbulence[1], mat->map_Ka.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Ka.texres.w, mat->map_Ka.texres.h);
	printf("\tmap_Kd: [\"%s\"]\n", mat->map_Kd.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->map_Kd.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->map_Kd.blendv);
	printf("\t\tColor correction (cc): [%d]\n", mat->map_Kd.cc);
	printf("\t\tClamping (clamp): [%d]\n", mat->map_Kd.clamp);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->map_Kd.mm.base, 
		mat->map_Kd.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Kd.offset[0],
		mat->map_Kd.offset[1], mat->map_Kd.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Kd.scale[0],
		mat->map_Kd.scale[1], mat->map_Kd.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->map_Kd.turbulence[0],
		mat->map_Kd.turbulence[1], mat->map_Kd.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Kd.texres.w, mat->map_Kd.texres.h);
	printf("\tmap_Ks: [\"%s\"]\n", mat->map_Ks.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->map_Ks.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->map_Ks.blendv);
	printf("\t\tColor correction (cc): [%d]\n", mat->map_Ks.cc);
	printf("\t\tClamping (clamp): [%d]\n", mat->map_Ks.clamp);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->map_Ks.mm.base, 
		mat->map_Ks.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Ks.offset[0],
		mat->map_Ks.offset[1], mat->map_Ks.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Ks.scale[0],
		mat->map_Ks.scale[1], mat->map_Ks.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->map_Ks.turbulence[0],
		mat->map_Ks.turbulence[1], mat->map_Ks.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Ks.texres.w, mat->map_Ks.texres.h);
	printf("\tmap_Ns: [\"%s\"]\n", mat->map_Ns.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->map_Ns.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->map_Ns.blendv);
	printf("\t\tClamping (clamp): [%d]\n", mat->map_Ns.clamp);
	printf("\t\tImfchan: [%d]\n", mat->map_Ns.imfchan);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->map_Ns.mm.base, 
		mat->map_Ns.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Ns.offset[0],
		mat->map_Ns.offset[1], mat->map_Ns.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Ns.scale[0],
		mat->map_Ns.scale[1], mat->map_Ns.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->map_Ns.turbulence[0],
		mat->map_Ns.turbulence[1], mat->map_Ns.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Ns.texres.w, mat->map_Ns.texres.h);
	printf("\tmap_d: [\"%s\"]\n", mat->map_d.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->map_d.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->map_d.blendv);
	printf("\t\tClamping (clamp): [%d]\n", mat->map_d.clamp);
	printf("\t\tImfchan: [%d]\n", mat->map_d.imfchan);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->map_d.mm.base, 
		mat->map_d.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_d.offset[0],
		mat->map_d.offset[1], mat->map_d.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_d.scale[0],
		mat->map_d.scale[1], mat->map_d.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->map_d.turbulence[0],
		mat->map_d.turbulence[1], mat->map_d.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_d.texres.w, mat->map_d.texres.h);
	printf("\tdecal: [\"%s\"]\n", mat->decal.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->decal.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->decal.blendv);
	printf("\t\tClamping (clamp): [%d]\n", mat->decal.clamp);
	printf("\t\tImfchan: [%d]\n", mat->decal.imfchan);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->decal.mm.base, 
		mat->decal.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->decal.offset[0],
		mat->decal.offset[1], mat->decal.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->decal.scale[0],
		mat->decal.scale[1], mat->decal.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->decal.turbulence[0],
		mat->decal.turbulence[1], mat->decal.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->decal.texres.w, mat->decal.texres.h);
	printf("\tdisp: [\"%s\"]", mat->disp.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->disp.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->disp.blendv);
	printf("\t\tClamping (clamp): [%d]\n", mat->disp.clamp);
	printf("\t\tImfchan: [%d]\n", mat->disp.imfchan);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->disp.mm.base, 
		mat->disp.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->disp.offset[0],
		mat->disp.offset[1], mat->disp.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->disp.scale[0],
		mat->disp.scale[1], mat->disp.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->disp.turbulence[0],
		mat->disp.turbulence[1], mat->disp.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->disp.texres.w, mat->disp.texres.h);
	printf("\tbump: [\"%s\"]", mat->bump.filename);
	printf("\t\tHorizontal blending (blendu): [%d]\n", mat->bump.blendu);
	printf("\t\tVertical blending (blendv): [%d]\n", mat->bump.blendv);
	printf("\t\tClamping (clamp): [%d]\n", mat->bump.clamp);
	printf("\t\tImfchan: [%d]\n", mat->bump.imfchan);
	printf("\t\tContrast range (mm): [%f - %f]\n", mat->bump.mm.base, 
		mat->bump.mm.gain);
	printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->bump.offset[0],
		mat->bump.offset[1], mat->bump.offset[2]);
	printf("\t\tScale (s; xyz): [%f, %f, %f]\n", mat->bump.scale[0],
		mat->bump.scale[1], mat->bump.scale[2]);
	printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", mat->bump.turbulence[0],
		mat->bump.turbulence[1], mat->bump.turbulence[2]);
	printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->bump.texres.w, mat->bump.texres.h);
	printf("\tReflection map (refl):\n", mat->bump.texres.w, 
		mat->bump.texres.h);
	refl_node_t* p = mat->refl_map.head;
	while (p) {
		printf("\tFilename: [\"%s\"]\n", p->options.filename);
		printf("\tType: [%d]\n", p->options.type);
		printf("\tHorizontal blending (blendu): [%d]\n", 
			p->options.blendu);
		printf("\tVertical blending (blendv): [%d]\n", 
			p->options.blendv);
		printf("\tColor correction (cc): [%d]\n", p->options.cc);
		printf("\tClamping (clamp): [%d]\n", p->options.clamp);
		printf("\t\tContrast range (mm): [%f - %f]\n", p->options.mm.base, 
			p->options.mm.gain);
		printf("\t\tOffset (o; xyz): [%f, %f, %f]\n", p->options.offset[0],
			p->options.offset[1], p->options.offset[2]);
		printf("\t\tScale (s; xyz): [%f, %f, %f]\n", p->options.scale[0],
			p->options.scale[1], p->options.scale[2]);
		printf("\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
			p->options.turbulence[0], p->options.turbulence[1], 
			p->options.turbulence[2]);
		printf("\t\tResolution (texres; width x height): [%d, %d]\n", 
			p->options.texres.w, p->options.texres.h);
		p = p->next;
	}	
	#undef x
	#undef y
	#undef z
}

void mtl_fprint(FILE* file, mtl_t* mat) {
	#define x 0
	#define y 1
	#define z 2
	fprintf(file, "Material Name: \"%s\"\n", mat->name);
	fprintf(file, "\tAmbient (Ka): [%f, %f, %f]\n",
		mat->ambient[x], mat->ambient[y], mat->ambient[z]);
	fprintf(file, "\tDiffuse (Kd): [%f, %f, %f]\n", 
		mat->diffuse[x], mat->diffuse[y], mat->diffuse[z]);
	fprintf(file, "\tSpecular (Ks): [%f, %f, %f]\n", 
		mat->specular[x], mat->specular[y], mat->specular[z]);
	fprintf(file, "\tTransmission filter (Tf): [%f, %f, %f]\n", 
		mat->tm_filter[x], mat->tm_filter[y], mat->tm_filter[z]);
	fprintf(file, "\tIllum: [%ud]\n", mat->illum);
	fprintf(file, "\tDissolve (d): [%f], Halo: [%d]\n", mat->dissolve.value, 
		mat->dissolve.halo);
	fprintf(file, "\tSpecular exponent: [%d]\n", mat->specular_exponent);
	fprintf(file, "\tSharpness: [%d]\n", mat->sharpness);
	fprintf(file, "\tOptical density: [%d]\n", mat->optical_density);
	fprintf(file, "\tmap_Ka: [\"%s\"]\n", mat->map_Ka.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", 
		mat->map_Ka.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->map_Ka.blendv);
	fprintf(file, "\t\tColor correction (cc): [%d]\n", mat->map_Ka.cc);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->map_Ka.clamp);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->map_Ka.mm.base, 
		mat->map_Ka.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Ka.offset[0],
		mat->map_Ka.offset[1], mat->map_Ka.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Ka.scale[0],
		mat->map_Ka.scale[1], mat->map_Ka.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->map_Ka.turbulence[0], mat->map_Ka.turbulence[1], 
		mat->map_Ka.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Ka.texres.w, mat->map_Ka.texres.h);
	fprintf(file, "\tmap_Kd: [\"%s\"]\n", mat->map_Kd.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", 
		mat->map_Kd.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->map_Kd.blendv);
	fprintf(file, "\t\tColor correction (cc): [%d]\n", mat->map_Kd.cc);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->map_Kd.clamp);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->map_Kd.mm.base, 
		mat->map_Kd.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Kd.offset[0],
		mat->map_Kd.offset[1], mat->map_Kd.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Kd.scale[0],
		mat->map_Kd.scale[1], mat->map_Kd.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->map_Kd.turbulence[0], mat->map_Kd.turbulence[1], 
		mat->map_Kd.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Kd.texres.w, mat->map_Kd.texres.h);
	fprintf(file, "\tmap_Ks: [\"%s\"]\n", mat->map_Ks.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", 
		mat->map_Ks.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->map_Ks.blendv);
	fprintf(file, "\t\tColor correction (cc): [%d]\n", mat->map_Ks.cc);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->map_Ks.clamp);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->map_Ks.mm.base, 
		mat->map_Ks.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Ks.offset[0],
		mat->map_Ks.offset[1], mat->map_Ks.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Ks.scale[0],
		mat->map_Ks.scale[1], mat->map_Ks.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->map_Ks.turbulence[0], mat->map_Ks.turbulence[1], 
		mat->map_Ks.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Ks.texres.w, mat->map_Ks.texres.h);
	fprintf(file, "\tmap_Ns: [\"%s\"]\n", mat->map_Ns.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", 
		mat->map_Ns.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->map_Ns.blendv);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->map_Ns.clamp);
	fprintf(file, "\t\tImfchan: [%d]\n", mat->map_Ns.imfchan);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->map_Ns.mm.base, 
		mat->map_Ns.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_Ns.offset[0],
		mat->map_Ns.offset[1], mat->map_Ns.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_Ns.scale[0],
		mat->map_Ns.scale[1], mat->map_Ns.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->map_Ns.turbulence[0], mat->map_Ns.turbulence[1], 
		mat->map_Ns.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_Ns.texres.w, mat->map_Ns.texres.h);
	fprintf(file, "\tmap_d: [\"%s\"]\n", mat->map_d.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", 
		mat->map_d.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->map_d.blendv);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->map_d.clamp);
	fprintf(file, "\t\tImfchan: [%d]\n", mat->map_d.imfchan);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->map_d.mm.base, 
		mat->map_d.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->map_d.offset[0],
		mat->map_d.offset[1], mat->map_d.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->map_d.scale[0],
		mat->map_d.scale[1], mat->map_d.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->map_d.turbulence[0], mat->map_d.turbulence[1], 
		mat->map_d.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->map_d.texres.w, mat->map_d.texres.h);
	fprintf(file, "\tdecal: [\"%s\"]\n", mat->decal.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", 
		mat->decal.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->decal.blendv);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->decal.clamp);
	fprintf(file, "\t\tImfchan: [%d]\n", mat->decal.imfchan);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->decal.mm.base, 
		mat->decal.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->decal.offset[0],
		mat->decal.offset[1], mat->decal.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->decal.scale[0],
		mat->decal.scale[1], mat->decal.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->decal.turbulence[0], mat->decal.turbulence[1], 
		mat->decal.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->decal.texres.w,  mat->decal.texres.h);
	fprintf(file, "\tdisp: [\"%s\"]", mat->disp.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", mat->disp.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->disp.blendv);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->disp.clamp);
	fprintf(file, "\t\tImfchan: [%d]\n", mat->disp.imfchan);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->disp.mm.base, 
		mat->disp.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->disp.offset[0],
		mat->disp.offset[1], mat->disp.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->disp.scale[0],
		mat->disp.scale[1], mat->disp.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->disp.turbulence[0], mat->disp.turbulence[1], 
		mat->disp.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->disp.texres.w, mat->disp.texres.h);
	fprintf(file, "\tbump: [\"%s\"]", mat->bump.filename);
	fprintf(file, "\t\tHorizontal blending (blendu): [%d]\n", mat->bump.blendu);
	fprintf(file, "\t\tVertical blending (blendv): [%d]\n", mat->bump.blendv);
	fprintf(file, "\t\tClamping (clamp): [%d]\n", mat->bump.clamp);
	fprintf(file, "\t\tImfchan: [%d]\n", mat->bump.imfchan);
	fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", mat->bump.mm.base, 
		mat->bump.mm.gain);
	fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", mat->bump.offset[0],
		mat->bump.offset[1], mat->bump.offset[2]);
	fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", mat->bump.scale[0],
		mat->bump.scale[1], mat->bump.scale[2]);
	fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
		mat->bump.turbulence[0], mat->bump.turbulence[1], 
		mat->bump.turbulence[2]);
	fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
		mat->bump.texres.w, mat->bump.texres.h);
	fprintf(file, "\tReflection map (refl):\n", mat->bump.texres.w, 
		mat->bump.texres.h);
	refl_node_t* p = mat->refl_map.head;
	while (p) {
		fprintf(file, "\tFilename: [\"%s\"]\n", p->options.filename);
		fprintf(file, "\tType: [%d]\n", p->options.type);
		fprintf(file, "\tHorizontal blending (blendu): [%d]\n", 
			p->options.blendu);
		fprintf(file, "\tVertical blending (blendv): [%d]\n", 
			p->options.blendv);
		fprintf(file, "\tColor correction (cc): [%d]\n", p->options.cc);
		fprintf(file, "\tClamping (clamp): [%d]\n", p->options.clamp);
		fprintf(file, "\t\tContrast range (mm): [%f - %f]\n", 
			p->options.mm.base, p->options.mm.gain);
		fprintf(file, "\t\tOffset (o; xyz): [%f, %f, %f]\n", 
			p->options.offset[0], p->options.offset[1], p->options.offset[2]);
		fprintf(file, "\t\tScale (s; xyz): [%f, %f, %f]\n", 
			p->options.scale[0], p->options.scale[1], p->options.scale[2]);
		fprintf(file, "\t\tTurbulence (t; xyz): [%f, %f, %f]\n", 
			p->options.turbulence[0], p->options.turbulence[1], 
			p->options.turbulence[2]);
		fprintf(file, "\t\tResolution (texres; width x height): [%d, %d]\n", 
			p->options.texres.w, p->options.texres.h);
		p = p->next;
	}
	#undef x
	#undef y
	#undef z
}