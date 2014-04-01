/**
 * Binary search tree
 */

#include "fkw_rbtree.h"
#include "fkw_rbtree_dict.h"

#include <stdio.h>
#include <unistd.h>

static fkw_rbtree_node_t *
fkw_rbtree_insert_word_value(fkw_rbtree_node_t *temp, fkw_rbtree_node_t *node,
    fkw_rbtree_node_t *sentinel)
{
	fkw_rbtree_node_t  **p;
	fkw_rbtree_node_value_t *node_value, *value_list;

	int cmp;

	for ( ;; ) {

		cmp = node->key - temp->key;

		if (cmp == 0) {
			p = &node;
		} else if (cmp > 0){
			p = &temp->right;
		} else {
			p = &temp->left;
		}

        if (*p == sentinel || cmp == 0) {
            break;
        }

        temp = *p;
    }
	
	// empty node
	if (*p == sentinel){
    	*p = node;
    	node->left = sentinel;
	    node->right = sentinel;
    	node->parent = temp;
		fkw_rbt_red(node);
		return node;
	}

	// alreade exitst
	value_list = (fkw_rbtree_node_value_t *) temp->data;
	node_value = (fkw_rbtree_node_value_t *) node->data;

	node_value->next = value_list;
	temp->data = node_value;

	return NULL;
}

static fkw_rbtree_hash_t *
tree_hash_init(fkw_dict_array_t *dict_array, int hash_size)
{
	int i;
	fkw_rbtree_t **trees;
	fkw_rbtree_hash_t *tree_hash;
	fkw_rbtree_node_t *rbtree_sentinel;

	tree_hash = (fkw_rbtree_hash_t *) mc_calloc(&(dict_array->mc), sizeof(fkw_rbtree_hash_t) );
	tree_hash->hash_size = hash_size;

	trees = (fkw_rbtree_t **) mc_calloc(&(dict_array->mc), sizeof(fkw_rbtree_t *) * hash_size);

	rbtree_sentinel = (fkw_rbtree_node_t *) mc_calloc(&(dict_array->mc), sizeof(fkw_rbtree_node_t) );

	for (i = 0; i < hash_size; i++){
		trees[i] = (fkw_rbtree_t *) mc_calloc(&(dict_array->mc), sizeof(fkw_rbtree_t) );
		fkw_rbtree_init(trees[i], rbtree_sentinel, fkw_rbtree_insert_word_value);
	}
	tree_hash->trees = trees;

	return tree_hash;
}

static fkw_word_dict_t *
word_dict_init(fkw_dict_array_t *dict_array, int word_size, int hash_size)
{
	fkw_word_dict_t *word_dict;

	word_dict = (fkw_word_dict_t *) mc_calloc(&(dict_array->mc), sizeof(fkw_word_dict_t));

	word_dict->word_size = word_size;
	word_dict->tree_hash = tree_hash_init(dict_array, hash_size);

	return word_dict;
}

static fkw_dict_array_t*
fkw_dict_init(int hash_size)
{
	int i;

	fkw_dict_array_t *dict_array;

	mc_collector_t *mc;

	mc = NULL;
	dict_array = (fkw_dict_array_t *) mc_calloc(&mc, sizeof(fkw_dict_array_t));

	dict_array->mc = mc;
	dict_array->dict_array = (fkw_word_dict_t **) mc_calloc(
			&(dict_array->mc), sizeof(fkw_word_dict_t *) * MAX_WORD_SIZE);

	for (i = 0; i < MAX_WORD_SIZE; i++){
		dict_array->dict_array[i] = word_dict_init(dict_array, i+1, hash_size);
	}

	return dict_array;
}

static fkw_rbtree_node_value_t*
create_rbtree_value_node(fkw_dict_array_t *dict_array, fkw_word_node_t *word){
	u_char *word_value;
	fkw_rbtree_node_value_t *node_value;

	node_value = (fkw_rbtree_node_value_t*) mc_calloc(
			&(dict_array->mc), sizeof(fkw_rbtree_node_value_t));

	word_value = (u_char *) mc_calloc(&(dict_array->mc), word->len);
	memcpy(word_value, word->word, word->len);

	node_value->next  = NULL;
	node_value->value.len  = word->len;
	node_value->value.data = word_value;

	return node_value;
}

