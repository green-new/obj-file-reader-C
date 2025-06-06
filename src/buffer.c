#include "buffer.h"
#include "defs.h"
#include <string.h>
#include <stdlib.h>

#define MAX_TEMP_SIZE 64

int
buffer_get_float(buffer_t src, float* dest) {
	if (src.length > MAX_TEMP_SIZE) {
		return PARSING_FAILURE; // Not enough characters
	}
	char numstr[MAX_TEMP_SIZE] = {0};
	unsigned int i = 0;
	for (; i < MAX_TEMP_SIZE; i++) {
		switch (buffer_start(src)[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '+':
			case '-':
			case 'e':
			case 'E':
				numstr[i] = buffer_start(src)[i];
				break;
			case '.':
			case ',':
				numstr[i] = '.';
				break;
            default:
				goto loop_exit;
				break;
		}
	}
loop_exit: ;
	char* errptr;
	numstr[i] = '\0';
	*dest = (float) strtod((const char*) numstr, (char**) &errptr);
	if (*errptr != '\0') {
		return PARSING_FAILURE;
	}
	return SUCCESS;
}
int 
buffer_get_int(buffer_t src, int* dest) {
	if (src.length > MAX_TEMP_SIZE) {
		return 1; // Not enough characters
	}
	char numstr[MAX_TEMP_SIZE] = {0};
	unsigned int i = 0;
	for (; i < MAX_TEMP_SIZE; i++) {
		switch (buffer_start(src)[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '+':
			case '-':
			case 'e':
			case 'E':
				numstr[i] = buffer_start(src)[i];
				break;
			case '.':
			case ',':
				numstr[i] = '.';
				break;
            default:
				goto loop_exit;
				break;
		}
	}
loop_exit: ;
	char* errptr;
	numstr[i] = '\0';
	*dest = (int) strtod((const char*)numstr, (char**)&errptr);
	if (*errptr != '\0') {
		return PARSING_FAILURE;
	}
	return SUCCESS;
}
int 
buffer_get_uint(buffer_t src, unsigned int* dest) {
	if (src.length > MAX_TEMP_SIZE) {
		return 1; // Not enough characters
	}
	char numstr[MAX_TEMP_SIZE] = {0};
	unsigned int i = 0;
	for (; i < MAX_TEMP_SIZE; i++) {
		switch (buffer_start(src)[i]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '+':
			case '-':
			case 'e':
			case 'E':
				numstr[i] = buffer_start(src)[i];
				break;
			case '.':
			case ',':
				numstr[i] = '.';
				break;
            default:
				goto loop_exit;
				break;
		}
	}
loop_exit: ;
	char* errptr;
	numstr[i] = '\0';
	*dest = (unsigned int) strtod((const char*)numstr, (char**)&errptr);
	if (*errptr != '\0') {
		return PARSING_FAILURE;
	}
	return SUCCESS;
}
int buffer_get_str(buffer_t src, char* restrict dest) {
	for (unsigned int i = src.offset; i < src.length; i++) {
		dest[i] = *buffer_at(src, i);
	}
	return SUCCESS;
}
int buffer_cmp(buffer_t src, char* restrict str) {
	return strncmp(buffer_start(src), str, src.length) == 0;
}