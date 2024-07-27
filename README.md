# obj-file-reader-C
This lightweight C library is used to read and parse .obj and .mtl files for their geometric and visual data into simple, readable array-like formats, helping you modularize your rendering logic and geometry logic.

# Compilation
Linux: Run the 'Makefile' with GNU's 'make' command with GCC.
Windows: Run the 'compile.bat' batch executable.
Releases: Download one of the releases to get a precompiled library.

# Features
- Vertex-face list as mesh representation
- Print or write to file mesh contents
- That's about it

# Planned features
- License
- .obj writer (procedural geometry creation)
- Full compatiblity with .mtl files
- Compilation and compatibility with C++ programs/compilers
- Complete Makefile
- Configure the mesh read with bitflags
  - Examples:
  - Triangulate face if dimension > 3
  - Calculate normals (flat vs shading) -> Reconfigure face details
  - Auto-generated texture coordinates (UV mapping, triplanar mapping, cylindrical mapping, spherical mapping, xy/zy/xz mapping
  - Possibly more later
- Utility functions to determine convexity, hullness, etc.
