#include "illum.h"
#include "defs.h"

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

void
illum_0(mtl_t* mat, float* rgb) {
	color[R] = mat->diffuse[R];
	color[G] = mat->diffuse[G];
	color[B] = mat->diffuse[B];
}

void
illum_1(mtl_t* mat, float* rgb) {
	
}