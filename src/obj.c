#include "obj.h"

// --------------------------------------------------------------
// Utility static functions
// --------------------------------------------------------------
/** Gets the dimension of the mesh component based on the ASCII text. Each value in the component must be separated by a single space character " ".
 * @param buffer The text line fetched by the .obj file.
 * @returns The number of dimensions in this line.
*/
static uint32_t get_dim(char buffer[MAX_LINE_LEN]) {
    uint32_t dim = 0;
    char* token = strtok(buffer, " ");
    do {
        token = strtok(NULL, " ");
        dim++;
    }  while (token);
    dim--;
    return dim;
}

/** Checks if the retrieved dimension from the raw text matches the expected dimension.
 * @param expected The expected dimension.
 * @param retrieved The retrieved dimension number.
 * @param line The line fetched from the .obj file.
 * @returns SUCCESS, INVALID_DIMS.
*/
static int check_dim(uint32_t* expected, uint32_t* retrieved, char* line) {
    *retrieved = get_dim(line);
    if (*expected != 0 && *retrieved != *expected) {
        return INVALID_DIMS;
    }
    *expected = *retrieved;
    return SUCCESS;
}

/** Determines the byte size of what the type represents. For example, a type_t of TYPE_UINT will return sizeof(uint32_t).
 * @param t The type to find the size of.
 * @returns The byte size of the represented primitive, or 0 if the type could not be determined.
*/
static uint32_t sizeof_typet(type_t t) {
    switch(t) {
        case TYPE_UINT:
            return sizeof(uint32_t);
        break;
        case TYPE_FLOAT:
            return sizeof(double);
        break;
        case TYPE_STR:
            return sizeof(const char*);
        break;
        default: break;
    }
    return 0;
}

/**
 * @brief Get the face flag.
 * Ensures face definitions across the file are consistent.
 *
 * @param face_str_element The face element. Usually "1/2/3".
 * @param line The line number.
 * @returns The flag. The face flag is an integer.
 */
static uint32_t get_face_flag(const char* face_str_element, uint32_t line) {
    const char* ptr = face_str_element;
    uint32_t first_pass = 1;
    uint32_t size = 0;
    uint32_t flag = 0;
    uint32_t shift = 0;
    uint32_t prev_flag = 0;
    uint32_t curr_flag = 0;
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

/**
 * @brief Gets the objects info (number of components and their dimensions).
 *
 * @param mesh The mesh object.
 * @param file The file object.
 * @param err_msg Output error message, if one is encountered.
 * @return A return code that can either be SUCCESS or INVALID_DIMS.
 */
static int obj_setinfo(mesh_t* mesh, FILE* file, char* err_msg) {
    int RETURN_CODE = SUCCESS;
    uint32_t tmp_num_verts = 0;
    uint32_t tmp_num_faces = 0;
    uint32_t tmp_num_norms = 0;
    uint32_t tmp_num_texs = 0;
    char buffer[MAX_LINE_LEN] = {0};
    uint32_t line_number = 0;

    int name_defined = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        line_number++;
        const char* type;

        char original_string_for_face[sizeof(buffer)];
        char original_string_for_dim[sizeof(buffer)];
        strcpy(original_string_for_face, buffer);
        strcpy(original_string_for_dim, buffer);

        type = strtok(buffer, " ");
        uint32_t tmp = 0;

        if (strequ(type, "v")) {
            tmp_num_verts++;
            if ((RETURN_CODE = check_dim(&mesh->vertex_dim, &tmp, original_string_for_dim)) != SUCCESS) {
                sprintf(err_msg, "Error: mismatch of vertex dimension at line %d: expected %d, got %d vertices\n",
                    line_number, mesh->vertex_dim, tmp);
                return RETURN_CODE;
            }
        } else if (strequ(type, "vn")) {
            tmp_num_norms++;
            if ((RETURN_CODE = check_dim(&mesh->vertex_dim, &tmp, original_string_for_dim)) != SUCCESS) {
                sprintf(err_msg, "Error: mismatch of vertex normal dimension at line %d: expected %d, got %d vertex normals (must be same dimensions as vertex dimension)\n",
                        line_number, mesh->vertex_dim, tmp);
                return RETURN_CODE;
            }
        } else if (strequ(type, "vt")){
            tmp_num_texs++;
            if ((RETURN_CODE = check_dim(&mesh->tex_dim, &tmp, original_string_for_dim)) != SUCCESS) {
                sprintf(err_msg, "Error: mismatch of vertex texture dimension at line %d: expected %d, got %d vertex texture components\n",
                        line_number, mesh->tex_dim, tmp);
                return RETURN_CODE;
            }
        } else if (strequ(type, "f")) {
            tmp_num_faces++;
            if ((RETURN_CODE = check_dim(&mesh->face_dim, &tmp, original_string_for_dim)) != SUCCESS) {
                sprintf(err_msg, "Error: mismatch of face dimension at line %d: expected %d, got %d face components\n",
                        line_number, mesh->tex_dim, tmp);
                return RETURN_CODE;
            }
            mesh->face_flag.flag = get_face_flag(original_string_for_face, line_number);
        } else if (strequ(type, "o") && !name_defined) {
            char* tmp_name = strtok(NULL, "\n");
            mesh->name = calloc(1, strlen(tmp_name));
            strcpy(mesh->name, tmp_name);
            name_defined = !name_defined;
        } else if (strequ(type, "mtllib")) {
            // char* tmp_name = strtok(NULL, "\n");
            // TODO: create a new entry in material library.
        }
    }
    mesh->num_vertices = tmp_num_verts;
    mesh->num_faces = tmp_num_faces;
    mesh->num_normals = tmp_num_norms;
    mesh->num_textures = tmp_num_texs;

    return RETURN_CODE;
}

