
/**
 *
 * Axxel - Acl memory caching
 *
 * Copyright (c) 2013 Axxel Team
 *
 * Use and distribution licensed under the MIT license.
 * See the LICENSE file for full text.
 *
 * Authors: Andres Gutierrez <andres@axxeld.com>
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"
#include "primes.h"
#include "memory.h"

static char *pstrndup(const char *str, unsigned int str_length)
{
	char *b;
	b = pmalloc(str_length + 1);
	if (!b) {
		return NULL;
	}
	memcpy(b, str, str_length);
	b[str_length] = '\0';
	return b;
}

/*
 * Dan Bernstein created this algorithm and posted it in a newsgroup. It is known by many as
 * the Chris Torek hash because Chris went a long way toward popularizing it. Since then it has
 * been used successfully by many, but despite that the algorithm itself is not very sound when
 * it comes to avalanche and permutation of the internal state. It has proven very good for small
 * character keys, where it can outperform algorithms that result in a more random distribution
 *
 * Bernstein's hash should be used with caution. It performs very well in practice, for no apparently
 * known reasons (much like how the constant 33 does better than more logical constants for no apparent
 * reason), but in theory it is not up to snuff. Always test this function with sample data for
 * every application to ensure that it does not encounter a degenerate case and cause excessive
 * collisions.
 */
static unsigned long hashfunc1(const char *key, unsigned int key_length)
{
	size_t hash = 0;
	char *hb_key = (char *) key;

	while (key_length--){
		hash = (hash * 33) ^ *hb_key++;
	}

	return hash;
}

static unsigned long hashfunc(const char *arKey, unsigned int nKeyLength)
{
	register unsigned long hash = 5381;	

	nKeyLength++;

	/* variant with the hash unrolled eight times */
	for (; nKeyLength >= 8; nKeyLength -= 8) {
		hash = ((hash << 5) + hash) ^ *arKey++;
		hash = ((hash << 5) + hash) ^ *arKey++;
		hash = ((hash << 5) + hash) ^ *arKey++;
		hash = ((hash << 5) + hash) ^ *arKey++;
		hash = ((hash << 5) + hash) ^ *arKey++;
		hash = ((hash << 5) + hash) ^ *arKey++;
		hash = ((hash << 5) + hash) ^ *arKey++;
		hash = ((hash << 5) + hash) ^ *arKey++;
	}

	switch (nKeyLength) {
		case 7: hash = ((hash << 5) + hash) ^ *arKey++; /* fallthrough... */
		case 6: hash = ((hash << 5) + hash) ^ *arKey++; /* fallthrough... */
		case 5: hash = ((hash << 5) + hash) ^ *arKey++; /* fallthrough... */
		case 4: hash = ((hash << 5) + hash) ^ *arKey++; /* fallthrough... */
		case 3: hash = ((hash << 5) + hash) ^ *arKey++; /* fallthrough... */
		case 2: hash = ((hash << 5) + hash) ^ *arKey++; /* fallthrough... */
		case 1: hash = ((hash << 5) + hash) ^ *arKey++; break;
		case 0: break;
	}

	return hash;
}

unsigned long p_hash_table_hashfunc_three(const char *key1, unsigned int key_length1, const char *key2, unsigned int key_length2, const char *key3, unsigned int key_length3)
{
	unsigned long hash = 0;
	char *hb_key = (char *) key1;

	while (key_length1--){
		hash = (hash * 33) ^ *hb_key++;
	}
	hash = (hash * 33) ^ '!';

	hb_key = (char *) key2;
	while (key_length2--){
		hash = (hash * 33) ^ *hb_key++;
	}
	hash = (hash * 33) ^ '!';

	hb_key = (char *) key3;
	while (key_length3--){
		hash = (hash * 33) ^ *hb_key++;
	}

	return hash;
}

/**
 * Creates a HashTable initializing its internal structures
 */
p_hash_table *p_hash_table_create(size_t size)
{
	p_hash_table *hash_table;

	hash_table = pmalloc(sizeof(p_hash_table));
	if (!hash_table) {
		return NULL;
	}

	hash_table->nodes = pmalloc(sizeof(struct hash_node*) * size);
	if (!hash_table->nodes) {
		free(hash_table);
		return NULL;
	}

	memset(hash_table->nodes, '\0', sizeof(struct hash_node*) * size);

	hash_table->locked = 0;
	hash_table->size = size;
	hash_table->num_elements = 0;	
	hash_table->num_collisions = 0;
	hash_table->prime_position = 0;
	hash_table->num_resizes = 0;

	return hash_table;
}

