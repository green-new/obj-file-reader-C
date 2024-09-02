/**
 * @file token.h
 * @author green
 * @date 8/29/2024
 * @brief Definition of tokens and tokenizer functions.
 */
#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include "buffer.h"

typedef struct token_node_t {
	buffer_t data;
	struct token_node_t* next;
} token_node_t;

typedef struct {
	token_node_t* head;
	unsigned int used;
} token_list_t;

int 
tokennode_create(token_node_t* out);

int 
tokenlist_create(token_list_t* out);

int  
tokenize(token_list_t* const out, 
	const char* str, 
	const char* const delim);

void 
tokennode_destroy(token_node_t* node);

void 
tokenlist_destroy(token_list_t* list);

#endif