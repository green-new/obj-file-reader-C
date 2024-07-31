#include "map.h"

#ifdef MAP_IMPL

// -----------------------------------------------------------------------------
// Static utility
// -----------------------------------------------------------------------------

static const float default_load_factor = 0.75f;
static const uint32_t default_capacity = 16;
static const size_t default_bucket_capacity = 4;

/** Produces a hash code from the provided string.
 * Uses the djb2 algorithm.
 * @param str The string.
 * @return 32-bit signed hash code.
 */
static int32_t hash(const char* str) {
	int32_t hash = 5381;
	int c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

/** Returns the pair if found in the specified bucket, NULL if none exists.
* @param bucket The bucket to search the pair for.
* @param key The key of the pair to find.
* @returns The pair if found, NULL if no pair was found.
*/
static map_pair* get_pair(map_bucket* bucket, const char* key) {
	if (bucket->count == 0) {
		return NULL;
	}
	for (int i = 0; i < bucket->count; i++) {
		map_pair* pair = &bucket->pairs[i];
		if (pair && pair->key && pair->value && strequ(key, pair->key)) {
			return pair;
		}
	}
	return NULL;
}

/** Attempts to find the bucket with the given key.
* Uses the hash function to find the correct bucket given the key.
* @param map The map used to search for the bucket.
* @param key The string key that'll be associated with the to-be-found bucket.
* @return The bucket if found, NULL if no bucket was found.
*/
static map_bucket* get_bucket(MAP_NAME* map, const char* key) {
	if (!map) {
		return NULL;
	}
	uint32_t index = hash(key) % map->capacity;
	map_bucket* bucket = &map->buckets[index];
	return bucket;
}

/** Destroys a given pair. Assumes "pair" is non-null.
* Will free the memory allocated to "pair->key" and "pair->value".
* @param pair Pointer to a map pair. Cannot be null.
*/
static void destroy_pair(map_pair* pair) {
	// Must be casted to (void*) because key is a (const char*) and technically 
	// cannot be modified.
	free((void*)pair->key);
	free(pair->value);
}

/** Destroys a given bucket. Assumes "bucket" is non-null.
* Destroys the "pairs" within the bucket and sets their properties to NULL, 
* and sets the "count" to "0".
* @param bucket The bucket to destroy.
*/
static void destroy_bucket(map_bucket* bucket) {
	for (size_t i = 0; i < bucket->count; i++) {
		map_pair* pair = &bucket->pairs[i];
		if (!pair) {
			continue;
		}
		// Destroy the pair's properties
		free((void*)pair->key);
		free(pair->value);
		pair->key = NULL;
		pair->value = NULL;
	}
	free(bucket->pairs);
	bucket->pairs = NULL;
	bucket->count = 0;
}

/** Swap the given buckets. */
static void 
swap_bucket(map_bucket* restrict bucket1, map_bucket* restrict bucket2) {
	map_bucket temp = *bucket1;
	*bucket1 = *bucket2;
	*bucket2 = temp;
}

/** Swap the given pairs. */
static void 
swap_pair(map_pair* restrict pair1, map_pair* restrict pair2) {
	map_pair temp = *pair1;
	*pair1 = *pair2;
	*pair2 = temp;
}

/** Destroy the bucket at the given index and replace it with the last one. */
static int 
destroy_bucket_in_map(MAP_NAME* map, size_t index) {
	if (index >= map->capacity) {												// No buckets in this map
		return NO_OP;
	}
	map_bucket* bucket = &map->buckets[index];
	if (!bucket) { 																// Does not exist
		return NULL_POINTER;
	}
	if (map->capacity == 1) {
		destroy_bucket(bucket);	
		map->capacity = 0;
		return SUCCESS;
	}
	// More than 2 buckets in map.
	map_bucket* last = &map->buckets[bucket->count - 1];
	swap_bucket(bucket, last);
	destroy_bucket(last); 														// "last" still points to the last element of the array
	map_bucket* temp = realloc(map->buckets, 
	sizeof map_bucket * (bucket->count - 1));									// Must reallocate the bucket array
	if (!temp) {
		return MEMORY_REFUSED;
	}
	map->buckets = temp;
	map->capacity--;
	return SUCCESS;
}

/** Pushes an already created pair into the end of the bucket.
*/
static int 
push_pair(map_bucket* bucket, map_pair* pair) {
	&bucket->pairs = realloc(bucket->pairs, (++bucket->count * sizeof map_pair));
	if (!(*bucket->pairs)) {
		return MEMORY_REFUSED;
	}
	bucket->pairs[bucket->count - 1] = *pair; // Create a pair at the end of the bucket
}

/** Swap the given pair with the last pair in the list, and delete it.
*/
static void 
delete_pair_in_bucket(map_bucket* bucket, size_t index) {
	if (bucket->count == 0) {
		return;
	}
	map_pair* pair = &bucket->pairs[index];
	if (bucket->count == 1) {
		destroy_pair(pair);
		bucket->count = 0;
		return;
	} 
	// Bucket count is 2 or more.
	map_pair* last = &bucket->pairs[bucket->count - 1];
	swap_pair(pair, last);
	destroy_pair(last);
	// Must reallocate the pairs array
	map_pair* temp = realloc(bucket->pairs, sizeof map_pair * (bucket->count - 1));
	if (!temp) {
		return MEMORY_REFUSED;
	}
	bucket->pairs = temp;
	bucket->count--;
	return SUCCESS;
}

// Rehashes the map.
// Does not create a new map; uses the original map.
// Does not increase capacity. Map capacity must be increased before calling 
// this, or else its effectively a no-op.
static void 
map_rehash(MAP_NAME* map) {
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		if (!bucket) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->count; j++) {
			map_pair* pair = &bucket->pairs[j];
			const char* key = pair->key;
			if (!pair) {
				continue;
			}
			uint32_t index = hash(key) % map->capacity;
			map_bucket* new_bucket = &map->buckets[index];
			if (!new_bucket) {
				map->buckets[index] = create_bucket(4);
			}
			if (bucket != new_bucket) {
				if (bucket->count) {
					
				}
				swap_pair(pair, &bucket->pairs[bucket->count - 1];
			}
		}
	}
}

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

