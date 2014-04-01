/**
 * Word Segmentation
 */

#ifndef FKW_SEG_H_INCLUDED_
#define FKW_SEG_H_INCLUDED_

#include "mem_collector.h"
#include "fkw_util.h"
#include "fkw_rbtree_dict.h"

typedef struct fkw_word_s fkw_word_t;
typedef struct fkw_atom_s fkw_atom_t;

struct fkw_word_s {
	fkw_str_t value; 
	fkw_word_t *next;
};

typedef struct {
	fkw_word_t *start;	
	fkw_word_t *end;

	unsigned int num;

	mc_collector_t *mc;

} fkw_word_list_t;

struct fkw_atom_s {
	fkw_str_t value;
	fkw_atom_t *next;
};

typedef struct {
	fkw_atom_t *worker_list;
	fkw_atom_t *worker_list_end;
	fkw_atom_t *free_list;

	unsigned int size;

	mc_collector_t *mc;
} fkw_atom_iter_t;


fkw_word_list_t * fkw_full_seg(fkw_str_t *text, fkw_dict_array_t *dict_array);

void fkw_free_word_list(fkw_word_list_t **word_list);

#endif

