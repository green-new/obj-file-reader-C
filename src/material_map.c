#include "material_map.h"

// -----------------------------------------------------------------------------
// Static utility
// -----------------------------------------------------------------------------

static const uint32_t default_load_factor = 12;
static const uint32_t default_capacity = 16;
static const uint32_t default_initial_pair_count = 4;

/** Produces a hash code from the provided string.
 * Uses the djb2 algorithm.
 * @param str The string.
 * @return 32-bit signed hash code.
 */
static int32_t 
hash(const char* str) {
	int32_t hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
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

/** Sets the given buckets values to default values. 
 * @param bucket The bucket.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
static int
init_bucket(map_bucket* bucket) {
	bucket->capacity = default_initial_pair_count;
	bucket->active = 0;
	bucket->pairs = calloc(default_initial_pair_count, sizeof(map_pair));
	if (!bucket->pairs) {
		return MEMORY_REFUSED;
	}
	return SUCCESS;
}

/** Sets the given pair values to default values.
 * @param pair The pair.
 */
static void 
init_pair(map_pair* pair) {
	pair->key = NULL;
	pair->value = mtl_create();
	pair->hash = 0;
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
	pair->key = NULL;
	pair->value = mtl_create();
	pair->hash = 0;
}

/** Destroys a given bucket's contents. Assumes "bucket" is non-null.
* Destroys the "pairs" within the bucket and sets their properties to NULL, 
* and sets the "active" to "0".
* @param bucket The bucket to destroy. Assumes it is valid.
*/
static void 
destroy_bucket(map_bucket* bucket) {
	free(bucket->pairs);
	bucket->pairs = NULL;
	bucket->capacity = 0;
	bucket->active = 0;
}

/** Swap the given pair with the last pair in the list, and delete it.
* Assumes "index" is within bounds.
* Assumes there is a non-zero number of active pairs.
* Call "destroy_pair" to free memory before calling this function or memory
* will be leaked.
* @param bucket The bucket from which the pair exists.
* @param index The index of the pair in the pairs array.
*/
static void
rm_pair_from_bucket(map_bucket* bucket, uint32_t index) {
	// Case: "pair" is not the "last pair"
	if (index < bucket->active - 1) {
		bucket->pairs[index] = bucket->pairs[bucket->active - 1];
	}
	bucket->pairs[bucket->active - 1] = (map_pair) { .key = NULL, .value = 
		mtl_create(), .hash = 0 };
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
static void 
mv_pair_from_buckets(
map_bucket* from,
map_bucket* to, 
map_pair* pair, 
uint32_t pairIndexInFrom) {
	rm_pair_from_bucket(from, pairIndexInFrom);
	add_pair_to_bucket(to, pair);
}

/** Doubles map capacity. 
 * Assumes the bucket list has already been allocated.
 * @param map The map.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int
map_dbl_capacity(mat_map* map) {
	map_bucket* temp = realloc(map->buckets, (map->capacity << 1) 
		* sizeof(map_bucket));
	if (!temp) {
		return MEMORY_REFUSED;
	}
	map->buckets = temp;
	map->capacity = map->capacity << 1;
	map->load_factor = map->load_factor << 1;
	return SUCCESS;
}

/** Doubles the pairs array within a bucket.
 * Assumes the pairs array has already been allocated.
 * @param bucket The bucket.
 * @return [SUCCESS, MEMORY_REFUSED]
 */
int 
bucket_dbl_capacity(map_bucket* bucket) {
	map_pair* temp = realloc(bucket->pairs, (bucket->capacity << 1) 
		* sizeof(map_pair));
	if (!temp) {
		return MEMORY_REFUSED;
	}
	bucket->pairs = temp;
	bucket->capacity = bucket->capacity << 1;
	return SUCCESS;
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
	int code;
	// Determine if we need to increase individual 
	// bucket capacity. Count actives based on hash codes
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		if (!bucket || bucket->active == 0) {
			continue;
		}
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
		}
	}
	// Increase individual bucket capacity if # of actives exceeds capacity
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		if (bucket->active > bucket->capacity) {
			if ((code = bucket_dbl_capacity(bucket)) != SUCCESS) {
				return MEMORY_REFUSED;
			}
			// Set newly created pairs to empty values
			for (uint32_t j = bucket->active; j < bucket->capacity; j++) {
				init_pair(&bucket->pairs[j]);
			}				
		}
	}	
	// Move the pairs based on hash code (rehashing)
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		for (uint32_t j = 0; j < bucket->active; j++) {
			map_pair* pair = &bucket->pairs[j];
			// Assumes a rehash_bucket is never null (should be if allocation
			// did not fail), and is atleast zero-initialized			
			uint32_t index = hash(pair->key) % map->capacity;
			map_bucket* rehash_bucket = &map->buckets[index];
			if (bucket != rehash_bucket) {
				mv_pair_from_buckets(bucket, rehash_bucket, pair, j);
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

int 
map_create(mat_map* map) {
	int code;
	map->active = 0;
	map->load_factor = default_load_factor;
	map->capacity = default_capacity;
	map->buckets = calloc(map->capacity, sizeof(map_bucket));
	if (!map->buckets) {
		free(map);
		return MEMORY_REFUSED;
	}
	// Set the properties for each bucket
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		if ((code = init_bucket(bucket)) != SUCCESS) {
			free(map);
			return code;
		}
		// Init pairs
		for (uint32_t i = 0; i < default_initial_pair_count; i++) {
			init_pair(&bucket->pairs[i]);
		}
	}
	return SUCCESS;
}

void 
map_destroy(mat_map* map) {
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		for (size_t i = 0; i < bucket->active; i++) {
			map_pair* pair = &bucket->pairs[i];
			destroy_pair(pair);
		}
		destroy_bucket(bucket);
		free(bucket->pairs);
		bucket->pairs = NULL;
	}
	free(map->buckets);
	map->buckets = NULL;
	map->capacity = 0;
	map->active = 0;
	map->load_factor = 0;
}

