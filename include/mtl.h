/**
 * @file mtl.h
 * @author green
 * @date 7/30/2024
 * @brief Definitions of material (.mtl) related structures and functions.
 * Header file containing the material structure (mtl_t), the material library
 * structure (mtllib_t), and their related functions. Imports "map.h" to create
 * a map of [name, material] values, which may impact any files that import this
 * header file.
 * @see http://paulbourke.net/dataformats/mtl/
 */
#ifndef MTL_READER_H_INCLUDED
#define MTL_READER_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "utils.h"
#include "material_map.h"

/** The filename for the texture or image. */
typedef const char*							filename_t;
/** Turns texture blending in the horizontal direction on or off. Default is 1 
 * (on). */
typedef int 								blendu_t;
/** Turns texture blending in the vertical direction on or off. Default is 1 
 * (on). */
typedef int									blendv_t;
/** Specifies a bump multiplier and can only be used with the 'bump' 
 * statement. */
typedef float								bm_t;
/** Increases the sharpness or calirty of mip-mapped texture files. */
typedef float								boost_t;
/** Enables color correction. Default if 0 (off). */
typedef int									cc_t;
/** Turns texture clamping on or off. If clamping is on, texture coordinates are
 * restricted to [0, 1] in the u/v/w range. Default is off. */
typedef int									clamp_t;
/* @enum imfchan_t
 * @brief Specifies the channel used to create scalar or bump texture. */
typedef enum { r, g, b, m, l, z }			imfchan_t;
/** @struct mm_t
 * @brief Specifies the range over which scalar or color textures may vary. */
typedef struct { float base; float gain; }	mm_t;
/** Offsets the position of the texture map. Default is {0, 0, 0}. Order is 
 * [u, v, w]. */
typedef float	 							offset_t[3];
/** Scales the size of the texture pattern. Default is {1, 1, 1}. Order is 
 * [u, v, w]. */
typedef float								scale_t[3];
/** Specifies and enables turbulence for textures. Intended to add variance to 
 * original textures and allows simple images to be repeated over large 
 * areas. */
typedef float								turb_t[3];
/** @struct texres_t
 * @brief Specifies the resolution of the texture created when an image is used.
 * The default texture size is the largest power of 2 that does not exceed the
 * original image size. */
typedef struct { uint32_t w; uint32_t h; }	texres_t;
/** Enables or disables anti-aliasing of textures in this material.*/
typedef int 								map_aat_t;
/** Specifies the type of the reflection map.*/
typedef enum { sphere, cube_top, 		
			cube_front, cube_back,
			cube_left, cube_right }			refl_type_t;

/** @struct map_Ka_t
 * @brief Ambient reflectivity mapping options.
 */
typedef struct {
	filename_t 	filename;
	blendu_t 	blendu;
	blendv_t 	blendv;
	cc_t 		cc;
	clamp_t		clamp;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} map_Ka_t;
/** @struct map_Kd_t
 * @brief Diffuse reflectivity mapping options.
 */
typedef struct {
	filename_t 	filename;
	blendu_t 	blendu;
	blendv_t	blendv;
	cc_t		cc;
	clamp_t		clamp;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} map_Kd_t;

/** @struct map_Ks_t
 * @brief Specular reflectivity mapping options.
 */
typedef struct {
	filename_t	filename;
	blendu_t 	blendu;
	blendv_t	blendv;
	cc_t		cc;
	clamp_t		clamp;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} map_Ks_t;

/** @struct map_Ns_t
 * @brief Specular exponent mapping options.
 */
typedef struct {
	filename_t	filename;
	blendu_t	blendu;
	blendv_t	blendv;
	clamp_t		clamp;
	imfchan_t	imfchan;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} map_Ns_t;

/** @struct map_d_t
 * @brief Dissolve (transparency) mapping options.
 */
typedef struct {
	filename_t	filename;
	blendu_t	blendu;
	blendv_t	blendv;
	clamp_t		clamp;
	imfchan_t	imfchan;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} map_d_t;

/** @struct decal_t
 * @brief Texture options for the material.
 */
typedef struct {
	filename_t	filename;
	blendu_t	blendu;
	blendv_t	blendv;
	clamp_t		clamp;
	imfchan_t	imfchan;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} decal_t;

/** @struct disp_t
 * @brief Surface deformity options.
 */
typedef struct {
	filename_t	filename;
	blendu_t	blendu;
	blendv_t	blendv;
	clamp_t		clamp;
	imfchan_t	imfchan;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} disp_t;

/** @struct bump_t
 * @brief Bump mapping options.
 */
typedef struct {
	filename_t	filename;
	bm_t 		bm;
	clamp_t		clamp;
	blendu_t	blendu;
	blendv_t	blendv;
	imfchan_t	imfchan;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} bump_t;
