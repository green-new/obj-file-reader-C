#include "mtllib.h"
#include "token.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
// Static utility functions
// -----------------------------------------------------------------------------

static int prefixequ(const char* str, const char* pre) {
	// Trim leading spaces
	while (isspace((unsigned char)*str)) {
		str++;
	}
	// Check if both are null just incase
	if (*str == '\0' && *pre == '\0') {
		return 1;
	}
	return strncmp(pre, str, strlen(pre)) == 0;
}

typedef enum MtlParseListFlags {
	MTL_PARSE_LIST_ALLOW_ONE_VALUE = 0x1
} MtlParseListFlags;

typedef struct MtlParseListConfig {
	unsigned long long flags;
	void* elements;
	unsigned int num_elements;
	unsigned int line_number;
	type_t element_type;
	token_list_t params;
} MtlParseListConfig;

int
mtl_read_from_list(
	MtlParseListConfig conf
) {
	if (
		conf.params.used != conf.num_elements &&
		!(conf.flags & MTL_PARSE_LIST_ALLOW_ONE_VALUE)
	) {
		printf(
			"Error parsing material: invalid number of arguments on line %u\n",
			conf.line_number
		);
		return PARSING_FAILURE;
	}
	
	if (
		tokenlist_read_void(
			conf.num_elements, 
			conf.params, 
			conf.element_type, 
			conf.elements,
			conf.flags & MTL_PARSE_LIST_ALLOW_ONE_VALUE
		) != SUCCESS) {
		printf("Error parsing material: fatal error on line %u\n", conf.line_number);
		return PARSING_FAILURE;
	}
}

typedef enum MtlParamFlags {
	MTL_PARSE_TARGET_HALO = 0x1,
	MTL_PARSE_TARGET_BLENDU = 0x2,
	MTL_PARSE_TARGET_BLENDV = 0x4,
	MTL_PARSE_TARGET_CC = 0x8,
	MTL_PARSE_TARGET_CLAMP = 0x10,
	MTL_PARSE_TARGET_MM = 0x20,
	MTL_PARSE_TARGET_OFFSET = 0x40,
	MTL_PARSE_TARGET_SCALE = 0x80,
	MTL_PARSE_TARGET_TURB = 0x100,
	MTL_PARSE_TARGET_TEXRES = 0x200,
	MTL_PARSE_TARGET_IMFCHAN = 0x800,
	MTL_PARSE_TARGET_BM = 0x1000,
	MTL_PARSE_TARGET_REFLMAP_TYPE = 0x2000
} MtlParamFlags;

typedef struct MtlParserInfo {
	unsigned long flags;
	int* haloTarget;
	int* blenduTarget;
	int* blendvTarget;
	int* ccTarget;
	int* clampTarget;
	mm_t* mmTarget;
	offset_t* offsetTarget;
	scale_t* scaleTarget;
	turb_t* turbTarget;
	texres_t* texresTarget;
	filename_t* filenameTarget;
	imfchan_t* imfchanTarget;
	bm_t* bmTarget;
	refl_type_t* reflmapTypeTarget;
} MtlParserInfo;