int 
map_copy(mat_map* restrict map1, const mat_map* restrict map2) {
	int code;
	map_destroy(map1); // Clear the first map
	map1->active = 0;
	map1->load_factor = map2->load_factor;
	map1->capacity = map2->capacity;
	map1->buckets = calloc(map1->capacity, sizeof(map_bucket));
	if (!map1->buckets) {
		free(map1);
		return MEMORY_REFUSED;
	}
	// Set the properties for each bucket
	for (uint32_t i = 0; i < map1->capacity; i++) {
		map_bucket* bucket = &map1->buckets[i];
		map_bucket* bucket2 = &map2->buckets[i];
		if ((code = init_bucket(bucket)) != SUCCESS) {
			free(map1);
			return code;
		}
		// Init pairs
		for (uint32_t i = 0; i < bucket2->capacity; i++) {
			init_pair(&bucket->pairs[i]);
		}
	}
	for (uint32_t i = 0; i < map2->capacity; i++) {
		map_bucket* bucket = &map2->buckets[i];
		for (uint32_t j = 0; j < bucket->active; j++) {
			map_pair* pair = &bucket->pairs[j];
			// Insert to the first map. Will copy pair properties
			if ((code = map_insert(map1, pair->key, pair->value)) != SUCCESS) {
				return MEMORY_REFUSED;
			}
		}
	}
	return SUCCESS;
}