static fkw_rbtree_node_t *
gen_tree_node(fkw_dict_array_t *dict_array, fkw_word_node_t *word){
	fkw_rbtree_node_t *node;

	node = (fkw_rbtree_node_t*) mc_calloc(&(dict_array->mc), sizeof(fkw_rbtree_node_t) );

	node->key = word->hash_key;
	node->data = create_rbtree_value_node(dict_array, word);

	return node;
}

fkw_rbtree_t *
fkw_get_rbtree(fkw_dict_array_t *dict_array, fkw_word_node_t *word)
{
	int idx, tree_idx;
	fkw_word_dict_t *word_dict;
	fkw_rbtree_hash_t *tree_hash;

	idx = word->size < MAX_WORD_SIZE ? word->size : MAX_WORD_SIZE;

	word_dict = dict_array->dict_array[idx-1];

	tree_hash = word_dict->tree_hash;
	tree_idx  = word->hash_key % tree_hash->hash_size;

	return tree_hash->trees[tree_idx];
}

void 
fkw_dict_add_word(fkw_dict_array_t *dict_array, fkw_word_node_t *word)
{
	fkw_rbtree_t *rbtree;
	rbtree = fkw_get_rbtree(dict_array, word);

	fkw_rbtree_node_t *node;
	node = gen_tree_node(dict_array, word);

	fkw_rbtree_insert(rbtree, node);
}

fkw_rbtree_node_value_t *
fkw_dict_search_word(fkw_dict_array_t *dict_array, fkw_word_node_t *word)
{
	fkw_rbtree_t *tree;
	fkw_rbtree_node_t *node;

	fkw_rbtree_node_value_t *node_value;

	tree = fkw_get_rbtree(dict_array, word);

	node = fkw_rbtree_search(tree, word->hash_key);
	if (node == NULL){
		return NULL;
	}

	node_value = (fkw_rbtree_node_value_t *) node->data;
	
	while(node_value){
		if (node_value->value.len != word->len){
			node_value = node_value->next;
			continue;
		}

		if (memcmp(word->word, node_value->value.data, word->len) == 0){
			return node_value;
		} else {
			node_value = node_value->next;
			continue;
		}
	}

	return NULL;
}

fkw_rbtree_node_value_t * 
fkw_dict_search(fkw_dict_array_t* dict_array, const char *data, size_t len)
{
	fkw_word_node_t word;

	if (dict_array == NULL){
		return NULL;
	}

	word.word     = data;
	word.len      = len;
	word.size     = get_word_size(data, len);
	word.hash_key = create_hash_key(data, len);
	
	return fkw_dict_search_word(dict_array, &word);
}

fkw_dict_array_t*
fkw_dict_array_init(unsigned int hash_size)
{
	fkw_dict_array_t *dict_array;

	dict_array = fkw_dict_init(hash_size);

	return dict_array;
}

fkw_dict_array_t *
fkw_load_dict(fkw_dict_array_t *dict_array, const char *fpath)
{
	FILE *fp;
	int i, word_len;
	char buf[64];

	fkw_word_node_t word;

	if (dict_array == NULL){
		return NULL;
	}

	if ((fp = fopen(fpath, "r")) == NULL) {
		return NULL;
	}

	while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
		word_len = strlen(buf);

		for (i=0; i < word_len; i++){
			if (buf[i] == '\n' || buf[i] == '\r'){
				buf[i] = '\0';
				break;
			}
		}
		word_len = strlen(buf);

		i = 0;

		word.word     = buf;
		word.len      = word_len;
		word.size     = get_word_size(buf, word_len);
		word.hash_key = create_hash_key(buf, word_len);

		fkw_dict_add_word(dict_array, &word);

		// j = j + 1;
	}
	fclose(fp);

	return dict_array;
}

fkw_dict_array_t*
fkw_add_dict_word(fkw_dict_array_t* dict_array, const char *data, size_t len)
{
	fkw_word_node_t word;

	if (dict_array == NULL){
		return NULL;
	}

	word.word     = data;
	word.len      = len;
	word.size     = get_word_size(data, len);
	word.hash_key = create_hash_key(data, len);

	fkw_dict_add_word(dict_array, &word);

	return dict_array;
}

void fkw_free_dict(fkw_dict_array_t **dict_array)
{
	mc_destory( (*dict_array)->mc );
	*dict_array = NULL;
}