/**
 * Inserts an element into the hash table resizing the number of buckets automatically
 */
int p_hash_table_insert_quick(p_hash_table *hash_table, const char *key, unsigned int key_length, void *data, size_t hash_key)
{
	struct p_hash_node *node;
	size_t hash, i, new_size;

	HT_LOCK(hash_table);

	if (!hash_key) {
		hash = hashfunc(key, key_length) % hash_table->size;
	} else {
		hash = hash_key % hash_table->size;
	}

	node = hash_table->nodes[hash];
	while (node) {
		if (node->key_length == key_length) {
			if (!memcmp(node->key, key, key_length + 1)) {
				if (node->data) {
					pfree(node->data);
				}
				node->data = data;
				HT_UNLOCK(hash_table);
				return 0;
			}
		}
		node = node->next;
	}

	node = pmalloc(sizeof(struct p_hash_node));
	if (!node) {
		HT_UNLOCK(hash_table);
		return 0;
	}

	if (key) {
		node->key = pstrndup(key, key_length);
		if (!node->key) {
			free(node);
			HT_UNLOCK(hash_table);
			return 0;
		}
	}

	if (hash_table->nodes[hash]) {
		hash_table->num_collisions++;	
	}

	node->key_length = key_length;
	node->data = data;
	node->hash = hash;	
	node->next = hash_table->nodes[hash];	
	hash_table->nodes[hash] = node;
	hash_table->num_elements++;

	HT_UNLOCK(hash_table);

	/**
	 * We tolerate only the 10% of the elements in collisioned slots, 
	 * Hash tables are resized only to prime numbers, this improve
	 * the distributions of keys in the buckets
	 */
	if (hash_table->num_collisions > (hash_table->size * 0.10)) {
		if (hash_table->prime_position == 0) {
			for (i = 0; i < 1000; i++) {
				if (primes[i] > hash_table->size) {
					hash_table->prime_position = i + 1;
					new_size = primes[i + 1];
					break;
				}
			}
		} else {
			if (hash_table->prime_position > 1000) {
				new_size = hash_table->size + hash_table->size * 0.25;
			} else {
				new_size = primes[hash_table->prime_position + 3];
				hash_table->prime_position += 3;
			}
		}
		fprintf(stderr, "%d] Collisions: %d Num elements: %d, resizing to %d...\n", (int)hash_table->num_resizes, (int) hash_table->num_collisions, (int) hash_table->num_elements, (int)(new_size));
		p_hash_table_resize(hash_table, new_size);
	}	

	return 0;
}

/**
 * Inserts an element into the hash table resizing the number of buckets automatically
 */
int p_hash_table_insert(p_hash_table *hash_table, const char *key, unsigned int key_length, void *data)
{
	return p_hash_table_insert_quick(hash_table, key, key_length, data, 0);
}

/**
 * Inserts an element in the hash table without resizing
 */
static int p_hash_table_insert_noresize(p_hash_table *hash_table, char *key, unsigned int key_length, void *data)
{
	struct p_hash_node *node;
	size_t hash;

	hash = hashfunc(key, key_length) % hash_table->size;

	node = hash_table->nodes[hash];
	while (node) {
		if (node->key_length == key_length) {
			if (!memcmp(node->key, key, key_length + 1)) {
				node->data = data;
				return 0;
			}
		}
		node = node->next;
	}

	node = malloc(sizeof(struct p_hash_node));
	if (!node) {
		return 0;
	}

	if (hash_table->nodes[hash]) {
		hash_table->num_collisions++;
	}

	node->key = key;
	node->hash = hash;
	node->key_length = key_length;
	node->data = data;	
	node->next = hash_table->nodes[hash];	
	hash_table->nodes[hash] = node;
	hash_table->num_elements++;

	return 1;
}

void *p_hash_table_get(p_hash_table *hash_table, const char *key, unsigned int key_length)
{
	struct p_hash_node *node;
	size_t hash;	

	HT_IS_LOCKED(hash_table);

	hash = hashfunc(key, key_length) % hash_table->size;

	node = hash_table->nodes[hash];
	while (node) {
		if (node->key_length == key_length) {
			if (!memcmp(node->key, key, key_length + 1)) {				
				return node->data;
			}
		}
		node = node->next;
	}

	return NULL;
}

