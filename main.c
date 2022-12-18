#include "obj_reader.h"
#include "time.h"

int main() {
    mesh_t teapot;
    FILE* output;
    output = fopen("output.txt", "w");
    clock_t teapot_mesh_time_start = clock();
    clock_t teapot_mesh_time_end;

    // Teapot currently doesn't work with face elements without the /'s...
    // Fix this later!
    int code = read_mesh("teapot.obj", &teapot);
    printf("read_mesh(...) returned: %d\n", code);
    teapot_mesh_time_end = clock();

    double teapot_cpu_time_used = ((double)(teapot_mesh_time_end - teapot_mesh_time_start)) / CLOCKS_PER_SEC;

    printf("Teapot mesh: %fs CPU time\n", teapot_cpu_time_used);
    
    printf("Teapot number of vertices: %d\n", teapot.num_vertices);
    printf("Teapot number of faces: %d\n", teapot.num_faces);

    mesh_write_to_file(&teapot, "output.txt");

    fclose(output);

    return 0;
}