int
mtl_parse_flags(
	token_list_t flaglist,
	MtlParserInfo info
) {
	token_node_t* p = flaglist.head;

	if (!p || flaglist.used <= 0) {
		return PARSING_FAILURE;
	}

	// halo parameter
	if (info.flags & MTL_PARSE_TARGET_HALO) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-halo")) {
				// -halo exists and takes no parameters
				*(info.haloTarget) = 1;
			}
			p = p->next;
		}
	}

	p = flaglist.head;

	// blendu parameter
	if (
		(
			info.flags & MTL_PARSE_TARGET_BLENDU 
			|| info.flags & MTL_PARSE_TARGET_BLENDV
		) && (
			info.blenduTarget
			|| info.blendvTarget
		)) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-blendu")) {
				// determine if the flag arguments are "on" or "off"
				char arg[255] = { 0 };
				token_node_t* argp = p->next;

				if (!argp) {
					return PARSING_FAILURE;
				}
				
				buffer_get_str(argp->buf, arg);
				if (strequ(arg, "on")) {
					if (info.flags & MTL_PARSE_TARGET_BLENDU) {
						*(info.blenduTarget) = 1;
					} else if (info.flags & MTL_PARSE_TARGET_BLENDV) {
						*(info.blendvTarget) = 1;
					}
				} else if (strequ(arg, "off")) {
					if (info.flags & MTL_PARSE_TARGET_BLENDU) {
						*(info.blenduTarget) = 0;
					} else if (info.flags & MTL_PARSE_TARGET_BLENDV) {
						*(info.blendvTarget) = 0;
					}
				} else {
					return PARSING_FAILURE;
				}
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// cc (color correction)
	if (info.flags & MTL_PARSE_TARGET_CC && info.ccTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-cc")) {
				// determine if the flag arguments are "on" or "off"
				char arg[16] = { 0 };
				token_node_t* argp = p->next;

				if (!argp) {
					return PARSING_FAILURE;
				}
				
				buffer_get_str(argp->buf, arg);
				if (strequ(arg, "on")) {
					*(info.ccTarget) = 1;
				} else if (strequ(arg, "off")) {
					*(info.ccTarget) = 0;
				} else {
					return PARSING_FAILURE;
				}
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// clamp
	if (info.flags & MTL_PARSE_TARGET_CLAMP && info.clampTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-clamp")) {
				// determine if the flag arguments are "on" or "off"
				char arg[255] = { 0 };
				token_node_t* argp = p->next;

				if (!argp) {
					return PARSING_FAILURE;
				}
				
				buffer_get_str(argp->buf, arg);
				if (strequ(arg, "on")) {
					*(info.ccTarget) = 1;
				} else if (strequ(arg, "off")) {
					*(info.ccTarget) = 0;
				} else {
					return PARSING_FAILURE;
				}
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// mm
	if (info.flags & MTL_PARSE_TARGET_MM && info.mmTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-mm")) {
				// get the "base" and the "gain"
				token_node_t* base = p->next;
				if (!base) {
					return PARSING_FAILURE;
				}
				token_node_t* gain = base->next;
				if (!gain) {
					return PARSING_FAILURE;
				}
				
				buffer_get_float(base->buf, &info.mmTarget->base);
				buffer_get_float(gain->buf, &info.mmTarget->gain);
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// offset
	if (info.flags & MTL_PARSE_TARGET_OFFSET && info.offsetTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-o")) {
				// get the x,y,z values
				token_node_t* x = p->next;
				if (!x) {
					return PARSING_FAILURE;
				}
				token_node_t* y = x->next;
				if (!y) {
					return PARSING_FAILURE;
				}
				token_node_t* z = y->next;
				if (!z) {
					return PARSING_FAILURE;
				}
				
				buffer_get_float(x->buf, *(info.offsetTarget));
				buffer_get_float(y->buf, *(info.offsetTarget + 1));
				buffer_get_float(z->buf, *(info.offsetTarget + 2));
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// scale
	if (info.flags & MTL_PARSE_TARGET_SCALE && info.scaleTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-s")) {
				// get the x,y,z values
				token_node_t* x = p->next;
				if (!x) {
					return PARSING_FAILURE;
				}
				token_node_t* y = x->next;
				if (!y) {
					return PARSING_FAILURE;
				}
				token_node_t* z = y->next;
				if (!z) {
					return PARSING_FAILURE;
				}
				
				buffer_get_float(x->buf, *info.scaleTarget);
				buffer_get_float(y->buf, *(info.scaleTarget + 1));
				buffer_get_float(z->buf, *(info.scaleTarget + 2));
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// turbulence
	if (info.flags & MTL_PARSE_TARGET_TURB && info.turbTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-s")) {
				// get the x,y,z values
				token_node_t* x = p->next;
				if (!x) {
					return PARSING_FAILURE;
				}
				token_node_t* y = x->next;
				if (!y) {
					return PARSING_FAILURE;
				}
				token_node_t* z = y->next;
				if (!z) {
					return PARSING_FAILURE;
				}
				
				buffer_get_float(x->buf, *info.turbTarget);
				buffer_get_float(y->buf, *(info.turbTarget + 1));
				buffer_get_float(z->buf, *(info.turbTarget + 2));
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// texres
	if (info.flags & MTL_PARSE_TARGET_TEXRES && info.texresTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-s")) {
				// get the w,h values
				token_node_t* w = p->next;
				if (!w) {
					return PARSING_FAILURE;
				}
				token_node_t* h = w->next;
				if (!h) {
					return PARSING_FAILURE;
				}
				
				buffer_get_uint(w->buf, &info.texresTarget->w);
				buffer_get_uint(h->buf, &info.texresTarget->h);
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// imfchan
	if (info.flags & MTL_PARSE_TARGET_IMFCHAN && info.imfchanTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-imfchan")) {
				// get the channel (either r,g,b,m,l,z)
				token_node_t* channel = p->next;
				if (!channel) {
					return PARSING_FAILURE;
				}
				
				if (buffer_cmp(channel->buf, "r")) {
					*(info.imfchanTarget) = imfchan_r;
				} else if (buffer_cmp(channel->buf, "g")) {
					*(info.imfchanTarget) = imfchan_g;
				} else if (buffer_cmp(channel->buf, "b")) {
					*(info.imfchanTarget) = imfchan_b;
				} else if (buffer_cmp(channel->buf, "m")) {
					*(info.imfchanTarget) = imfchan_m;
				} else if (buffer_cmp(channel->buf, "l")) {
					*(info.imfchanTarget) = imfchan_l;
				} else if (buffer_cmp(channel->buf, "z")) {
					*(info.imfchanTarget) = imfchan_z;
				}
			}

			p = p->next;
		}
	}

	p = flaglist.head;

	// refl map
	if (info.flags & MTL_PARSE_TARGET_REFLMAP_TYPE 
		&& info.reflmapTypeTarget) {
		while (p) {
			char flag[16] = { 0 };
			buffer_get_str(p->buf, flag);

			if (strequ(flag, "-type")) {
				// get the mapping type
				token_node_t* type = p->next;
				if (!type) {
					return PARSING_FAILURE;
				}
				
				if (buffer_cmp(type->buf, "sphere")) {
					*(info.reflmapTypeTarget) = refl_sphere;
				} else if (buffer_cmp(type->buf, "cube_top")) {
					*(info.imfchanTarget) = refl_cube_top;
				} else if (buffer_cmp(type->buf, "cube_bottom")) {
					*(info.imfchanTarget) = refl_cube_bottom;
				} else if (buffer_cmp(type->buf, "cube_front")) {
					*(info.imfchanTarget) = refl_cube_front;
				} else if (buffer_cmp(type->buf, "cube_back")) {
					*(info.imfchanTarget) = refl_cube_back;
				} else if (buffer_cmp(type->buf, "cube_left")) {
					*(info.imfchanTarget) = refl_cube_left;
				} else if (buffer_cmp(type->buf, "cube_right")) {
					*(info.imfchanTarget) = refl_cube_right;
				} else {
					// unspecified
					return PARSING_FAILURE;
				}
			}

			p = p->next;
		}
	}

	return SUCCESS;
}

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
		printf("\t\"%s\"\n", list.keys[i]);
		mtl_t* at = { 0 };
		map_at(&lib->map, list.keys[i], &at);
		mtl_print(at);
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
	long length = 0;
	if (!file) {
		return INVALID_FILE;
    } else {
		// TODO Potential vulnerability: if the file contains a NUL character 
		// somewhere, this will throw off the 'mtltext' string, and could open 
		// up attacks
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		mtltext = calloc(length + 1, sizeof(char));
		if (mtltext) {
			size_t end = fread(mtltext, 1, length, file);
			mtltext[end] = '\0'; // fread doesn't null terminate
		}
		fclose(file);
	}

	// create the lines ("newmtl Material\n", etc)
	token_list_t lines = (token_list_t) { .head = NULL, .used = 0 };
	if ((tokenlist_create(&lines)) != SUCCESS) {
		return MEMORY_REFUSED;
	}
	if ((ntokenize(&lines, mtltext, length, "\n")) != SUCCESS) {
		return PARSING_FAILURE;
	}

	typedef struct mtl_cmd_t {
		const char command[255];
		token_list_t parameters;
		unsigned int line_number;
	} mtl_cmd_t;

	// create the mtl_cmt_t list, zero-initialized
	mtl_cmd_t cmd_list[1024] = { 0 };
	unsigned int n_commands = 0;

	// create a list of string commands from each line
	// max 1024 commands in material document (arbitrary)
	{
		char strCommands[1024][255] = { 0 };
		token_node_t* line = lines.head;
		unsigned int n_lines = 1;
		while (line) {

			// copy up to the first ' ' to the string to get the command
			unsigned int char_count = 0;
			while (*buffer_at(line->buf, char_count) != ' ') {
				char curr_char = *buffer_at(line->buf, char_count);
				strCommands[n_commands][char_count++] = curr_char;

				// exit
				if (char_count > 255) {
					printf(
						"Error parsing material: command was \
						too long on line %u\n", n_lines);
					return PARSING_FAILURE;
				}
			}

			// get the parameters of this command, as a linked list
			token_list_t parameters = (token_list_t) { 
				.head = NULL, 
				.used = 0 
			};
			if ((tokenlist_create(&parameters)) != SUCCESS) {
				return MEMORY_REFUSED;
			}
			// get list of parameters to this command
			if (ntokenize(
					&parameters, 
					buffer_at(line->buf, char_count + 1), 
					line->buf.length - char_count - 1, 
					" ")
				 != SUCCESS) {

				return PARSING_FAILURE;
			}

			// set the command in the list
			strcpy(
				cmd_list[n_commands].command, 
				strCommands[n_commands]
			);
			cmd_list[n_commands].parameters = parameters;
			cmd_list[n_commands].line_number = n_lines;
			n_commands++;
			n_lines++;

			// get the next line
			line = line->next;
		}

	}

	// the material to build
	mtl_t old_mat; 
	mtl_t curr_mat;
	int first_pass = 1;

	// loop through each command
	for (
		unsigned int cmd_i = 0; 
		cmd_i < n_commands; 
		cmd_i++
	) {

		// get the command data
		const unsigned int line_number = cmd_list[cmd_i].line_number;
		const char* cmd = cmd_list[cmd_i].command;
		const token_list_t params = cmd_list[cmd_i].parameters;

		// copy the name
		if (strequ(cmd, "newmtl")) {
			old_mat = curr_mat;

			// insert the old material
			if (!first_pass) {
				map_insert(&lib->map, old_mat.name, old_mat);
			}

			curr_mat = mtl_create();

			// get the name
			const token_node_t* p = params.head;
			buffer_get_str(p->buf, curr_mat.name);

			first_pass = 0;

		// ambient
		} else if (strequ(cmd, "Ka")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_FLOAT;
			conf.elements = &curr_mat.ambient;
			conf.flags = MTL_PARSE_LIST_ALLOW_ONE_VALUE;
			conf.num_elements = 3;
			conf.line_number = line_number;
			conf.params = params;
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}
		// diffuse
		} else if (strequ(cmd, "Kd")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_FLOAT;
			conf.elements = &curr_mat.diffuse;
			conf.flags = MTL_PARSE_LIST_ALLOW_ONE_VALUE;
			conf.num_elements = 3;
			conf.line_number = line_number;
			conf.params = params;
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}
		// specular
		} else if (strequ(cmd, "Ks")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_FLOAT;
			conf.elements = &curr_mat.specular;
			conf.flags = MTL_PARSE_LIST_ALLOW_ONE_VALUE;
			conf.num_elements = 3;
			conf.line_number = line_number;
			conf.params = params;
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}
		// transmission filter
		} else if (strequ(cmd, "Tf")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_FLOAT;
			conf.elements = &curr_mat.tm_filter;
			conf.flags = MTL_PARSE_LIST_ALLOW_ONE_VALUE;
			conf.num_elements = 3;
			conf.line_number = line_number;
			conf.params = params;
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}

		// illum
		} else if (strequ(cmd, "illum")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_UINT;
			conf.elements = &curr_mat.illum;
			conf.num_elements = 1;
			conf.line_number = line_number;
			conf.params = params;
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}
			
		// opacity
		} else if (strequ(cmd, "d")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_FLOAT;
			conf.elements = &curr_mat.dissolve.value;
			conf.num_elements = 1;
			conf.line_number = line_number;
			conf.params = params;
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}

			MtlParserInfo pinfo = { 0 };
			pinfo.flags = MTL_PARSE_TARGET_HALO;
			pinfo.haloTarget = &curr_mat.dissolve.halo;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}

		// specular exponent
		} else if (strequ(cmd, "Ns")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_UINT;
			conf.elements = &curr_mat.specular_exponent;
			conf.num_elements = 1;
			conf.line_number = line_number;
			conf.params = params;
			// get value
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}
		
		// sharpnress
		} else if (strequ(cmd, "sharpness")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_UINT;
			conf.elements = &curr_mat.sharpness;
			conf.num_elements = 1;
			conf.line_number = line_number;
			conf.params = params;
			// get value
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}
			
		// optical density
		} else if (strequ(cmd, "Ni")) {
			MtlParseListConfig conf = { 0 };
			conf.element_type = TYPE_FLOAT;
			conf.elements = &curr_mat.optical_density;
			conf.num_elements = 1;
			conf.line_number = line_number;
			conf.params = params;
			// get value
			if (mtl_read_from_list(conf) != SUCCESS) {
				return PARSING_FAILURE;
			}
		
		// ambient map
		} else if (strequ(cmd, "map_Ka")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.map_Ka.filename);
			
			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CC
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.map_Ka.filename;
			pinfo.blenduTarget = &curr_mat.map_Ka.blendu;
			pinfo.blendvTarget = &curr_mat.map_Ka.blendv;
			pinfo.ccTarget = &curr_mat.map_Ka.cc;
			pinfo.clampTarget = &curr_mat.map_Ka.clamp;
			pinfo.mmTarget = &curr_mat.map_Ka.mm;
			pinfo.offsetTarget = &curr_mat.map_Ka.offset;
			pinfo.scaleTarget = &curr_mat.map_Ka.scale;
			pinfo.turbTarget = &curr_mat.map_Ka.turbulence;
			pinfo.texresTarget = &curr_mat.map_Ka.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}
		
		// diffuse map
		} else if (strequ(cmd, "map_Kd")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.map_Kd.filename);
			
			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CC
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.map_Kd.filename;
			pinfo.blenduTarget = &curr_mat.map_Kd.blendu;
			pinfo.blendvTarget = &curr_mat.map_Kd.blendv;
			pinfo.ccTarget = &curr_mat.map_Kd.cc;
			pinfo.clampTarget = &curr_mat.map_Kd.clamp;
			pinfo.mmTarget = &curr_mat.map_Kd.mm;
			pinfo.offsetTarget = &curr_mat.map_Kd.offset;
			pinfo.scaleTarget = &curr_mat.map_Kd.scale;
			pinfo.turbTarget = &curr_mat.map_Kd.turbulence;
			pinfo.texresTarget = &curr_mat.map_Kd.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}
		
		// specular map
		} else if (strequ(cmd, "map_Ks")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.map_Ks.filename);
			
			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CC
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.map_Ks.filename;
			pinfo.blenduTarget = &curr_mat.map_Ks.blendu;
			pinfo.blendvTarget = &curr_mat.map_Ks.blendv;
			pinfo.ccTarget = &curr_mat.map_Ks.cc;
			pinfo.clampTarget = &curr_mat.map_Ks.clamp;
			pinfo.mmTarget = &curr_mat.map_Ks.mm;
			pinfo.offsetTarget = &curr_mat.map_Ks.offset;
			pinfo.scaleTarget = &curr_mat.map_Ks.scale;
			pinfo.turbTarget = &curr_mat.map_Ks.turbulence;
			pinfo.texresTarget = &curr_mat.map_Ks.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}
			
		// specular exponent map
		} else if (strequ(cmd, "map_Ns")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.map_Ns.filename);
			
			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_IMFCHAN
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.map_Ns.filename;
			pinfo.blenduTarget = &curr_mat.map_Ns.blendu;
			pinfo.blendvTarget = &curr_mat.map_Ns.blendv;
			pinfo.clampTarget = &curr_mat.map_Ns.clamp;
			pinfo.imfchanTarget = &curr_mat.map_Ns.imfchan;
			pinfo.mmTarget = &curr_mat.map_Ns.mm;
			pinfo.offsetTarget = &curr_mat.map_Ns.offset;
			pinfo.scaleTarget = &curr_mat.map_Ns.scale;
			pinfo.turbTarget = &curr_mat.map_Ns.turbulence;
			pinfo.texresTarget = &curr_mat.map_Ns.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}
			
		// dissolve/opacity map
		} else if (strequ(cmd, "bump")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.bump.filename);
			
			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_IMFCHAN
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.bump.filename;
			pinfo.blenduTarget = &curr_mat.bump.blendu;
			pinfo.blendvTarget = &curr_mat.bump.blendv;
			pinfo.clampTarget = &curr_mat.bump.clamp;
			pinfo.imfchanTarget = &curr_mat.bump.imfchan;
			pinfo.mmTarget = &curr_mat.bump.mm;
			pinfo.offsetTarget = &curr_mat.bump.offset;
			pinfo.scaleTarget = &curr_mat.bump.scale;
			pinfo.turbTarget = &curr_mat.bump.turbulence;
			pinfo.texresTarget = &curr_mat.bump.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}
			
		// anti-aliasing map
		} else if (strequ(cmd, "map_aat")) {
			// get if the next flag is "on" or "off"
			const token_node_t* p = params.head;

			if (!p) {
				return PARSING_FAILURE;
			}

			if (buffer_cmp(p->buf, "on")) {
				curr_mat.map_aat = 1;
			} else if (buffer_cmp(p->buf, "off")) {
				curr_mat.map_aat = 0;
			} else {
				return PARSING_FAILURE;
			}
			
		// decal
		} else if (strequ(cmd, "decal")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.decal.filename);
			
			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_IMFCHAN
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.decal.filename;
			pinfo.blenduTarget = &curr_mat.decal.blendu;
			pinfo.blendvTarget = &curr_mat.decal.blendv;
			pinfo.clampTarget = &curr_mat.decal.clamp;
			pinfo.imfchanTarget = &curr_mat.decal.imfchan;
			pinfo.mmTarget = &curr_mat.decal.mm;
			pinfo.offsetTarget = &curr_mat.decal.offset;
			pinfo.scaleTarget = &curr_mat.decal.scale;
			pinfo.turbTarget = &curr_mat.decal.turbulence;
			pinfo.texresTarget = &curr_mat.decal.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}

		// roughness
		} else if (strequ(cmd, "disp")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.disp.filename);

			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_IMFCHAN
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.disp.filename;
			pinfo.blenduTarget = &curr_mat.disp.blendu;
			pinfo.blendvTarget = &curr_mat.disp.blendv;
			pinfo.clampTarget = &curr_mat.disp.clamp;
			pinfo.imfchanTarget = &curr_mat.disp.imfchan;
			pinfo.mmTarget = &curr_mat.disp.mm;
			pinfo.offsetTarget = &curr_mat.disp.offset;
			pinfo.scaleTarget = &curr_mat.disp.scale;
			pinfo.turbTarget = &curr_mat.disp.turbulence;
			pinfo.texresTarget = &curr_mat.disp.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}

		// bump map
		} else if (strequ(cmd, "map_d")) {
			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, curr_mat.map_d.filename);

			MtlParserInfo pinfo = { 0 };
			pinfo.flags = 
				MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_IMFCHAN
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.filenameTarget = &curr_mat.map_d.filename;
			pinfo.blenduTarget = &curr_mat.map_d.blendu;
			pinfo.blendvTarget = &curr_mat.map_d.blendv;
			pinfo.clampTarget = &curr_mat.map_d.clamp;
			pinfo.imfchanTarget = &curr_mat.map_d.imfchan;
			pinfo.mmTarget = &curr_mat.map_d.mm;
			pinfo.offsetTarget = &curr_mat.map_d.offset;
			pinfo.scaleTarget = &curr_mat.map_d.scale;
			pinfo.turbTarget = &curr_mat.map_d.turbulence;
			pinfo.texresTarget = &curr_mat.map_d.texres;

			// parse flags
			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}

		// reflection map, a little different
		} else if (strequ(cmd, "refl")) {
			refl_opts_t refl_opts = { 0 };

			// get the filename (last parameter)
			const token_node_t* p = params.head;
			if (!p) {
				return PARSING_FAILURE;
			}
			while (p->next) {
				p = p->next;
			}
			buffer_get_str(p->buf, refl_opts.filename);

			MtlParserInfo pinfo = { 0 };
			pinfo.flags = MTL_PARSE_TARGET_REFLMAP_TYPE
				| MTL_PARSE_TARGET_BLENDU
				| MTL_PARSE_TARGET_BLENDV
				| MTL_PARSE_TARGET_CC
				| MTL_PARSE_TARGET_CLAMP
				| MTL_PARSE_TARGET_MM
				| MTL_PARSE_TARGET_OFFSET
				| MTL_PARSE_TARGET_SCALE
				| MTL_PARSE_TARGET_TURB
				| MTL_PARSE_TARGET_TEXRES;
			pinfo.blenduTarget = &refl_opts.blendu;
			pinfo.blendvTarget = &refl_opts.blendv;
			pinfo.ccTarget = &refl_opts.cc;
			pinfo.clampTarget = &refl_opts.clamp;
			pinfo.mmTarget = &refl_opts.mm;
			pinfo.offsetTarget = &refl_opts.offset;
			pinfo.scaleTarget = &refl_opts.scale;
			pinfo.turbTarget = &refl_opts.turbulence;
			pinfo.texresTarget = &refl_opts.texres;

			if (mtl_parse_flags(params, pinfo) != SUCCESS) {
				return PARSING_FAILURE;
			}
			
			// create a new map if not already created
			if (!curr_mat.refl_map.head) {
				refl_create(&curr_mat.refl_map, refl_opts);
			} else {
				// append to the reflection map
				if (refl_append(&curr_mat.refl_map, refl_opts) != SUCCESS) {
					return PARSING_FAILURE;
				}
			}

		} else {
			printf(
				"Warning loading material: Unspecified command argument on \
				line %u\n", line_number);
			// don't throw error - maybe store these in a void* map
			// return PARSING_FAILURE;
		}

	}

	// when we exit, we need to add the curr_mat
	map_insert(&lib->map, curr_mat.name, curr_mat);

	free(mtltext);

    return SUCCESS;
}