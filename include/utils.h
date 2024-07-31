#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "defs.h"

typedef enum {
    TYPE_UINT,
    TYPE_STR,
    TYPE_FLOAT
} type_t;

/** Compares two strings for absolute equality.
 * @param str0 The first string.
 * @param str1 The second string.
 * @returns 0 on absolute equality (determined by strcmp in the standard 
 * library), and a non-zero value if nonequal.
*/
int 
strequ(const char* str0, const char* str1);

/** Points the file back to the start of the file.
 * @param file Pointer to the file object.
 * @returns The result from fseek() from the standard library.
*/
int 
fstart(FILE* file);

/** Initalizes a provided buffer with data from the line based on the expected 
 * dimensions and data format.
 * @param line The string line from the file.
 * @param buffer The heap-allocated buffer to initialize.
 * @param dim The expected dimension of values to read in.
 * @param dataforamt The dataformat to use for the retrieved values.
 * @returns 0 if the function completed successfuly, or a non-zero value if the 
 * function failed during conversion.
*/
uint32_t 
buffer_init(char line[MAX_LINE_LEN], 
void* buffer, 
const uint32_t dim, 
const type_t dataformat);

/** Prints a dimension of numbers to standard output.
 * 
 * @param data The data to print.
 * @param type The format of the data.
 * @param dim The dimension of the data.
 */
void 
buffer_print(const void* data, const type_t type, const uint32_t dim);

/** Writes the buffer to the provided file. File must be at least on write mode.
 * 
 * @param file The file pointer.
 * @param data The data to write to file.
 * @param dim The dimension of the data.
 * @param type The type of the data.
 */
void 
buffer_fwrite(FILE* file, 
const void* data,  
const type_t type, 
const uint32_t dim);

#endif