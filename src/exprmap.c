#include "exprmap.h"
#include <stdio.h>

typedef struct Index {
	char* key;
	Expr* value;
} Index;

typedef struct Bucket {
	unsigned int count;
	Index* pairs;
} Bucket;

struct ExprMap {
	unsigned int count;
	Bucket* buckets;
};

static Index* getIndex(Bucket* bucket, char* key);
static unsigned long hash(const char* str);

ExprMap* newExprMap(size_t capacity) {
	ExprMap* map;
	
	map = malloc(sizeof(ExprMap));
	if (map == NULL) {
		return NULL;
	}
	map->count = capacity;
	map->buckets = malloc(capacity * sizeof(Bucket));
	if (map->buckets == NULL) {
		free(map);
		return NULL;
	} else {
		memset(map->buckets, 0, capacity * sizeof(Bucket));
		return map;
	}
}

void deleteExprMap(ExprMap* map) {
	size_t m;
	size_t mapSize;
	Bucket* bucket;
	Index* pair;

	// Validate the input
	if (map == NULL) {
		fprintf(stderr, "Cannot delete null map.");
		exit(1);
	}
	mapSize = map->count;
	bucket = map->buckets;
	for (size_t i = 0; i < mapSize; i++) {
		m = bucket->count;
		pair = bucket->pairs;
		for (size_t j = 0; j < m; j++) {
			free(pair->key);
			free(pair->value);
			pair++;
		}
		free(bucket->pairs);
		bucket++;
	}
	free(map->buckets);
	free(map);
}

bool getExpr(ExprMap* map, char* key, Expr* outputBuffer)
{
	size_t index;
	Bucket *bucket;
	Index *pair;

	if (map == NULL) {
		fprintf(stderr, "Cannot get key from null map.\n");
		return false;
	}
	if (key == NULL) {
		fprintf(stderr, "Cannot get null key from map.\n");
		return false;
	}
	index = hash(key) % map->count;
	bucket = &(map->buckets[index]);
	pair = getIndex(bucket, key);
	if (pair == NULL) {
		return false;
	}
	memcpy(outputBuffer, pair->value, sizeof(ExprMap));
	return true;
}

bool exprExists(ExprMap* map, char* key)
{
	size_t index;
	Bucket* bucket;
	Index* pair;

	if (map == NULL) {
		return 0;
	}
	if (key == NULL) {
		return 0;
	}
	index = hash(key) % map->count;
	bucket = &(map->buckets[index]);
	pair = getIndex(bucket, key);
	if (pair == NULL) {
		return false;
	}
	return true;
}

bool putExpr(ExprMap* map, char* key, Expr* value)
{
	size_t keyLength;
	size_t index;
	Bucket* bucket;
	Index* tempPairs;
	Index* pair;
	char* newKey;
	Expr* newValue;

	// Validate the inputs.
	if (map == NULL || key == NULL || value == NULL) {
		return false;
	}
	
	keyLength = strlen(key);
	// Get a pointer to the bucket the key string hashes to
	index = hash(key) % map->count;
	bucket = &(map->buckets[index]);
	// Check if we can handle insertion by simply replacing
	// an existing value in a key-value pair in the bucket.
	if ((pair = getIndex(bucket, key)) != NULL) {
		/* The bucket contains a pair that matches the provided key,
		 * change the value for that pair to the new value.
		 *
		 * Copy the new value into the pair that matches the key */
		memcpy(pair->value, value, sizeof(Expr));
		return 1;
	}
	// Allocate space for a new key and value
	newKey = malloc((keyLength + 1) * sizeof(char));
	if (newKey == NULL) {
		return 0;
	}
	newValue = malloc(sizeof(Expr));
	if (newValue == NULL) {
		free(newKey);
		return 0;
	}
	// Create a key-value pair
	if (bucket->count == 0) {
		// The bucket is empty, lazily allocate space for a single
		// key-value pair.
		bucket->pairs = malloc(sizeof(Index));
		if (bucket->pairs == NULL) {
			free(newKey);
			free(newValue);
			return 0;
		}
		bucket->count = 1;
	} else {
		// The bucket wasn't empty but no pair existed that matches the provided
		// key, so create a new key-value pair.
		tempPairs = realloc(bucket->pairs, (bucket->count + 1) * sizeof(Index));
		if (tempPairs == NULL) {
			free(newKey);
			free(newValue);
			return 0;
		}
		bucket->pairs = tempPairs;
		bucket->count++;
	}
	// Get the last pair in the chain for the bucket
	pair = &(bucket->pairs[bucket->count - 1]);
	pair->key = newKey;
	pair->value = newValue;
	// Copy the key and its value into the key-value pair
	strcpy(pair->key, key);
	memcpy(pair->value, value, sizeof(Expr));
	return true;
}

/**
 * Returns a pair from the bucket that matches the provided key,
 * or null if no such pair exist.
 */
static Index* getIndex(Bucket* bucket, char* key)
{
	size_t n;
	Index* pair;

	n = bucket->count;
	if (n == 0) {
		return NULL;
	}
	pair = bucket->pairs;
	for (size_t i = 0; i < n; i++) {
		if (pair->key != NULL && pair->value != NULL) {
			if (strcmp(pair->key, key) == 0) {
				return pair;
			}
		}
		pair++;
	}
	return NULL;
}

/**
 * Returns a hash code for the provided string.
 */
static unsigned long hash(const char* str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}
