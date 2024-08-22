#include "illum.h"
#include "defs.h"

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

/**
 The following list defines the terms and vectors that are used in the 
illumination model equations:
 
 Term	Definition
 
 Ft	Fresnel reflectance										/ ?
 Ft	Fresnel transmittance									/ ?
 Ia	ambient light											/ ?
 I	light intensity											/ ?
 Ir	intensity from reflected direction						/ ?
 	(reflection map and/or ray tracing)						/ ?
 It	intensity from transmitted direction					/ ?
 Ka	ambient reflectance										/ mat->ambient
 Kd	diffuse reflectance										/ mat->diffuse
 Ks	specular reflectance									/ mat->specular
 Tf	transmission filter										/ mat->tm_filter
 
 Vector	Definition
 
 H	unit vector bisector between L and V					/ ?
 L	unit light vector										/ ?
 N	unit surface normal										/ ?
 V	unit view vector										/ ?
*/

void
illum_0(mtl_t* mat, float* rgb) {
	color[R] = mat->diffuse[R];
	color[G] = mat->diffuse[G];
	color[B] = mat->diffuse[B];
}

void
illum_1(mtl_t* mat, float* rgb, light_t* lights, uint32_t n_lights) {
	
}