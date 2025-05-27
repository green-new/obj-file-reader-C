/**
 * @file refl.h
 * @author green
 * @date 8/27/2024
 * @brief Definition of the reflection map used in .mtl files. 
 * @see http://paulbourke.net/dataformats/mtl/
 */
#ifndef MTL_REFLMAP_H_INCLUDED
#define MTL_REFLMAP_H_INCLUDED

#include <stdlib.h>
#include "defs.h"
#include "mtl_opts.h"

/** @struct refl_opts_t
 * @brief Reflection map options.
 */
typedef struct {
	filename_t 		filename;
	refl_type_t 	type;
	blendu_t 		blendu;
	blendv_t		blendv;
	cc_t			cc;
	clamp_t			clamp;
	mm_t			mm;
	offset_t		offset;
	scale_t			scale;
	turb_t			turbulence;
	texres_t		texres;
} refl_opts_t;

/** 
 * @struct refl_node_t
 * @brief Reflection map list node containing individual options.
 */
typedef struct refl_node_t {
	refl_opts_t			options;
	struct refl_node_t*	next;
} refl_node_t;

/** 
 * @struct refl_t
 * @brief Reflection map options.
 */
typedef struct {
	refl_node_t*	head;
	uint32_t 		used;
} refl_t;

// -----------------------------------------------------------------------------
// 		refl_t.
// -----------------------------------------------------------------------------

/** 
 * @brief Create an empty reflection map.
 * @param refl The reflection map to create.
 * @param options The options to use to create a first node.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int
refl_create(refl_t* refl, refl_opts_t options);

/** 
 * @brief Destroy a reflection map.
 */
void
refl_destroy(refl_t* refl);

/** 
 * @brief Append to the reflection map with new options.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int
refl_append(refl_t* refl, refl_opts_t options);

#endif