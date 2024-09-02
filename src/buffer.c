#include "buffer.h"
#include "defs.h"
#include <stdlib.h>

#define buffer_start(buffer) ((buffer)->data + (buffer)->offset)
#define buffer_end(buffer) ((buffer)->data + (buffer)->offset + (buffer)->length)
#define buffer_at(buffer, index) *((buffer)->data + (buffer)->offset + index)
#define buffer_within(buffer, index) \
	(()((buffer)->offset + index < (buffer)->offset + (buffer)->length))

int
buffer_get_float(buffer_t* src, float* dest) {
	#define N 64
	if (src->length > N) {
		return PARSING_FAILURE; // Not enough characters
	}
	char numstr[N] = {0};
	unsigned int i = 0;
	for (; i < N; i++) {
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
	#undef N
	return SUCCESS;
}
int 
buffer_get_int(buffer_t* src, int* dest) {
	#define N 64
	if (src->length > N) {
		return 1; // Not enough characters
	}
	char numstr[N] = {0};
	unsigned int i = 0;
	for (; i < N; i++) {
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
int buffer_get_str(buffer_t* src, char* dest, unsigned int n) {
	for (unsigned int i = 0; i < n && i < src->length; i++) {
		dest[i] = buffer_at(src, i);
	}
	return SUCCESS;
}