int 
map_insert(mat_map* map, const char* key, mtl_t value) {
	int code;
	// Create the [key, value] pair
	map_pair pair;
	pair.key = malloc(strlen(key) + 1);
	if (!pair.key) {
		return MEMORY_REFUSED;
	}
	pair.value = value;
	strncpy((char*)pair.key, key, strlen(key) + 1);
	// Determine if we need to rehash the map early on
	if (map->active >= map->load_factor) { 
		map_rehash(map);
	}
	// Map capacity is` 0. User code will do this to save memory.
	// Set it to 1 and increase from there on
	if (map->capacity == 0) {
		map->capacity = 1;
		map->buckets = calloc(1, sizeof(map_bucket));
		if (!map->buckets) {
			free((void*)pair.key);
			return MEMORY_REFUSED;
		}
		// Set the properties for the new bucket
		map_bucket* bucket = &map->buckets[0];
		if ((code = init_bucket(bucket)) != SUCCESS) {
			free(map->buckets);
			free((void*)pair.key);
			return MEMORY_REFUSED;
		}
		// Init pairs
		for (uint32_t j = 0; j < bucket->capacity; j++) {
			init_pair(&bucket->pairs[j]);
		}
	}
	
	map_bucket* bucket = get_bucket(map, key);
	// Bucket already exists with this key's hash
	if (bucket) {
		// Bucket exists, and more than 0 pairs exist in this bucket.
		if (bucket->active == 0) {
			map->active++;
		}

		if (bucket->active + 1 > bucket->capacity) {
			if ((code = bucket_dbl_capacity(bucket)) != SUCCESS) {
				free(map->buckets);
				free((void*)pair.key);
				return MEMORY_REFUSED;
			}
		}
		bucket->active++;
		bucket->pairs[bucket->active - 1] = pair;
	}
	
	return SUCCESS;
}

void  
map_erase(mat_map* map, const char* key) {
	map_bucket* bucket = get_bucket(map, key);
	if (!bucket) {
		return;
	}
	for (uint32_t i = 0; i < bucket->active; i++) {
		map_pair* pair = &bucket->pairs[i];
		if (strequ(key, pair->key)) {
			// 1. Free memory
			destroy_pair(pair);
			// 2. Remove from bucket.
			rm_pair_from_bucket(bucket, i);
			bucket->active--;
			if (bucket->active == 0) {
				map->active--;
			}
			return;
		}
	}
}

int 
map_at(mat_map* map, const char* key, mtl_t** out) {
	map_bucket* bucket = get_bucket(map, key);
	if (!bucket) {
		*out = NULL;
		return NOT_FOUND;
	}
	for (uint32_t i = 0; i < bucket->active; i++) {
		map_pair* pair = &bucket->pairs[i];
		if (strequ(key, pair->key)) {
			*out = &pair->value;
			return SUCCESS;
		}
	}
	*out = NULL;
	return NOT_FOUND;
}

int 
map_empty(mat_map* map) {
	return map->active == 0;
}

size_t 
map_size(mat_map* map) {
	size_t count = 0;
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		count += bucket->active;
	}
	return count;
}

int 
map_clear(mat_map* map) {
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		for (uint32_t j = 0; j < bucket->active; j++) {
			map_pair* pair = &bucket->pairs[j];
			// 1. Free memory
			destroy_pair(pair);
			// 2. Remove from bucket.
			rm_pair_from_bucket(bucket, i);
			// Don't free the pairs array.
		}
		bucket->active = 0;
	}
	// Don't free the buckets array.
	map->active = 0;
	return SUCCESS;
}

keys_list_t
map_keys(mat_map* map) {
	keys_list_t list = (keys_list_t) { .keys = NULL, .used = 0 };
	list.used = map_size(map);
	list.keys = calloc(list.used, sizeof(const char*));
	uint32_t k = 0;
	for (uint32_t i = 0; i < map->capacity; i++) {
		map_bucket* bucket = &map->buckets[i];
		for (uint32_t j = 0; j < bucket->active; j++) {
			map_pair* pair = &bucket->pairs[j];
			if (pair->key != NULL) {
				list.keys[k++] = pair->key;
			}
		}
	}
	return list;
}

void
keys_list_destroy(keys_list_t* list) {
	free(list->keys);
	list->keys = NULL;
	list->used = 0;	
}