int p_hash_table_exists(p_hash_table *hash_table, const char *key, unsigned int key_length)
{
	struct p_hash_node *node;
	size_t hash;

	HT_IS_LOCKED(hash_table);

	hash = hashfunc(key, key_length) % hash_table->size;

	node = hash_table->nodes[hash];
	while (node) {
		if (node->key_length == key_length) {
			if (!memcmp(node->key, key, key_length + 1)) {				
				return 1;
			}
		}
		node = node->next;
	}

	return 0;
}

void p_hash_table_destroy(p_hash_table *hash_table)
{
	size_t n;
	struct p_hash_node *node, *oldnode;

	HT_LOCK(hash_table);

	for (n = 0; n < hash_table->size; ++n) {
		node = hash_table->nodes[n];
		while (node) {
			//fprintf(stderr, "Freeing bucket=%d\n", n);
			free(node->data);
			free(node->key);
			oldnode = node;
			node = node->next;
			free(oldnode);
		}
	}
	free(hash_table->nodes);
	free(hash_table);

	HT_UNLOCK(hash_table);
}

/**
 * Removes an element from the hash list
 */
int p_hash_table_remove(p_hash_table *hash_table, const char *key, unsigned int key_length)
{
	struct p_hash_node *node, *prevnode = NULL;
	size_t hash;

	HT_LOCK(hash_table);

	hash = hashfunc(key, key_length) % hash_table->size;

	node = hash_table->nodes[hash];
	while (node) {
		if (node->key_length == key_length) {
			if (!memcmp(node->key, key, key_length + 1)) {
				free(node->key);
				if (prevnode) {
					prevnode->next = node->next;
				} else {
					hash_table->nodes[hash] = node->next;
				}
				free(node);
				hash_table->num_elements--;
				HT_UNLOCK(hash_table);
				return 1;
			}
		}
		prevnode = node;
		node = node->next;
	}

	HT_UNLOCK(hash_table);

	return 0;
}

/**
 * Removes an element resizing
 */
int p_hash_table_remove_resize(p_hash_table *hash_table, const char *key, unsigned int key_length, size_t hash)
{
	struct p_hash_node *node, *prevnode = NULL;	

	node = hash_table->nodes[hash];
	while (node) {
		if (node->key_length == key_length) {
			if (!memcmp(node->key, key, key_length + 1)) {
				if (prevnode) {
					prevnode->next = node->next;
				} else {
					hash_table->nodes[hash] = node->next;
				}
				free(node);				
				return 1;
			}
		}
		prevnode = node;
		node = node->next;
	}

	return 0;
}

int p_hash_table_resize(p_hash_table *hash_table, size_t size)
{
	p_hash_table resized_hash;
	size_t n, hash;
	struct p_hash_node *node,*next;

	if (size != hash_table->size) {

		HT_LOCK(hash_table);

		resized_hash.size = size;		
		resized_hash.num_elements = 0;
		resized_hash.num_collisions = 0;
		resized_hash.num_resizes = hash_table->num_resizes;

		resized_hash.nodes = calloc(size, sizeof(struct p_hash_node*));
		if (!resized_hash.nodes) {
			HT_UNLOCK(hash_table);
			return -1;
		}

		for (n = 0; n < hash_table->size; ++n) {
			for (node = hash_table->nodes[n]; node; node = next) {
				next = node->next;
				hash = node->hash;
				p_hash_table_insert_noresize(&resized_hash, node->key, node->key_length, node->data);
				p_hash_table_remove_resize(hash_table, node->key, node->key_length, hash);
			}
		}

		free(hash_table->nodes);
		hash_table->num_elements = resized_hash.num_elements;
		hash_table->num_collisions = resized_hash.num_collisions;
		hash_table->size = resized_hash.size;
		hash_table->nodes = resized_hash.nodes;
		hash_table->num_resizes = resized_hash.num_resizes;
		hash_table->num_resizes++;

		HT_UNLOCK(hash_table);
	}

	return 0;
}

/**
 * Returns how many buckets are filled respect the total number of buckets
 */
double p_hash_table_empty_ratio(p_hash_table *hash_table)
{
	int n, number_buckets = 0;

	for (n = 0; n < hash_table->size; ++n) {
		if (hash_table->nodes[n]) {
			number_buckets++;
		}
	}

	//fprintf(stderr, "%d\n", number_buckets);
	//fprintf(stderr, "%d\n", hash_table->num_elements);

	return number_buckets / hash_table->num_elements;
}
