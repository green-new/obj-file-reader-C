#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "obj.h"
#include "mtl.h"

int main(int argc, char** argv) {
    mesh_t mesh;
    clock_t then = clock();
    clock_t now;
    double duration;
    printf("Reading the file...\n");

    const char* fn = argv[1];
    if (argv[1] == NULL) {
        fn = "..\\..\\..\\models\\cube.obj";
    }

    int code = obj_read(fn, &mesh);

    printf("obj_read returned: %d\n", code);

    now = clock();
    duration = ((double)(now - then)) / CLOCKS_PER_SEC;

    printf("Mesh took: %f seconds (CPU time)\n", duration);

    obj_print(&mesh);
    obj_fwrite(&mesh, "output.txt");
    obj_destroy(&mesh);

    getchar();

    return 0;
}
