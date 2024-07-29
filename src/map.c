#include "map.h"

#ifdef MAP_IMPL

// --------------------------------------------------------------
// Static utility
// --------------------------------------------------------------

static const float default_load_factor = 0.75f;
static const uint32_t default_capacity = 16;

/** Produces a hash code from the provided string.
 * Uses the djb2 algorithm.
 */
static uint32_t hash(const char* str) {
	uint32_t hash = 5381;
	int c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

/** Returns the pair if found, NULL if none exists. Will also return a NULL pair if the pair's value is NULL.
*/
static map_pair* get_pair(map_bucket* bucket, const char* key) {
	if (bucket->count == 0) {
		return NULL;
	}
	for (int i = 0; i < bucket->count; i++) {
		map_pair* pair = bucket->pairs[i];
		if (pair && pair->key && pair->value && strequ(key, pair->key)) {
			return pair;
		}
	}
	return NULL;
}

/** Returns the bucket if found, NULL if none exists.
*/
static map_bucket* get_bucket(MAP_NAME* map, const char* key) {
	if (!map) {
		return NULL;
	}
	uint32_t index = hash(key) % map->count;
	map_bucket* bucket = map->buckets[index];
	if (!bucket) {
		return NULL;
	} else {
		return bucket;
	}
}

/** Returns a new pair. NULL if it cannot be created
*/
static map_pair* create_pair(const char* key, const VALUE_TYPE* value) {
	// Keys cannot be NULL or empty. Values cannot be NULL.
	if (!key || strlen(key) == 0 || !value) {
		return NULL;
	}
	map_pair* pair = calloc(1, sizeof(map_pair));
	if (!pair) {
		return NULL;
	}
	pair->key = calloc(1, strlen(key) + 1);
	pair->value = calloc(1, sizeof(VALUE_TYPE));
	return pair;
}

/** Creates an empty bucket. NULL if it cannot be created. Does not create the pairs array.
*/
static map_bucket* create_bucket() {
	map_bucket* bucket = calloc(1, sizeof(map_bucket));
	bucket->count = 0;
	return bucket;
}

static void swap_pair(map_pair** restrict pair1, map_pair** restrict pair2) {
	map_pair** temp = pair1;
	*pair = *pair2;
	*pari2 = *map_pair;
}

static void map_rehash(MAP_NAME* map) {
	for (uint32_t i = 0; i < map->count; i++) {
		map_bucket** bucket = &map->buckets[i];
		if (!(*bucket)) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->count; j++) {
			map_pair** pair = &bucket->pairs[j];
			const char* key = pair->key;
			if (!(*pair)) {
				continue;
			}
			uint32_t index = hash(key) % map->count;
			map_bucket** new_bucket = &map->buckets[index];
			if (!(*new_bucket)) {
				map->buckets[index] = create_bucket();
			}
			if (*bucket != *new_bucket) {
				if (bucket->count) {
					
				}
				swap_pair(pair, &bucket->pairs[bucket->count - 1];
			}
		}
	}
}

// --------------------------------------------------------------
// Implementation
// --------------------------------------------------------------

int map_create(MAP_TYPE* map, uint32_t capacity, float load_factor) {
	if (capacity <= 0) {
		return NO_OP;
	}
	map = malloc(sizeof(MAP_TYPE));
	if (!map) {
		return OUT_OF_MEMORY;
	}
	map->load_factor = load_factor;
	map->count = capacity;
	map->buckets = calloc(map->count, sizeof(map_bucket));
	if (!map->buckets) {
		free(map);
		return OUT_OF_MEMORY;
	}
	return SUCCESS;
}

int map_create(MAP_TYPE* map) {
	return map_create(map, default_capacity, default_load_factor);
}

int map_destroy(MAP_TYPE* map) {
	if (!map) {
		return NULL_POINTER;
	}
	for (uint32_t i = 0; i < map->count; i++) {
		map_bucket* bucket = map->buckets[i];
		if (!bucket) {
			continue;
		}
		for (uint32_t j = 0; j < bucket->count; j++) {
			map_pair* pair = bucket->pair[j];
			if (!pair) {
				continue;
			}
			free(pair->key);
			free(pair->value);
		}
		free(bucket->pairs);
	}
	return SUCCESS;
}

int map_copy(MAP_NAME* restrict map1, const MAP_NAME* restrict map2) {
	if (!map1 || !map2) {
		return NULL_POINTER;
	}
	map_clear(map1);
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
				return code; // [OUT_OF_MEMORY, NULL_POINTER, EMPTY_STRING]
			}
		}
	}
	return SUCCESS;
}

int map_insert(MAP_NAME* map, const char* key, const VALUE_TYPE* value) {
	if (!map || !value || !key) {
		return NULL_POINTER;
	}
	if (strlen(key) == 0) {
		return EMPTY_STRING;
	}
	map_pair* pair = create_pair(key, value);
	if (!pair) {
		return OUT_OF_MEMORY;
	}
	if (map->count == 0) { // No buckets
		map->count += 1;
		map_bucket* bucket = create_bucket();
		if (!bucket) {
			return OUT_OF_MEMORY;
		}
		bucket->pairs[bucket->count - 1] = pair;
		map->buckets[map->count - 1] = bucket;
		
		return SUCCESS;
	}
	map_bucket* bucket = get_bucket(map, key);
	if (bucket) {
		if (bucket->count == 0) { // Bucket exists, no pairs exist. Create a single pair in this bucket
			bucket->count += 1;
			if (!pair) {
				return OUT_OF_MEMORY;
			}
			bucket->pairs[count - 1] = pair;
		} else { // Bucket exists, and more than 0 pairs exist in this bucket.
			
		}
	} else {
		// Create 32 new buckets
		map->buckets = realloc(
	}
	
	return SUCCESS;
}

int map_erase(MAP_NAME* map, const char* key) {
	if (!map) {
		return NULL_POINTER;
	}
	
}

int map_at(MAP_NAME* map, const char* key, VALUE_TYPE* out) {
	
}

int map_empty(MAP_NAME* map) {
	return map->count == 0;
}

size_t map_size(MAP_NAME* map) {
	size_t count = 0;
	for (uint32_t i = 0; i < map->count; i++) {
		map_bucket* bucket = map->buckets[i];
		if (!bucket) {
			continue;
		}
		count += bucket->count;
	}
	return count;
}

int map_clear(MAP_NAME* map) {
	if (!map) {
		return NULL_POINTER;
	}
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
				return code; // [OUT_OF_MEMORY, NULL_POINTER, EMPTY_STRING]
			}
		}
		bucket->count = 0;
		free(bucket);
	}
	map->count = 0;
	return SUCCESS;
}


#endif