typedef struct {
	filename_t filename;
	refl_type_t type;
	refl_list_t* next;
} refl_list_t;
/** @struct refl_t
 * @brief Reflection map options.
 */
typedef struct {
	filename_t*	filenames;
	uint32_t	num_files;
	refl_type_t	type;
	blendu_t 	blendu;
	blendv_t	blendv;
	cc_t		cc;
	clamp_t		clamp;
	mm_t		mm;
	offset_t	offset;
	scale_t		scale;
	turb_t		turbulence;
	texres_t	texres;
} refl_t;

/** @struct mtl_t
 * @brief Material data format as defined from
 * http://paulbourke.net/dataformats/mtl/.
 */
typedef struct {
	/** Name of this material type. */
    const char* name;
	/** Specifies the ambient reflectivity (Ka) using rgb values 
	* scaled from [0, 1]. */
    float ambient[3];
	/** Specifies the diffuse reflectivity (Kd) using rgb values scaled from 
	* [0, 1]. */
    float diffuse[3];
	/** Specifies the specular reflectivity (Ks) using rgb values scaled from 
	* [0, 1]. */
    float specular[3];
	/** Specifies the transmission filter (Tf) of this material using rgb values 
	* scaled from [0, 1]. */
    float tm_filter[3];
	/** Specifies the illumination model (illum) to use. Value between 0-10, see 
	* complete descriptions in illum.h. */
    uint32_t illum;
	/** Specifies the "dissolve" (d), or more well known as transparency, of 
	* this material. */
	struct {
		/** The dissolve or transparency value. */
		float value;	
		/** The "halo" flag behind the "d" command. Specifies that the material's 
		* transparency should be dependent on a surface's orientation with 
		* regards to the world camera. */
		int halo;
	} dissolve;				
	/** Specifies the specular exponent (Ns) for this material. */
    uint32_t specular_exponent;
	/** Specifies the sharpness of reflections (sharprness) of this material. */
    uint32_t sharpness;
	/** Specifies the optical density (Ni) for this material. Also known as 
	* "index of refraction". */
    float optical_density;
	/** Ambient map. */
	map_Ka_t map_Ka;
	/** Diffuse map. */
	map_Kd_t map_Kd;
	/** Specular map. */
	map_Ks_t map_Ks;
	/** Specular exponent (shininess) map. */
	map_Ns_t map_Ns;
	/** Dissolve (transparency) map. */
	map_d_t	map_d;
	/** Antialiasing map. */
	map_aat_t;
	/** Texture decaling. */
	decal_t	decal;
	/** Texture distortion map. */
	disp_t disp;
	/** Bump map. */
	bump_t bump;
	/** Reflection map. */
	refl_t refl_map;
} mtl_t;

/** A library of material types. Non-opaque. */
typedef struct {
	/** Name of this material library. */
    const char* name;
	/** Map of [material name, material] key/value pairs. */
    mat_map map;
} mtllib_t;

/** @brief Creates a new, empty material object.
 * @returns The material.
 */
mtl_t 
mtl_create(void);

/** @brief Destroy the material.
 * Destroys any heap allocated data in this material object, and sets all 
 * values to 0.
 * @param material The material object.
 */
void 
mtl_destroy(mtl_t* material);

/** @brief Prints the material's properties to standard output.
 * @param mat The material to print.
 */
void 
mtl_print(mtl_t* mat);

/** @brief Prints the material's properties to file. 
 * Prints the material's properties to the provided file. Assumes the file is
 * opened.
 * @param file The file pointer.
 * @param mat The material to print.
 * @return void
 */
void 
mtl_fprint(FILE* file, mtl_t* mat);

/** @brief Creates a new, empty material library. 
 * @return A material library.
 */
mtllib_t 
mtllib_create(void);

/** @brief Destroys this material library.
 * @param lib The material library.
 */
void 
mtllib_destroy(mtllib_t* lib);

/** @brief Prints the material library to standard output.
 * @param lib The material library.
 */
void 
mtllib_print(mtllib_t* lib);

/** @brief Print the material library to file. 
 * Prints the material library to the provided file. Assumes the file is
 * opened.
 * @param file The file pointer.
 * @param lib The material library.
 */
void 
mtllib_fprint(FILE* file, mtllib_t* lib);

/** @brief Read a material library from file. 
 * Reads the material from the provided filename into the provided material
 * library.
 * @param fn Filename for the material file.
 * @param lib Pointer to the material library.
 * @return Can return either: [MTL_SUCCESS, MTL_COULDNT_OPEN_FILE, 
 * MTL_OUT_OF_MEMORY].
 */
int 
mtllib_read(const char* fn, mtllib_t* lib);

#endif