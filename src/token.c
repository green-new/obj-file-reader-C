#include "defs.h"
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "utils.h"

// -----------------------------------------------------------------------------
// Static utility
// -----------------------------------------------------------------------------
int 
tokennode_create(token_node_t** node) {
	(*node) = malloc(sizeof(token_node_t));
	if ((*node) == NULL) {
		return MEMORY_REFUSED;
	}
	(*node)->buf = (buffer_t) { .data = NULL, .length = 0, .offset = 0 };
	(*node)->next = NULL;
	return SUCCESS;
}

void 
tokennode_destroy(token_node_t** node) {
	(*node)->buf = (buffer_t) { .data = NULL, .length = 0, .offset = 0 };
	(*node)->next = NULL;
	free((*node));
}

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------
int 
tokenlist_create(token_list_t* out) {
	int code = SUCCESS;
	if ((code = tokennode_create(&out->head)) != SUCCESS) {
		return code;
	}
	out->used = 0;
	return SUCCESS;
}

int 
tokenize(token_list_t* const out, const char* str, const char* delim) {
	int code = SUCCESS;
	token_node_t* p = out->head;
	token_node_t* q = NULL;
	const char* token;
	unsigned int begin = 0;
	unsigned int end = 0;
	unsigned int delim_len = strlen(delim);
	unsigned int str_len = strlen(str);
	// Right-leaning delimiation e.g. "asdf;asdf;asdf;", delim = ";"
	// results in [asdf, asdf, asdf]
	while (begin < str_len) {
		token = strstr(str + begin, delim);
		if (token == NULL) {
			return SUCCESS;
		}
		end = (unsigned int) (token - str);
		buffer_t buf = (buffer_t) { 
			.data = str, .offset = begin, .length = end - begin 
			};
		if (!p) {
			if ((code = tokennode_create(&p)) != SUCCESS) {
				return code;
			}
			if (q) {
				q->next = p;
			}
		}
		p->buf = buf;
		q = p;
		p = p->next;
		out->used++;
		begin = (unsigned int) (token - str + delim_len);
	}
	return SUCCESS;
}

int 
ntokenize(
	token_list_t* const out, 
	const char* str, 
	unsigned int n, 
	const char* delim
) {
	int code = SUCCESS;
	token_node_t* p = out->head;
	token_node_t* q = NULL;
	const char* token;
	unsigned int begin = 0;
	unsigned int end = 0;
	unsigned int delim_len = strlen(delim);
	// Right-leaning delimiation e.g. "asdf;asdf;asdf;", delim = ";"
	// results in [asdf, asdf, asdf]
	while (begin < n) {
		token = strstr(str + begin, delim);
		if (token == NULL) {
			end = n;
		} else {
			end = (unsigned int) (token - str) > n ? n : (token - str);
		}
		buffer_t buf = (buffer_t) { 
			.data = str, .offset = begin, .length = end - begin 
		};
		if (!p) {
			if ((code = tokennode_create(&p)) != SUCCESS) {
				return code;
			}
			if (q) {
				q->next = p;
			}
		}
		p->buf = buf;
		q = p;
		p = p->next;
		out->used++;
		begin = end + delim_len;
	}
	return SUCCESS;
}

int
tokenlist_read_void(
	unsigned int n,
	token_list_t list,
	type_t type,
	void* dest,
	int copy_first_value
) {
	if (list.used != n) {
		return PARSING_FAILURE;
	}
	const token_node_t* p = list.head;

	switch (type) {
		case TYPE_FLOAT:
			for (unsigned int i = 0; i < n; i++) {
				if (!p) {
					return PARSING_FAILURE;
				}
				buffer_get_float(p->buf, dest + (i * sizeof(float)));

				if (!copy_first_value) {
					p = p->next;
				}
			}
		return SUCCESS;
		case TYPE_STR:
			for (unsigned int i = 0; i < n; i++) {
				if (!p) {
					return PARSING_FAILURE;
				}
				buffer_get_str(p->buf, dest + (i * sizeof(char)));

				if (!copy_first_value) {
					p = p->next;
				}
			}
		return SUCCESS;
		case TYPE_UINT:
			for (unsigned int i = 0; i < n; i++) {
				if (!p) {
					return PARSING_FAILURE;
				}
				buffer_get_uint(p->buf, dest + (i * sizeof(unsigned int)));

				if (!copy_first_value) {
					p = p->next;
				}
			}
		return SUCCESS;
		default: return PARSING_FAILURE;
	}
}

void 
tokenlist_destroy(token_list_t* list) {
	token_node_t* p = list->head;
	while (p) {
		token_node_t* temp = p;
		p = p->next;
		tokennode_destroy(&temp);
	}
	(*list) = (token_list_t) { .head = NULL, .used = 0 };
}