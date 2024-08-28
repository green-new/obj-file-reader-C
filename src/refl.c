#include "refl.h"

// -----------------------------------------------------------------------------
// Static utitliy functions
// -----------------------------------------------------------------------------
refl_node_t* refl_node_create(refl_opts_t options) {
    refl_node_t* node = calloc(1, sizeof(refl_node_t));
    node->options = options;
    node->next = NULL;
    return node;
}

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

int
refl_create(refl_t* refl, refl_opts_t options) {
    refl->used = 0;
    refl->head = refl_node_create(options);
    if (!refl->head) {
        return MEMORY_REFUSED;
    }
    return SUCCESS;
}

void
refl_destroy(refl_t* refl) {
    refl->used = 0;
    refl_node_t* p = refl->head;
    while (p) {
        refl_node_t* temp = p->next;
        free(p);
        p = temp;
    }
}

int
refl_append(refl_t* refl, refl_opts_t options) {
    refl->used++;
    refl_node_t* p = refl->head;
    while (p->next) {
        p = p->next;
    }
    p->next = calloc(1, sizeof(refl_node_t));
    if (!p->next) {
        return MEMORY_REFUSED;
    }
    p->next->options = options;
    return SUCCESS;
}