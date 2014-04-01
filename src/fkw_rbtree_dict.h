/**
 * Binary search tree
 */

#ifndef FKW_RBTREE_DICT_H_INCLUDED_
#define FKW_RBTREE_DICT_H_INCLUDED_

#include <string.h>

#include "mem_collector.h"
#include "fkw_util.h"
#include "fkw_rbtree.h"

#include <stdlib.h>

#define MAX_WORD_SIZE 5

typedef struct fkw_rbtree_node_value_s fkw_rbtree_node_value_t;

struct fkw_rbtree_node_value_s {
	fkw_str_t value;
	fkw_rbtree_node_value_t *next;
};

typedef struct {
	const char *word;
	size_t len;
	unsigned int size;
	unsigned int hash_key;
} fkw_word_node_t;

typedef struct {
	unsigned int hash_size;
	fkw_rbtree_t **trees;
} fkw_rbtree_hash_t;

typedef struct {
	unsigned int word_size;
	fkw_rbtree_hash_t *tree_hash;

} fkw_word_dict_t;

typedef struct {
	fkw_word_dict_t **dict_array;
	mc_collector_t *mc;
} fkw_dict_array_t;

fkw_dict_array_t* fkw_dict_array_init(unsigned int hash_size);

fkw_dict_array_t* fkw_load_dict(fkw_dict_array_t *dict_array, const char *fpath);

fkw_dict_array_t* fkw_add_dict_word(
		fkw_dict_array_t* dict_array, const char *data, size_t len);

fkw_rbtree_node_value_t * fkw_dict_search(
		fkw_dict_array_t* dict_array, const char *data, size_t len);

fkw_rbtree_node_value_t * fkw_dict_search_word(
		fkw_dict_array_t* dict_array, fkw_word_node_t *word);


void fkw_free_dict(fkw_dict_array_t **dict_array);

#endif
