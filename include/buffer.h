/**
 * @file buffer.h
 * @author green
 * @date 8/29/2024
 * @brief Definition of parsing buffers.
 */
#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#define buffer_start(buffer) ((buffer).data + (buffer).offset)
#define buffer_end(buffer) ((buffer).data + (buffer).offset + (buffer).length)
#define buffer_at(buffer, index) *((buffer).data + (buffer).offset + index)
#define buffer_within(buffer, index) \
	(()((buffer).offset + index < (buffer).offset + (buffer).length))

typedef struct {
	const char* data;
	unsigned int offset;
	unsigned int length;
} buffer_t;

int
buffer_get_float(buffer_t* src, float* dest);
int 
buffer_get_int(buffer_t* src, int* dest);
int 
buffer_get_uint(buffer_t* src, unsigned int* dest);
int 
buffer_get_strn(buffer_t* src, char* dest, unsigned int n);
int 
buffer_get_str(buffer_t* src, char* dest);
int 
buffer_cmp(buffer_t* src, char* str);

#endif