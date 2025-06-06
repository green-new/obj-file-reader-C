/**
 * @file token.h
 * @author green
 * @date 8/29/2024
 * @brief Definition of tokens and tokenizer functions.
 */
#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include "buffer.h"
#include "utils.h"

typedef struct token_node_t {
	buffer_t buf;
	struct token_node_t* next;
} token_node_t;

typedef struct {
	token_node_t* head;
	unsigned int used;
} token_list_t;

int 
tokenlist_create(token_list_t* out);

int  
tokenize(token_list_t* const out, 
	const char* str, 
	const char* delim);
	
int 
ntokenize(token_list_t* const out,
	const char* str,
	unsigned int n,
	const char* delim);

/**
 * Reads a from a token list into a pre allocated destination a number of 
 * elements of specified type.
 */
int
tokenlist_read_void(
	unsigned int n,
	token_list_t list,
	type_t type,
	void* dest,
	int copy_first_value);

void 
tokenlist_destroy(token_list_t* list);

#endif