#include "material_map.h"

#ifdef MAP_IMPL

// -----------------------------------------------------------------------------
// Static utility
// -----------------------------------------------------------------------------

static const uint32_t default_load_factor = 12;
static const uint32_t default_capacity = 16;
static const uint32_t default_initial_bucket_count = 4;
static const uint32_t uint32_max = 4294967295

/** Produces a hash code from the provided string.
 * Uses the djb2 algorithm.
 * @param str The string.
 * @return 32-bit signed hash code.
 */
static int32_t 
hash(const char* str) {
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
static map_pair* 
get_pair(map_bucket* bucket, const char* key) {
	if (bucket->capacity == 0) {
		return NULL;
	}
	for (int i = 0; i < bucket->capacity; i++) {
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
static 
map_bucket* get_bucket(mat_map* map, const char* key) {
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
static void 
destroy_pair(map_pair* pair) {
	// Must be casted to (void*) because key is a (const char*) and technically 
	// cannot be modified.
	free((void*)pair->key);
	free(pair->value);
	pair->key = NULL;
	pair->value = NULL;
}

/** Destroys a given bucket's contents. Assumes "bucket" is non-null.
* Destroys the "pairs" within the bucket and sets their properties to NULL, 
* and sets the "count" to "0".
* @param bucket The bucket to destroy. Assumes it is valid.
*/
static void 
destroy_bucket(map_bucket* bucket) {
	for (size_t i = 0; i < bucket->capacity; i++) {
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
	bucket->capacity = 0;
}

/** Swap the given buckets' contents. 
* @param bucket1 The first bucket.
* @param bucket2 The second bucket.
*/
static void 
swap_bucket(map_bucket* restrict bucket1, map_bucket* restrict bucket2) {
	map_bucket temp = *bucket1;
	*bucket1 = *bucket2;
	*bucket2 = temp;
}

/** Swap the given pairs' contents. 
* @param The first pair.
* @param The second pair.
*/
static void 
swap_pair(map_pair* restrict pair1, map_pair* restrict pair2) {
	map_pair temp = *pair1;
	*pair1 = *pair2;
	*pair2 = temp;
}

/** Destroy the bucket at the given index and replace it with the last.
 * Replaces the specified bucket in the map with the last bucket in the array.
 * @param map The map.
 * @param index The index of the bucket in the "buckets" array. Assumes this
 * index is valid, or else its undefined.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
static void 
destroy_bucket_in_map(mat_map* map, size_t index) {
	map_bucket* bucket = &map->buckets[index];
	destroy_bucket(bucket);	
	map->capacity = 0;
	map->buckets = temp;
	map->active--;
}

/** Swap the given pair with the last pair in the list, and delete it.
* Assumes "index" is within bounds.
* Assumes there is a non-zero number of active pairs.
* Simply zeros out the struct. Call "destroy_pair" to free memory.
* @param bucket The bucket from which the pair exists.
* @param index The index of the pair in the pairs array.
*/
static void
rm_pair_from_bucket(map_bucket* bucket, uint32_t index) {
	// Case: "pair" is not the "last pair"
	if (index < bucket->active - 1) {
		bucket->pairs[index] = bucket->pairs[bucket->active - 1];
	}
	bucket->pairs[bucket->active - 1] = {0};
	bucket->active--;
}

/** Copys a pre-existing pair to the end of the "pairs" array within the 
 * provided bucket.
 * Assumes the number of active pairs does not or will not exceed the bucket
 * capacity.
 * @param bucket The bucket.
 * @param pair The pair.
 */
static void
add_pair_to_bucket(map_bucket* bucket, map_pair* pair) {
	// Copy the pair to the end
	bucket->active++;
	bucket->pairs[bucket->active - 1] = *pair;
}

/** "Moves" (copys and removes) a pair from one bucket to another.
 * Assumes the pair is within the "from" bucket.
 * Assumes the number of active pairs in the "to" bucket does not or will not 
 * exceed the bucket capacity.
 * @param from The bucket the pair is moving from.
 * @param to The bucket the pair is moving to.
 * @param pair The pair.
 */
static void mv_pair_from_buckets(map_bucket* from, map_bucket* to, map_pair* pair, uint32_t pairIndexInFrom) {
	rm_pair_from_bucket(from, pairIndexInFrom);
	add_pair_to_bucket(to, pair);
}

/** @brief Rehashes the map.
 * Does not create a new map; uses the original map.
 * Does not increase capacity. Map capacity must be increased before calling 
 * this, or else its effectively a no-op.
 * Makes reallocation calls on buckets if the size they have during rehashing
 * is not big enough.
 * @return SUCCESS, MEMORY_REFUSED
 */
static int 
map_rehash(mat_map* map) {
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		if (!bucket || bucket->active == 0) {
			continue;
		}
		// Determine if we need to increase individual 
		// bucket capacity. Count actives based on hash codes
		for (uint32_t j = 0; j < bucket->active; j++) {
			map_pair* pair = &bucket->pairs[j];
			const char* key = pair->key;
			if (!pair || pair->hash) {
				continue;
			}
			// Pair is not null and not rehashed
			uint32_t index = hash(key) % map->capacity;
			map_bucket* rehash_bucket = &map->buckets[index];
			if (bucket != rehash_bucket) {
				rehash_bucket->active++;
				bucket->active--;
			}
			
			// If bucket is equal to the rehashed bucket, then its a no-op
		}
		// Increase individual bucket capacity if # of actives exceeds capacity
		for (uint32_t j = 0; j < map->capacity
		
		// Move the pairs based on hash code (rehashing)
		for (uint32_t j = 0; j < bucket->active; j++) {
			// Assumes a rehash_bucket is never null (should be if allocation
			// did not fail), and is atleast zero-initialized
			if (bucket != rehash_bucket) {
				if (rehash_bucket->capacity <= 0) {
					// No capacity - increase capacity then move on
					rehash_bucket->capacity = default_initial_bucket_count;
					map_pair* temp = realloc(rehash_bucket->pairs, 
						default_initial_bucket_count);
					if (!temp) {
						return MEMORY_REFUSED;
					}
					rehash_bucket->pairs = temp;
				} else if (rehash_bucket->active + 1 > capacity) {
					// Adding a pair to the rehash bucket will exceed capacity,
					// capacity must increase
					
				}
				mv_pair_from_buckets(bucket, rehash_bucket, pair);
			}
		}
	}
	// Set "hash" property to 0 for all pairs
	for (uint32_t i = 0; i < map->active; i++) {
		map_bucket* bucket = &map->buckets[i];
		for (uint32_t j = 0; j < bucket->active; j++) {
			map_pair* pair = &bucket->pairs[l];
			pair->hash = 0;
		}
	}
	return SUCCESS;
}

// -----------------------------------------------------------------------------
// Implementation
// -----------------------------------------------------------------------------

typedef struct {
	/** Heap allocated; string name of this material. */
	const char* key;
	/** The material value. */
	mtl_t value;
	/** Boolean value if this pair has been rehashed. Sets to 1 during the 
	rehashing period, set back to 0 after rehashing has been completed. */
	int hash;
} map_pair;

typedef struct {
	/** Heap allocated; dynamic array of pairs. */
	map_pair* pairs;
	/** Number of contiguous "map pairs" in this bucket. */
	uint32_t capacity;
	/** Number of contiguous "active map pairs" in this bucket. */
	uint32_t active;
} map_bucket;

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

int 
map_create(MAP_TYPE* map, uint32_t capacity, uint32_t load_factor) {
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
		bucket->pairs = calloc(default_initial_bucket_count, sizeof map_pair);
		bucket->capacity = default_initial_bucket_count;
		bucket->active = 0;
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
		for (uint32_t j = 0; j < bucket->capacity; j++) {
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
map_copy(mat_map* restrict map1, const mat_map* restrict map2) {
	map_clear(map1); // Clear the first map
	for (uint32_t i = 0; i < map2->count; i++) {
		map_bucket* bucket = &map2->buckets[i];
		if (!bucket) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->capacity; j++) {
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
map_insert(mat_map* map, const char* key, const mtl_t* value) {
	map_pair pair;											// Create the [key, value] pair
	pair.key = calloc(1, strlen(key) + 1);
	if (!pair.key) {
		return MEMORY_REFUSED;
	}
	strcpy(pair.key, key);
	pair.value = calloc(1, sizeof mtl_t);
	if (!pair.value) {
		free(pair.key);
		return MEMORY_REFUSED;
	}
	memcpy(pair.value, value, sizeof mtl_t);
	// Determine if we need to rehash the map early on
	int requires_rehash = 0;
	if (map->active >= map->load_factor) {
		requires_rehash = 1;
	}
	// User created a map with 0 capacity; change it to the default and move on
	if (map->capacity == 0) {
		map->capacity = default_capacity;
		map->active = default_capacity;
		map_bucket bucket;
		bucket->pairs = NULL;
		bucket->capacity = 0;
		bucket->pairs[bucket->capacity - 1] = pair;
		map->buckets[map->capacity - 1] = bucket;
		
		return SUCCESS;
	}
	map_bucket* bucket = get_bucket(map, key);
	// Bucket already exists with this key's hash
	if (bucket) {
		// No pairs exist. Create a single pair in this bucket
		if (bucket->capacity == 0) {
			bucket->capacity += 1;
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
map_erase(mat_map* map, const char* key) {
	if (!map) {
		return NULL_POINTER;
	}
	
}

int 
map_at(mat_map* map, const char* key, mtl_t* out) {
	
}

int 
map_empty(mat_map* map) {
	return map->capacity == 0;
}

size_t 
map_size(mat_map* map) {
	// Get the number of pairs.
	size_t count = 0;
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = map->buckets[i];
		if (!bucket) {
			continue;
		}
		count += bucket->capacity;
	}
	return count;
}

int 
map_clear(mat_map* map) {
	for (uint32_t i = 0; i < map2->count; i++) {
		map_bucket* bucket = map2->buckets[i];
		if (!bucket) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->capacity; j++) {
			map_pair* pair = bucket->pairs[j];
			if (!pair) {
				continue;
			}
			int code = map_insert(map1, pair->key, pair->value);
			if (!code) {
				return code; // [MEMORY_REFUSED]
			}
		}
		bucket->capacity = 0;
		free(bucket);
	}
	map->capacity = 0;
	return SUCCESS;
}


#endif