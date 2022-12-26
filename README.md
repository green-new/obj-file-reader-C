# obj-file-reader-C
A .obj file reader in C

Compiles in C99

TODOs:
- Compatible with .mtl files
- Compilation and compatibility with C++ programs/compilers
- Complete Makefile
- Configure the mesh as it is begin read with bitflags
  - Examples:
  - Triangulate face if dimension > 3
  - Calculate normals (flat vs shading) -> Reconfigure face details
  - Auto-generated texture coordinates (UV mapping, triplanar mapping, cylindrical mapping, spherical mapping, xy/zy/xz mapping
  - Possibly more later
- Utility functions (Computational Geometry in C by Joseph O'Rourke) to determine concavity, convexity, hullness
