/**
 * @file map.h
 * @author green
 * @date 7/30/2024
 * @brief Definitions of opqaue "map" types, map type macros, and map functions.
 * Header file containing the details required to create, destroy, and modify a 
 * type-erased map structure in C.
 */
#include <string.h>
#include "defs.h"
#include "utils.h"
#include "mtl.h"

#ifndef MATMAP_H_INCLUDED
#define MATMAP_H_INCLUDED

/** @struct map_pair
 * @brief A structure containing a [key, value] pair for use in a map.
 * Contains a "const char*" key, and a specified VALUE_TYPE pointer based on 
 * the implementer's definition.
*/
typedef struct 
map_pair;
typedef struct 
map_bucket;
typedef struct 
mat_map;

/** Creates an empty map with a specific initial capacity.
 * @param map Pointer to the map to initialize. Cannot be NULL.
 * @param capacity The initial capacity of this map. Must be an unsigned integer greater than or equal to 0.
 * @param load_factor The load factor. When load_factor*map_capacity buckets have more than 1 pair, the map is automatically rehashed.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int 
map_create(mat_map* map, uint32_t capacity, float load_factor);

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

/** Copies the contents of map2 into map1. Each pointer cannot point to the same object or it is undefined behavior.
 * @param map1 The map to be modified. Cannot be null. Cannot be == to map2.
 * @param map2 The map to be copied. Cannot be null. Cannot be == to map1.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int 
map_copy(mat_map* map1, const mat_map* map2);

/**
 * Inserts a [key, value] pair into the map. 
 * If the number of buckets exceeds the load_factor of the current capacity, the hash table is automatically rehashed and inserts the new pair.
 * Creates a copy of 'value' into the map. If the map is 
 * @param map The map.
 * @param key The key.
 * @param value The value.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int 
map_insert(mat_map* map, const char* key, const mtl_t* value);

/** Erases & frees the value at the given key in the map.
 * @param map The map.
 * @param key The key of the pair to remove.
 * @return [SUCCESS, NO_OP]
 */
int 
map_erase(mat_map* map, const char* key);

/** Retrives the object at the specified key in the map.
 * @param map The map.
 * @param key The key.
 * @param out The out parameter that is updated to the value if found.
 * @return [FOUND, NOT_FOUND]
 */
int 
map_at(mat_map* map, const char* key, mtl_t* out);

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
 * @param The map.
 */
void 
map_clear(mat_map* map);

#undef MAP_H_INCLUDED