// --------------------------------------------------------------
// Implementation
// --------------------------------------------------------------
void obj_print(const mesh_t* mesh) {
    for (uint32_t i = 0; i < mesh->num_vertices; i++) {
        buffer_print(mesh->vertex_data[i].pos, TYPE_FLOAT, mesh->vertex_dim);
    }
    for (uint32_t i = 0; i < mesh->num_normals; i++) {
        buffer_print(mesh->normal_data[i].norm, TYPE_FLOAT, mesh->vertex_dim);
    }
    for (uint32_t i = 0; i < mesh->num_textures; i++) {
        buffer_print(mesh->texture_data[i].tex, TYPE_FLOAT, mesh->tex_dim);
    }
    for (uint32_t i = 0; i < mesh->num_faces; i++) {
        if (mesh->face_flag.flag & pos_flag) {
            printf("Position Indices: \n");
            buffer_print(mesh->face_data[i].indices, TYPE_UINT, mesh->face_dim);
        }
        if (mesh->face_flag.flag & tex_flag) {
            printf("Texture Indices: \n");
            buffer_print(mesh->face_data[i].texs, TYPE_UINT, mesh->face_dim);
        }
        if (mesh->face_flag.flag & norm_flag) {
            printf("Normal Indices: \n");
            buffer_print(mesh->face_data[i].norms, TYPE_UINT, mesh->face_dim);
        }
    }
}

