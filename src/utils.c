#include <stdlib.h>
#include <string.h>

#include "utils.h"

/** Compares two strings for absolute equality.
 * @param str0 The first string.
 * @param str1 The second string.
 * @returns 0 on absolute equality (determined by strcmp in the standard library), and a non-zero value if nonequal.
*/
inline int strequ(const char* str0, const char* str1) {
    return strcmp(str0, str1) == 0;
}

inline int fstart(FILE* fptr) {
    return fseek(fptr, 0, SEEK_SET);
}

uint32_t buffer_init(char line[MAX_LINE_LEN], void* buffer, const uint32_t dim, const type_t dataformat) {
    char* token = strtok(line, " ");
    for (uint32_t o = 0; o < dim; o++) {
        token = strtok(NULL, " ");
        token[strcspn(token, "\n")] = '\0';
        switch(dataformat) {
            case TYPE_FLOAT:
                ((double*)buffer)[o] = atof(token);
            break;
            case TYPE_UINT:
                ((uint32_t*)buffer)[o] = atoi(token);
            break;
            case TYPE_STR:
                ((char**)buffer)[o] = realloc(((char**)buffer)[o], strlen(token) + 1);
                memset(((char**)buffer)[o], 0, sizeof *((char**)buffer)[o]);
                strcpy(((char**)buffer)[o], token);
            break;
            default: break;
        }
    }
    return 0;
}

void buffer_print(const void* data, const type_t type, const uint32_t dim) {
    printf("[");
    for (uint32_t i = 0; i < dim; i++) {
        switch(type) {
            case TYPE_FLOAT:
                printf("%f", ((double*)data)[i]);
            break;
            case TYPE_UINT:
                printf("%u", ((uint32_t*)data)[i]);
            break;
            case TYPE_STR:
                printf("%s", ((const char**)data)[i]);
            break;
            default: break;
        }
        if (i != dim - 1) printf(", ");
    }
    printf("]\n");
}

void buffer_fwrite(FILE* fptr, const void* data, const uint32_t dim, const type_t type) {
    fprintf(fptr, "[");
    for (uint32_t j = 0; j < dim; j++) {
        switch (type) {
            case TYPE_FLOAT:
                fprintf(fptr, "%f", ((double*)data)[j]);
            break;
            case TYPE_UINT:
                fprintf(fptr, "%u", ((uint32_t*)data)[j]);
            break;
            case TYPE_STR:
                fprintf(fptr, "%s", ((const char**)data)[j]);
            break;
        }
        if (j != dim - 1)
            fprintf(fptr, ", ");
    }
    fprintf(fptr, "]\n");
}