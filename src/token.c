#include "defs.h"
#include "token.h"
#include <string.h>

int 
tokennode_create(token_node_t* node) {
	node = malloc(sizeof(token_node_t));
	if (!node) {
		return MEMORY_REFUSED;
	}
	node->data = (buffer_t) {0};
	node->next = NULL;
	return SUCCESS;
}

int 
tokenlist_create(token_list_t* out) {
	if (!tokennode_create(out->head)) {
		return MEMORY_REFUSED;
	}
	out->used = 0;
	return SUCCESS;
}

int 
tokenize(token_list_t* out, const char* const str, const char* const delim) {
	token_node_t* p = out->head;
	const char* token;
	unsigned int begin = 0;
	unsigned int end = 0;
	unsigned int delim_len = strlen(delim);
	while (str[begin] != '\0' && token = strstr(str + begin, delim)) {
		end = (unsigned int) (token - str);
		if (end == begin) {
			begin += delim_len;
			continue;
		}
		buffer_t buf = (buffer_t) {0};
		buf.data = str;
		buf.offset = begin;
		buf.length = end - begin;
		p->data = buf;
		if (!p->next) {
			if (!tokennode_create(p->next)) {
				return MEMORY_REFUSED;
			}
		}
		p = p->next;
		out->used++;
		begin = (unsigned int) (token + delim_len);
	}
	return SUCCESS;
}


void 
tokennode_destroy(token_node_t* node) {
	node->data = (buffer_t) {0};
	node->next = NULL;
	free(node);
}

void 
tokenlist_destroy(token_list_t* list) {
	token_node_t* p = list->head;
	while (p) {
		token_node_t* temp = p;
		p = p->next;
		temp->data = (buffer_t) {0};
		temp->next = NULL;
		free(temp);
	}
}