void obj_fwrite(const mesh_t* mesh, const char* fn) {

    FILE* file = fopen(fn, "w");
    if (!file) {
        return;
	}

    fprintf(file, "*** Object Name: %s ***\n", mesh->name);
    fprintf(file, "*** Vertex Dimension: %d ***\n", mesh->vertex_dim);
    fprintf(file, "*** Texture Dimension: %d ***\n", mesh->tex_dim);
    fprintf(file, "*** Face Dimension: %d ***\n", mesh->face_dim);

    fprintf(file, "*** Vertex Positions ***\n");
    if (mesh->num_vertices <= 0) {
        fprintf(file, "none\n");
    } else {
        for (uint32_t i = 0; i < mesh->num_vertices; i++) {
            buffer_fwrite(file, mesh->vertex_data[i].pos, TYPE_FLOAT, mesh->vertex_dim);
        }
    }

    fprintf(file, "*** Vertex Normals ***\n");
    if (mesh->num_normals <= 0) {
        fprintf(file, "none\n");
    } else {
        for (uint32_t i = 0; i < mesh->num_normals; i++) {
            buffer_fwrite(file, mesh->normal_data[i].norm, TYPE_FLOAT, mesh->vertex_dim);
        }
    }

    fprintf(file, "*** Texture Coordinates ***\n");
    if (mesh->num_textures <= 0) {
        fprintf(file, "none\n");
    } else {
        for (uint32_t i = 0; i < mesh->num_textures; i++) {
            buffer_fwrite(file, mesh->texture_data[i].tex, TYPE_FLOAT, mesh->tex_dim);
        }
    }

    fprintf(file, "*** Face mesh ***\n");
    for (uint32_t i = 0; i < mesh->num_faces; i++) {
        fprintf(file, "*** Face %d ***\n", i+1);
        if (mesh->face_flag.flag & pos_flag) {
            fprintf(file, "Position Indices ->");
            buffer_fwrite(file, mesh->face_data[i].indices, TYPE_UINT, mesh->face_dim);
        }

        if (mesh->face_flag.flag & tex_flag) {
            fprintf(file, "Texture Indices ->");
            buffer_fwrite(file, mesh->face_data[i].texs, TYPE_UINT, mesh->face_dim);
        }

        if (mesh->face_flag.flag & norm_flag) {
            fprintf(file, "Normal Indices ->");
            buffer_fwrite(file, mesh->face_data[i].norms, TYPE_UINT, mesh->face_dim);
        }
    }

    fclose(file);
}

void obj_destroy(mesh_t* mesh) {
    for (uint32_t i = 0; i < mesh->num_vertices; i++) {
        free(mesh->vertex_data[i].pos);
    }
    free(mesh->vertex_data);
    for (uint32_t i = 0; i < mesh->num_normals; i++) {
        free(mesh->normal_data[i].norm);
    }
    free(mesh->normal_data);
    for (uint32_t i = 0; i < mesh->num_textures; i++) {
        free(mesh->texture_data[i].tex);
    }
    free(mesh->texture_data);
    for (uint32_t i = 0; i < mesh->num_faces; i++) {
        if (mesh->face_flag.flag & pos_flag) {
            free(mesh->face_data[i].indices);
		}
        if (mesh->face_flag.flag & tex_flag) {
            free(mesh->face_data[i].texs);
		}
        if (mesh->face_flag.flag & norm_flag) {
            free(mesh->face_data[i].norms);
		}
    }

    free(mesh->face_data);
    if (mesh->name && !mesh->name) {
        free(mesh->name);
	}
}

void obj_init(mesh_t* mesh) {
    mesh->face_dim = 0;
    mesh->vertex_dim = 0;
    mesh->tex_dim = 0;

    mesh->num_faces = 0;
    mesh->num_normals = 0;
    mesh->num_textures = 0;
    mesh->num_vertices = 0;

    mesh->vertex_data = 0;
    mesh->face_data = 0;
    mesh->normal_data = 0;
    mesh->texture_data = 0;

    mesh->face_flag.flag = 0;

    mesh->name = "";
}

