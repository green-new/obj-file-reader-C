#include "defs.h"
#include "token.h"
#include <stdlib.h>
#include <stdio.h>

void printbuf(buffer_t* buf) {
    char c[64] = {0};
    for (unsigned j = 0; j < buf->length; j++) {
        c[j] = buffer_start(*buf)[j];
    }
    printf("%s", c);
}

int test_case4() {
    int code = 0;
    const char* str = "This;is;a;string;that;I;will;\
tokenize;";
    token_list_t list = (token_list_t) { .head = NULL, .used = 0 };
    if ((code = tokenlist_create(&list)) != SUCCESS) {
        return code;
    } 
    if ((code = tokenize(&list, str, ";")) != SUCCESS) {
        return code;
    }
    token_node_t* p = list.head;
    printf("\n[");
    const char* separator = "";
    while (p) {
        printf("%s", separator);
        printbuf(&(*p).buf);
        separator = ", ";
        p = p->next;
    }
    printf("]");
    return 0;
}

int test_case3() {
    int code = 0;
    const char* str = "---This------is--a----string-----that---I------will---\
tokenize----";
    token_list_t list = (token_list_t) { .head = NULL, .used = 0 };
    if ((code = tokenlist_create(&list)) != SUCCESS) {
        return code;
    } 
    if ((code = tokenize(&list, str, "---")) != SUCCESS) {
        return code;
    }
    token_node_t* p = list.head;
    printf("\n[");
    const char* separator = "";
    while (p) {
        printf("%s", separator);
        printbuf(&(*p).buf);
        separator = ", ";
        p = p->next;
    }
    printf("]");
    return 0;
}

int test_case2() {
    int code = 0;
    const char* str = ";;;This;is;a;string;that;I;will;tokenize;;;;";
    token_list_t list = (token_list_t) { .head = NULL, .used = 0 };
    if ((code = tokenlist_create(&list)) != SUCCESS) {
        return code;
    } 
    if ((code = tokenize(&list, str, ";")) != SUCCESS) {
        return code;
    }
    token_node_t* p = list.head;
    printf("\n[");
    const char* separator = "";
    while (p) {
        printf("%s", separator);
        printbuf(&(*p).buf);
        separator = ", ";
        p = p->next;
    }
    printf("]");
    return 0;
}

int test_case1() {
    int code = 0;
    const char* str = ";asdfThis;asdfis;asdfa;asdfstring;asdfthat;asdfI;asdf\
will;asdftokenize;asdf";
    token_list_t list = (token_list_t) { .head = NULL, .used = 0 };
    if ((code = tokenlist_create(&list)) != SUCCESS) {
        return code;
    } 
    if ((code = tokenize(&list, str, ";asdf")) != SUCCESS) {
        return code;
    }
    token_node_t* p = list.head;
    printf("\n[");
    const char* separator = "";
    while (p) {
        printf("%s", separator);
        printbuf(&(*p).buf);
        separator = ", ";
        p = p->next;
    }
    printf("]");
    return 0;
}

int test_case0() {
    int code = 0;
    const char* str = ";This;is;a;string;that;I;will;tokenize;";
    token_list_t list = (token_list_t) { .head = NULL, .used = 0 };
    if ((code = tokenlist_create(&list)) != SUCCESS) {
        return code;
    }
    if ((code = tokenize(&list, str, ";")) != SUCCESS) {
        return code;
    }
    token_node_t* p = list.head;
    printf("\n[");
    const char* separator = "";
    while (p) {
        printf("%s", separator);
        printbuf(&(*p).buf);
        separator = ", ";
        p = p->next;
    }
    printf("]");
    return 0;
}

int main() {
    int code = 0;
    code = test_case0();
    code = test_case1();
    code = test_case2();
    code = test_case3();
    code = test_case4();
    return code;
}