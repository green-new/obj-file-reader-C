/**
 * @file map.h
 * @author green
 * @date 7/30/2024
 * @brief Definitions of the material map type and map functions.
 * Header file containing the details required to create, destroy, and modify a 
 * type-erased .MTL map structure in C.
 */

#ifndef MATMAP_H_INCLUDED
#define MATMAP_H_INCLUDED

#include <string.h>
#include "defs.h"
#include "utils.h"
#include "mtl.h"

// TODO: 'const' qualified map pointers for functions that make no 
// modifications.

/** @struct map_pair
 * @brief A structure containing a [key, value] pair for use in a map.
 * Contains a "const char*" key, and a specified VALUE_TYPE pointer based on 
 * the implementer's definition.
*/
typedef struct {
	/** Heap allocated; string name of this material. */
	const char* key;
	/** The material value. */
	mtl_t value;
	/** Boolean value if this pair has been rehashed. Sets to 1 during the 
	rehashing period, set back to 0 after rehashing has been completed. */
	int hash;
} map_pair;

/** @struct map_bucket
 * @brief A structure containing a contiguous and dynamic list of "map_pair"'s.
*/
typedef struct {
	/** Heap allocated; dynamic array of pairs. */
	map_pair* pairs;
	/** Number of contiguous "map pairs" in this bucket. */
	uint32_t capacity;
	/** Number of contiguous "active map pairs" in this bucket. */
	uint32_t active;
} map_bucket;

/** @struct keys_list_t
 * @brief A structure containing a contiguous list of keys as strings.
*/
typedef struct {
	uint32_t used;
	const char** keys;
} keys_list_t;

/** @struct mat_map
 * @brief A map of [name, mtl_t] pairs. 
 * Contains a heap allocated list of material name, and material object pairs. 
 * The "name" is the same as the material's "name. The pairs are stored in 
 * buckets which are addressable by the name's hash code. The map uses the djb2 
 * hash algorithm.
 */
typedef struct {
	/** Heap allocated; contiguous list of buckets addressable by hash code, mod
	the current capacity of the map. */
	map_bucket* buckets;
	/** Maximum capacity of the map at a point. Once the number of active 
	buckets reaches the load factor, this capacity is doubled from its previous
	value. Default is 16. */
	uint32_t capacity;
	/** The number of heap allocated buckets which may or may not contain any
	pairs. */
	uint32_t active;
	/** The number of buckets that, once reached, increases the capacity by 
	double its previous value. Default is 12. */
	uint32_t load_factor;
} mat_map;

/** Creates an empty map with an initial capacity of 16 and load_factor of 0.75.
 * @param map Pointer to the map to initialize.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int 
map_create(mat_map* map);

/** Frees a map.
 * @param map The map to free.
 */
void 
map_destroy(mat_map* map);

/** Copies the contents of map2 into map1. 
 * Each pointer cannot point to the same object or it is undefined behavior.
 * @param map1 The map to be modified. restricted.
 * @param map2 The map to be copied. restricted.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int 
map_copy(mat_map* map1, const mat_map* map2);

/**
 * Inserts a [key, value] pair into the map. 
 * If the number of buckets exceeds the load_factor,
 * the hash table is automatically rehashed and inserts the new pair.
 * Creates a copy of 'value' into the map. If the map is 
 * @param map The map.
 * @param key The key.
 * @param value The value.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int 
map_insert(mat_map* map, const char* key, mtl_t value);

/** Erases & frees the value at the given key in the map.
 * @param map The map.
 * @param key The key of the pair to remove.
 */
void  
map_erase(mat_map* map, const char* key);

/** Retrives the object at the specified key in the map.
 * @param map The map.
 * @param key The key.
 * @param out The out parameter that is updated to the value if found. NULL if
 * no material was found.
 * @return NOT_FOUND if it wasn't found, SUCCESS otherwise.
 */
int  
map_at(mat_map* map, const char* key, mtl_t** out);

/** Determines if this map is empty or not.
 * @param map The map.
 * @return 1 if empty, 0 if nonempty.
 */
int 
map_empty(mat_map* map);

/** Returns the number of [key, value] pairs in this map.
 * @param map The map.
 * @return The number of pairs in this map.
 */
size_t 
map_size(mat_map* map);

/** Clears this map of all elements.
 * @param map The map.
 */
int 
map_clear(mat_map* map);

/** Return a heap-allocated list of keys.
 * @param map The map.
 */
keys_list_t
map_keys(mat_map* map);

/** Destroys the list. Does not destroy the keys in the map.
 * @param map The map.
 */
void
keys_list_destroy(keys_list_t* list);

#endif