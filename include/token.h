/**
 * @file token.h
 * @author green
 * @date 8/29/2024
 * @brief Definition of tokens and tokenizer functions.
 */
#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

typedef struct {
	buffer_t data;
	token_node_t* next;
} token_node_t;

typedef struct {
	const char* origstr;
	token_node_t* head;
	unsigned int used;
} token_list_t;

token_list_t tokenize(const char* str, const char* delim);

#endif