int obj_read(const char* fn, mesh_t* mesh) {
    obj_init(mesh);
    int RETURN_CODE = SUCCESS;
    char err_msg[256];

	// TODO: Error callbacks
    FILE* file = fopen(fn, "r");
    if (!file) {
        printf("Error: invalid, inaccessible, unavailable, or nonexistent file \"%s\"\n", fn);
        RETURN_CODE = INVALID_FILE;
        return RETURN_CODE;
    }

    if ((RETURN_CODE = obj_setinfo(mesh, file, err_msg)) != SUCCESS) {
        printf("%s", err_msg);
        return RETURN_CODE;
    }

    struct face_buffers {
        uint32_t* pos_idx_buffer;
        uint32_t* tex_idx_buffer;
        uint32_t* norm_idx_buffer;
        char** face_str_buffer;
    } face_buffers;
    void* generic_buffer;

	// Memory allocations
    // TODO: we exit without cleaning up. How can we do this properly?
    if (!(face_buffers.pos_idx_buffer = calloc(mesh->face_dim, sizeof *face_buffers.pos_idx_buffer)) ||
    !(face_buffers.tex_idx_buffer = calloc(mesh->face_dim, sizeof *face_buffers.tex_idx_buffer)) ||
    !(face_buffers.norm_idx_buffer = calloc(mesh->face_dim, sizeof *face_buffers.norm_idx_buffer)) ||
    !(face_buffers.face_str_buffer = calloc(mesh->face_dim, sizeof *face_buffers.face_str_buffer)) ||
    !(generic_buffer = calloc(1, MAX_LINE_LEN)) ||
    !(mesh->vertex_data = calloc(mesh->num_vertices, sizeof(vertex_t))) ||
    !(mesh->normal_data = calloc(mesh->num_normals, sizeof(normal_t))) ||
    !(mesh->texture_data = calloc(mesh->num_textures, sizeof(texture_t))) ||
    !(mesh->face_data = calloc(mesh->num_faces, sizeof(face_t)))) {
        return OUT_OF_MEMORY;
    }
    for (uint32_t i = 0; i < mesh->face_dim; i++) {
        if (!(face_buffers.face_str_buffer[i] = calloc(mesh->face_dim, sizeof *face_buffers.face_str_buffer[i]))) {
            return OUT_OF_MEMORY;
        }
    }
    for (uint32_t i = 0; i < mesh->num_vertices; i++) {
        if (!(mesh->vertex_data[i].pos = calloc(mesh->vertex_dim, sizeof *mesh->vertex_data[i].pos))) {
            return OUT_OF_MEMORY;
        }
    }
    for (uint32_t i = 0; i < mesh->num_normals; i++) {
        if (!(mesh->normal_data[i].norm = calloc(mesh->vertex_dim, sizeof *mesh->normal_data[i].norm))) {
            return OUT_OF_MEMORY;
        }
    }
    for (uint32_t i = 0; i < mesh->num_textures; i++) {
        if (!(mesh->texture_data[i].tex = calloc(mesh->tex_dim, sizeof *mesh->texture_data[i].tex))) {
            return OUT_OF_MEMORY;
        }
    }
    for (uint32_t i = 0; i < mesh->num_faces; i++) {
        if (mesh->face_flag.flag & pos_flag) {
            if (!(mesh->face_data[i].indices = calloc(mesh->face_dim, sizeof *mesh->face_data[i].indices))) {
                return OUT_OF_MEMORY;
            }
        }
        if (mesh->face_flag.flag & norm_flag) {
            if (!(mesh->face_data[i].norms = calloc(mesh->face_dim, sizeof *mesh->face_data[i].norms))) {
                return OUT_OF_MEMORY;
            }
        }
        if (mesh->face_flag.flag & tex_flag) {
            if (!(mesh->face_data[i].texs = calloc(mesh->face_dim, sizeof *mesh->face_data[i].texs))) {
                return OUT_OF_MEMORY;
            }
        }
    }

    // Read the file contents
    char line_buffer[MAX_LINE_LEN];
    char line_buffer_cpy[sizeof line_buffer];
    uint32_t vi = 0, ti = 0, ni = 0, fi = 0;
    fstart(file);
    while (fgets(line_buffer, sizeof line_buffer, file)) {
        void** generic_member;
        uint32_t dim;
        type_t dataformat;

        strcpy(line_buffer_cpy, line_buffer);

        const char* type = strtok(line_buffer, " ");

        if (strequ(type, "v")) {
            generic_member = (void**)(&mesh->vertex_data[vi].pos);
            dim = mesh->vertex_dim;
            dataformat = TYPE_FLOAT;
            vi++;
        } else if (strequ(type, "vt")) {
            generic_member = (void**)(&mesh->texture_data[ti].tex);
            dim = mesh->tex_dim;
            dataformat = TYPE_FLOAT;
            ti++;
        } else if (strequ(type, "vn")) {
            generic_member = (void**)(&mesh->normal_data[ni].norm);
            dim = mesh->vertex_dim;
            dataformat = TYPE_FLOAT;
            ni++;
        } else if (strequ(type, "f")) {
            if (buffer_init(line_buffer_cpy, face_buffers.face_str_buffer, mesh->face_dim, TYPE_STR)) {
                continue;
			}
            for (uint32_t j = 0; j < mesh->face_dim; j++) {
                char* buffer = strtok(face_buffers.face_str_buffer[j], "/");
                if (mesh->face_flag.flag & pos_flag) {
                    int pos_index = atoi(buffer);
                    face_buffers.pos_idx_buffer[j] = pos_index;
                    buffer = strtok(NULL, "/");
                }
                if (mesh->face_flag.flag & tex_flag) {
                    int tex_index = atoi(buffer);
                    face_buffers.tex_idx_buffer[j] = tex_index;
                    buffer = strtok(NULL, "/");
                }
                if(mesh->face_flag.flag & norm_flag) {
                    int norm_index = atoi(buffer);
                    face_buffers.norm_idx_buffer[j] = norm_index;
                    buffer = strtok(NULL, "/");
                }
            }
            if (mesh->face_flag.flag & pos_flag) {
                memcpy(mesh->face_data[fi].indices, face_buffers.pos_idx_buffer, sizeof *face_buffers.pos_idx_buffer * mesh->face_dim);
			}
            if (mesh->face_flag.flag & tex_flag) {
                memcpy(mesh->face_data[fi].texs, face_buffers.tex_idx_buffer, sizeof *face_buffers.tex_idx_buffer * mesh->face_dim);
			}
            if (mesh->face_flag.flag & norm_flag) {
                memcpy(mesh->face_data[fi].norms, face_buffers.norm_idx_buffer, sizeof *face_buffers.norm_idx_buffer * mesh->face_dim);
			}
			
            memset(face_buffers.face_str_buffer, 0, sizeof *face_buffers.face_str_buffer * mesh->face_dim);
            memset(face_buffers.pos_idx_buffer, 0, sizeof *face_buffers.pos_idx_buffer * mesh->face_dim);
            memset(face_buffers.tex_idx_buffer, 0, sizeof *face_buffers.tex_idx_buffer * mesh->face_dim);
            memset(face_buffers.norm_idx_buffer, 0, sizeof *face_buffers.norm_idx_buffer * mesh->face_dim);

            fi++;
            continue;
        } else {
            continue;
        }

        if (!(generic_buffer = realloc(generic_buffer, sizeof_typet(dataformat) * dim))) {
            RETURN_CODE = OUT_OF_MEMORY;
            return RETURN_CODE;
        }

        if (buffer_init(line_buffer_cpy, generic_buffer, dim, dataformat)) {
            continue;
		}

        memcpy(*generic_member, generic_buffer, sizeof_typet(dataformat) * dim);
        memset(generic_buffer, 0, sizeof_typet(dataformat) * dim);
    }

    free(generic_buffer);
    for (uint32_t i = 0; i < mesh->face_dim; i++) {
        free(face_buffers.face_str_buffer[i]);
	}

    free(face_buffers.face_str_buffer);
    free(face_buffers.pos_idx_buffer);
    free(face_buffers.tex_idx_buffer);
    free(face_buffers.norm_idx_buffer);

    fclose(file);

    // Completed.
    return RETURN_CODE;
}