typedef struct {
	const char* key;		// Heap allocated
	VALUE_TYPE* value;		// Heap allocated
} map_pair;

typedef struct {
	map_pair* pairs;		// Heap allocated, stack containing
	uint32_t count;
} map_bucket;

typedef struct {
	map_bucket* buckets;	// Heap allocated
	uint32_t capacity;
	uint32_t active;
	float load_factor;
} MAP_NAME;

int 
map_create(MAP_TYPE* map, uint32_t capacity, float load_factor) {
	map = calloc(1, sizeof MAP_TYPE);
	if (!map) {
		return MEMORY_REFUSED;
	}
	map->load_factor = load_factor;
	map->capacity = capacity;
	map->buckets = calloc(capacity, sizeof map_bucket);
	if (!map->buckets) {
		free(map);
		return MEMORY_REFUSED;
	}
	// Set the properties for each bucket
	for (uint32_t i = 0; i < capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		bucket->pairs = NULL;
		bucket->count = 0;
	}
	return SUCCESS;
}

int 
map_create(MAP_TYPE* map) {
	return map_create(map, default_capacity, default_load_factor);
}

void 
map_destroy(MAP_TYPE* map) {
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = map->buckets[i];
		if (!bucket) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->count; j++) {
			map_pair* pair = bucket->pair[j];
			if (!pair) {
				continue;
			}
			if (pair->key) {
				free(pair->key);
			}
			if (pair->value) {
				free(pair->value);
			}
		}
		free(bucket->pairs);
	}
	free(map->buckets);
	free(map);
}

int 
map_copy(MAP_NAME* restrict map1, const MAP_NAME* restrict map2) {
	map_clear(map1); // Clear the first map
	for (uint32_t i = 0; i < map2->count; i++) {
		map_bucket* bucket = &map2->buckets[i];
		if (!bucket) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->count; j++) {
			map_pair* pair = bucket->pairs[j];
			if (!pair) {
				continue;
			}
			int code = map_insert(map1, pair->key, pair->value); // Insert to the first map. Will copy pair properties
			if (!code) {
				return code; // Check map_insert error codes
			}
		}
	}
	return SUCCESS;
}

int 
map_insert(MAP_NAME* map, const char* key, const VALUE_TYPE* value) {
	map_pair pair;											// Create the [key, value] pair
	pair.key = calloc(1, strlen(key) + 1);
	if (!pair.key) {
		return MEMORY_REFUSED;
	}
	strcpy(pair.key, key);
	pair.value = calloc(1, sizeof VALUE_TYPE);
	if (!pair.value) {
		free(pair.key);
		return MEMORY_REFUSED;
	}
	memcpy(pair.value, value, sizeof VALUE_TYPE);
	int requires_rehash = 0;								// Determine if we need to rehash the map early on
	if (map->active >= map->capacity * map->load_factor) {
		requires_rehash = 1;
	}
	if (map->capacity == 0) { 								// User created a map with 0 capacity; change it to the default and move on
		map->capacity = default_capacity;
		map_bucket bucket;									// Create the bucket
		bucket->pairs = NULL;
		bucket->count = 0;
		if (!bucket) {
			return MEMORY_REFUSED;
		}
		bucket->pairs[bucket->count - 1] = pair;
		map->buckets[map->capacity - 1] = bucket;
		
		return SUCCESS;
	}
	map_bucket* bucket = get_bucket(map, key);
	// Bucket already exists with this key's hash
	if (bucket) {
		if (bucket->count == 0) { // No pairs exist. Create a single pair in this bucket
			bucket->count += 1;
			if (!pair) {
				return MEMORY_REFUSED;
			}
			bucket->pairs[count - 1] = pair;
		} else { // Bucket exists, and more than 0 pairs exist in this bucket.
			
		}
	// No buckets left
	} else {
		// Double the capacity by adding new buckets
		map->capacity *= 2;
		map->buckets = realloc(
	}
	
	return SUCCESS;
}

int 
map_erase(MAP_NAME* map, const char* key) {
	if (!map) {
		return NULL_POINTER;
	}
	
}

int 
map_at(MAP_NAME* map, const char* key, VALUE_TYPE* out) {
	
}

int 
map_empty(MAP_NAME* map) {
	return map->capacity == 0;
}

size_t 
map_size(MAP_NAME* map) {
	size_t count = 0;
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = map->buckets[i];
		if (!bucket) {
			continue;
		}
		count += bucket->count;
	}
	return count;
}

int 
map_clear(MAP_NAME* map) {
	for (uint32_t i = 0; i < map2->count; i++) {
		map_bucket* bucket = map2->buckets[i];
		if (!bucket) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->count; j++) {
			map_pair* pair = bucket->pairs[j];
			if (!pair) {
				continue;
			}
			int code = map_insert(map1, pair->key, pair->value);
			if (!code) {
				return code; // [MEMORY_REFUSED]
			}
		}
		bucket->count = 0;
		free(bucket);
	}
	map->capacity = 0;
	return SUCCESS;
}


#endif