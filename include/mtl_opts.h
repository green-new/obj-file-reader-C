/**
 * @file mtl_opts.h
 * @author green
 * @date 7/30/2024
 * @brief Definitions of material (.mtl) related options in mapping commands.
 * @see http://paulbourke.net/dataformats/mtl/
 */
#ifndef MTL_OPTIONS_H_INCLUDED
#define MTL_OPTIONS_H_INCLUDED

#include <stdint.h>

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

#endif
