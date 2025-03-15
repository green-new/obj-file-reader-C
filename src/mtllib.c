#include "mtllib.h"
#include "token.h"
#include <string.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// Static utility functions
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

int mtllib_create(mtllib_t* lib) {
	int code = SUCCESS;
	if ((code = map_create(&lib->map)) != SUCCESS) {
		return code;
	}
	lib->name = NULL;
	return code;
}

void mtllib_destroy(mtllib_t* lib) {
	map_destroy(&lib->map);
	free((void*)lib->name);
}

void mtllib_print(mtllib_t* lib) {
	printf("Library Name: \"%s\"\n", lib->name);
	uint32_t used = map_size(&lib->map);
	printf("Used: %d\n", used);
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
        mtl_t* mtl = NULL;
        map_at(&lib->map, list.keys[i], &mtl);
		if (mtl) {
			mtl_fprint(file, mtl);
		}
	}
}

int mtllib_read(const char* fn, mtllib_t* lib) {
    FILE* file = fopen(fn, "r");
	char* mtltext; // mtl lib contents
    if (!file) {
        return INVALID_FILE;
    } else {
		// TODO Potential vulnerability: if the file contains a NUL character 
		// somewhere, this will throw off the 'mtltext' string, and could open 
		// up attacks
		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		fseek(file, 0, SEEK_SET);
		mtltext = malloc(length + 1);
		if (mtltext) {
			fread(mtltext, 1, length, file);
			mtltext[length] = '\0'; // fread doesn't null terminate
		}
		fclose(file);
	}
	token_list_t cmds;
	tokenize(&cmds, mtltext, "\n");
	token_node_t* pcmd = cmds.head;
	// the current material to build
	mtl_t curr_mat;
	while (pcmd) {
		token_list_t vars;
		// get list of parameters to this command
		ntokenize(&vars, buffer_start(pcmd->buf), pcmd->buf.length, " ");
		// copy cmd to string
		char cmd[256] = { 0 };
		buffer_get_strn(&pcmd->buf, cmd, pcmd->buf.length);
		// switch on cmd type
		token_node_t* pvar = vars.head;
		if (strequ(cmd, "newmtl")) {
			curr_mat = mtl_create();
			if (!pvar) {
				// Name not provided
				return PARSING_FAILURE;
			}
			// Keep copying a name until its done
			while (pvar) {
				buffer_get_strn(&pvar->buf, curr_mat.name, pvar->buf.length);
				pvar = pvar->next;
			}
		} else if (strequ(cmd, "Ka")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_float(&pvar->buf, &curr_mat.ambient[0]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.ambient[1]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.ambient[2]);
		} else if (strequ(cmd, "Kd")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_float(&pvar->buf, &curr_mat.diffuse[0]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.diffuse[1]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.diffuse[2]);
		} else if (strequ(cmd, "Ks")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_float(&pvar->buf, &curr_mat.specular[0]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.specular[1]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.specular[2]);
		} else if (strequ(cmd, "Tf")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_float(&pvar->buf, &curr_mat.tm_filter[0]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.tm_filter[1]);
			pvar = pvar->next;
			buffer_get_float(&pvar->buf, &curr_mat.tm_filter[2]);
		} else if (strequ(cmd, "illum")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_uint(&pvar->buf, &curr_mat.illum);
		} else if (strequ(cmd, "d")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			while (pvar) {
				if (strncmp(buffer_start(pvar->buf), "-halo", pvar->buf.length) == 0) {
					curr_mat.dissolve.halo = 1;
				} else {
					// Must be the factor
					buffer_get_float(&pvar->buf, &curr_mat.dissolve.value);
				}
				pvar = pvar->next;
			}
		} else if (strequ(cmd, "Ns")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_uint(&pvar->buf, &curr_mat.specular_exponent);
		} else if (strequ(cmd, "sharpness")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_uint(&pvar->buf, &curr_mat.sharpness);
		} else if (strequ(cmd, "Ni")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			// Get the values
			buffer_get_float(&pvar->buf, &curr_mat.optical_density);
		} else if (strequ(cmd, "map_Kd")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-cc")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.cc = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.cc = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ka.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ka.texres.h);
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.map_Ka.filename, MAX_MATERIAL_OPT_FILENAME);
			}
			pvar = pvar->next;
		} else if (strequ(cmd, "map_Kd")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-cc")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.cc = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.cc = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Kd.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Kd.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Kd.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ka.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ka.texres.h);
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.map_Ka.filename, MAX_MATERIAL_OPT_FILENAME);
			}
			pvar = pvar->next;
		} else if (strequ(cmd, "map_Ks")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Ks.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Ks.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Ks.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Ks.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-cc")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Ks.cc = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Ks.cc = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Ks.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Ks.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ks.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ks.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ks.texres.h);
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.map_Ks.filename, MAX_MATERIAL_OPT_FILENAME);
			}
			pvar = pvar->next;
		} else if (strequ(cmd, "map_Ns")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Ns.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Ns.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Ns.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Ns.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_Ns.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_Ns.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_Ns.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ns.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.map_Ns.texres.h);
			} else if (buffer_cmp(&pvar->buf, "-imfchan")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "r")) {
					curr_mat.map_Ns.imfchan = imfchan_r;
				} else if (buffer_cmp(&pvar->buf, "g")) {
					curr_mat.map_Ns.imfchan = imfchan_g;
				} else if (buffer_cmp(&pvar->buf, "b")) {
					curr_mat.map_Ns.imfchan = imfchan_b;
				} else if (buffer_cmp(&pvar->buf, "m")) {
					curr_mat.map_Ns.imfchan = imfchan_m;
				} else if (buffer_cmp(&pvar->buf, "l")) {
					curr_mat.map_Ns.imfchan = imfchan_l;
				} else if (buffer_cmp(&pvar->buf, "z")) {
					curr_mat.map_Ns.imfchan = imfchan_z;
				}
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.map_Ns.filename, MAX_MATERIAL_OPT_FILENAME);
			}
			pvar = pvar->next;
		} else if (strequ(cmd, "map_d")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_d.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_d.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_d.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_d.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.map_d.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.map_d.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.map_d.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.map_d.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.map_d.texres.h);
			} else if (buffer_cmp(&pvar->buf, "-imfchan")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "r")) {
					curr_mat.map_d.imfchan = imfchan_r;
				} else if (buffer_cmp(&pvar->buf, "g")) {
					curr_mat.map_d.imfchan = imfchan_g;
				} else if (buffer_cmp(&pvar->buf, "b")) {
					curr_mat.map_d.imfchan = imfchan_b;
				} else if (buffer_cmp(&pvar->buf, "m")) {
					curr_mat.map_d.imfchan = imfchan_m;
				} else if (buffer_cmp(&pvar->buf, "l")) {
					curr_mat.map_d.imfchan = imfchan_l;
				} else if (buffer_cmp(&pvar->buf, "z")) {
					curr_mat.map_d.imfchan = imfchan_z;
				}
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.map_d.filename, MAX_MATERIAL_OPT_FILENAME);
			}
			pvar = pvar->next;
		} else if (strequ(cmd, "map_aat")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "on")) {
				curr_mat.map_aat = 1;
			}
			pvar = pvar->next;
		} else if (strequ(cmd, "decal")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.decal.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.decal.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.decal.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.decal.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.decal.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.decal.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.decal.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.decal.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.decal.texres.h);
			} else if (buffer_cmp(&pvar->buf, "-imfchan")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "r")) {
					curr_mat.decal.imfchan = imfchan_r;
				} else if (buffer_cmp(&pvar->buf, "g")) {
					curr_mat.decal.imfchan = imfchan_g;
				} else if (buffer_cmp(&pvar->buf, "b")) {
					curr_mat.decal.imfchan = imfchan_b;
				} else if (buffer_cmp(&pvar->buf, "m")) {
					curr_mat.decal.imfchan = imfchan_m;
				} else if (buffer_cmp(&pvar->buf, "l")) {
					curr_mat.decal.imfchan = imfchan_l;
				} else if (buffer_cmp(&pvar->buf, "z")) {
					curr_mat.decal.imfchan = imfchan_z;
				}
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.decal.filename, MAX_MATERIAL_OPT_FILENAME);
			}
			pvar = pvar->next;
		} else if (strequ(cmd, "disp")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.disp.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.disp.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.disp.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.disp.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.disp.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.disp.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.disp.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.disp.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.disp.texres.h);
			} else if (buffer_cmp(&pvar->buf, "-imfchan")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "r")) {
					curr_mat.disp.imfchan = imfchan_r;
				} else if (buffer_cmp(&pvar->buf, "g")) {
					curr_mat.disp.imfchan = imfchan_g;
				} else if (buffer_cmp(&pvar->buf, "b")) {
					curr_mat.disp.imfchan = imfchan_b;
				} else if (buffer_cmp(&pvar->buf, "m")) {
					curr_mat.disp.imfchan = imfchan_m;
				} else if (buffer_cmp(&pvar->buf, "l")) {
					curr_mat.disp.imfchan = imfchan_l;
				} else if (buffer_cmp(&pvar->buf, "z")) {
					curr_mat.disp.imfchan = imfchan_z;
				}
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.disp.filename, MAX_MATERIAL_OPT_FILENAME);
			}
		} else if (strequ(cmd, "bump")) {
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.bump.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.bump.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.bump.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.bump.blendv = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					curr_mat.bump.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					curr_mat.bump.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &curr_mat.bump.texres.w);
				buffer_get_uint(&pvar->buf, &curr_mat.bump.texres.h);
			} else if (buffer_cmp(&pvar->buf, "-imfchan")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "r")) {
					curr_mat.bump.imfchan = imfchan_r;
				} else if (buffer_cmp(&pvar->buf, "g")) {
					curr_mat.bump.imfchan = imfchan_g;
				} else if (buffer_cmp(&pvar->buf, "b")) {
					curr_mat.bump.imfchan = imfchan_b;
				} else if (buffer_cmp(&pvar->buf, "m")) {
					curr_mat.bump.imfchan = imfchan_m;
				} else if (buffer_cmp(&pvar->buf, "l")) {
					curr_mat.bump.imfchan = imfchan_l;
				} else if (buffer_cmp(&pvar->buf, "z")) {
					curr_mat.bump.imfchan = imfchan_z;
				}
			} else if (buffer_cmp(&pvar->buf, "-bm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &curr_mat.bump.bm);
			} else {
				// Must be the filename
				buffer_get_strn(&pvar->buf, curr_mat.bump.filename, MAX_MATERIAL_OPT_FILENAME);
			}
		} else if (strequ(cmd, "refl")) {
			pvar = pvar->next;
			refl_opts_t opts;
			if (!pvar) {
				// Parameters not provided
				return PARSING_FAILURE;
			}
			if (buffer_cmp(&pvar->buf, "-blendu")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					opts.blendu = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					opts.blendu = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-blendv")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					opts.blendv = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					opts.blendv = 1;
				}
			}  else if (buffer_cmp(&pvar->buf, "-cc")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					opts.cc = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					opts.cc = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-clamp")) {
				pvar = pvar->next;
				if (buffer_cmp(&pvar->buf, "on")) {
					opts.clamp = 1;
				} else if (buffer_cmp(&pvar->buf, "off")) {
					opts.clamp = 1;
				}
			} else if (buffer_cmp(&pvar->buf, "-mm")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.mm.base);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.mm.gain);
			} else if (buffer_cmp(&pvar->buf, "-o")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.offset[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.offset[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.offset[2]);
			} else if (buffer_cmp(&pvar->buf, "-s")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.scale[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.scale[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.scale[2]);
			} else if (buffer_cmp(&pvar->buf, "-t")) {
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.turbulence[0]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.turbulence[1]);
				pvar = pvar->next;
				buffer_get_float(&pvar->buf, &opts.turbulence[2]);
			} else if (buffer_cmp(&pvar->buf, "-texres")) {
				pvar = pvar->next;
				// width and height should be equivalent.
				buffer_get_uint(&pvar->buf, &opts.texres.w);
				buffer_get_uint(&pvar->buf, &opts.texres.h);
			}
			if (curr_mat.refl_map.head != NULL) {
				refl_append(&curr_mat.refl_map, opts);
			} else {
				refl_create(&curr_mat.refl_map, opts);
			}
		} else {
			// Unsupported command detected
			return PARSING_FAILURE;
		}
		// Go to the next command
		pcmd = pcmd->next;
	}
	free(mtltext);

    return SUCCESS;
}