#include "mtl.h"
#include "mtllib.h"

int test_mtl_read(mtllib_t* lib) {
    return mtllib_read("D:\\Programming\\obj-file-reader-C\\models\\cube.mtl", lib);
}

int main() {
    int code = SUCCESS;
    mtllib_t lib;
    mtllib_create(&lib);
    if ((code = test_mtl_read(&lib)) != SUCCESS) {
        printf("Material failed\n");
    }
    mtllib_print(&lib);
    return 0;
}