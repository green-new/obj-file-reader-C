/**
 * @file buffer.h
 * @author green
 * @date 8/29/2024
 * @brief Definition of parsing buffers.
 */
#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

typedef struct {
	const char* data;
	uint32_t offset;
	uint32_t length;
} buffer_t;

int
buffer_get_float(buffer_t* src, float* dest);
int 
buffer_get_int(buffer_t* src, int* dest);
int 
buffer_get_str(buffer_t* src, char* dest, unsigned int n);

#endif