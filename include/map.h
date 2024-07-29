#include <string.h>
#include "defs.h"
#include "utils.h"

#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#ifndef VALUE_TYPE
#error VALUE_TYPE undefined
#endif

#ifndef MAP_NAME
#define MAP_NAME map
#endif

#ifndef CAPACITY
#define CAPACITY 16
#endif

#ifndef LOAD_FACTOR
#define LOAD_FACTOR 0.75f
#endif


typedef struct {
	const char* key;
	VALUE_TYPE* value;
} map_pair;

typedef struct {
	map_pair* pairs;
	uint32_t count;
} map_bucket;

typedef struct {
	map_bucket* buckets;
	uint32_t count;
} MAP_NAME;

/** Creates an empty map with a specific initial capacity.
 * An initial capacity of a value equal to 0 is a no op.
 * @param map Pointer to the map to initialize.
 * @param capacity The initial capacity of this map.
 * @param load_factor The load factor. Determines when new buckets must be made.
 * @return [SUCCESS, OUT_OF_MEMORY, NO_OP]
 */
int map_create(MAP_TYPE* map, uint32_t capacity, float load_factor);

/** Creates an empty map with an initial capacity of 16 and loadd_factor of 0.75.
 * @param map Pointer to the map to initialize.
 * @return [SUCCESS, OUT_OF_MEMORY]
 */
int map_create(MAP_TYPE* map);

/** Frees a map.
 * @param map The map to free.
 * @return [SUCCESS, NULL_POINTER]
 */
int map_destroy(MAP_TYPE* map);

/** Copies the contents of map2 into map1. Each pointer cannot point to the same object or it is undefined behavior.
 * @param map1 The map to be modified.
 * @param map2 The map to be copied.
 * @return [SUCCESS, NULL_POINTER, OUT_OF_MEMORY]
 */
int map_copy(MAP_NAME* map1, const MAP_NAME* map2);

/**
 * Inserts a [key, value] pair into the map. Creates a copy of 'value' into the map.
 * @param map The map.
 * @param key The key.
 * @param value The value.
 * @return [SUCCESS, NULL_POINTER, EMPTY_STRING, OUT_OF_MEMORY, NO_OP]
 */
int map_insert(MAP_NAME* map, const char* key, const VALUE_TYPE* value);

/** Erases & frees the value at the given key in the map.
 * @param map The map.
 * @param key The key of the pair to remove.
 * @return [SUCCESS, NO_OP]
 */
int map_erase(MAP_NAME* map, const char* key);

/** Retrives the object at the specified key in the map.
 * @param map The map.
 * @param key The key.
 * @param out The out parameter that is updated to the value if found.
 * @return [FOUND, NOT_FOUND]
 */
int map_at(MAP_NAME* map, const char* key, VALUE_TYPE* out);

/** Determines if this map is empty or not.
 * @param map The map.
 * @return 1 if empty, 0 if nonempty.
 */
int map_empty(MAP_NAME* map);

/** Returns the number of [key, value] pairs in this map.
 * @param map The map.
 * @return The number of pairs in this map.
 */
size_t map_size(MAP_NAME* map);

/** Clears this map of all elements.
 * @param The map.
 */
void map_clear(MAP_NAME* map);


#undef MAP_H_INCLUDED