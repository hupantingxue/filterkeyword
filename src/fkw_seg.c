/**
 * Word Segmentation
 */

#include "fkw_util.h"
#include "fkw_seg.h"

static fkw_atom_iter_t *
atom_iter_init(unsigned int size)
{
	int i;

	fkw_atom_t *atom;
	fkw_atom_iter_t *atom_iter;	

	mc_collector_t *mc;

	mc = NULL;

	atom_iter = (fkw_atom_iter_t *) mc_calloc(&mc, sizeof(fkw_atom_iter_t) );

	atom_iter->size = size;

	atom_iter->worker_list     = NULL;
	atom_iter->worker_list_end = NULL;
	atom_iter->free_list       = NULL;
	atom_iter->mc = mc;

	i = 0;	
	while (i < size) {
		atom = (fkw_atom_t *) mc_calloc(&(atom_iter->mc), sizeof(fkw_atom_t) );

		atom->value.data = NULL;
		atom->value.len  = 0;
		atom->next       = NULL;

		if (atom_iter->free_list == NULL){
			atom_iter->free_list = atom;
		} else {
			atom->next = atom_iter->free_list;
			atom_iter->free_list = atom;
		}

		i++;
	}

	return atom_iter;
}

static void 
atom_iterator_add(fkw_atom_iter_t *atom_iter, fkw_str_t *atom_value)
{
	fkw_atom_t *atom;
	if (atom_iter->free_list == NULL){
		atom_iter->free_list   = atom_iter->worker_list;
		atom_iter->worker_list = atom_iter->worker_list->next;

		atom_iter->free_list->next = NULL;
	}

	atom = atom_iter->free_list;
	atom_iter->free_list = atom_iter->free_list->next;

	atom->next = NULL;
	atom->value.data = atom_value->data;
	atom->value.len  = atom_value->len;

	if (atom_iter->worker_list == NULL) {
		atom_iter->worker_list     = atom;
		atom_iter->worker_list_end = atom;
	} else {
		atom_iter->worker_list_end->next = atom;
		atom_iter->worker_list_end = atom;
	}
}

static fkw_atom_t *
fkw_atom_iter_pop(fkw_atom_iter_t *atom_iter)
{
	fkw_atom_t *atom;

	if (atom_iter->worker_list == NULL){
		return NULL;
	}

	atom = atom_iter->worker_list;
	atom_iter->worker_list = atom->next;

	atom->next = atom_iter->free_list;
	atom_iter->free_list = atom;

	return atom;
}

static void
fkw_add_to_word_list(fkw_word_list_t *word_list, const char *data, size_t len)
{
	fkw_word_t *word;
	word = (fkw_word_t *) mc_calloc(&(word_list->mc), sizeof(fkw_word_t) );

	word->next = NULL;
	word->value.data = mc_calloc(&(word_list->mc), len + 1);
	memcpy(word->value.data, data, len);

	word->value.len = len;

	if (word_list->start == NULL){
		word_list->start = word;
		word_list->end   = word;
	} else {
		word_list->end->next = word;
		word_list->end = word;
	}

	word_list->num += 1;

}

void
fkw_do_seg(fkw_dict_array_t *dict_array, 
	fkw_atom_iter_t *atom_iter, fkw_word_list_t *word_list)
{
	unsigned int size, len;

	fkw_atom_t *atom;

	fkw_word_node_t word_node;

	atom = atom_iter->worker_list;
	if (!(atom && atom->next)){
		return;
	}

	size = 1;
	len  = atom->value.len; 
	while (atom->next){
		atom = atom->next;
		size += 1;
		len  += atom->value.len;

		word_node.word = (const char *)atom_iter->worker_list->value.data;
		word_node.size = size;
		word_node.len  = len;
		word_node.hash_key = create_hash_key(word_node.word, len);

		if (fkw_dict_search_word(dict_array, &word_node)){
			fkw_add_to_word_list(word_list, word_node.word, word_node.len);
		}
	}
}

fkw_word_list_t *
fkw_full_seg(fkw_str_t *text, fkw_dict_array_t *dict_array)
{
	u_char *start;

	fkw_str_t atom_value;

	fkw_atom_iter_t *atom_iter;

	fkw_word_list_t *word_list;

	mc_collector_t *mc;

	atom_iter = atom_iter_init(MAX_WORD_SIZE);

	mc = NULL;
	word_list = (fkw_word_list_t *) mc_calloc(&mc, sizeof(fkw_word_list_t));
	word_list->start = NULL;
	word_list->end   = NULL;
	word_list->num   = 0;
	word_list->mc    = mc;

	start = text->data;
	while ((start - text->data) < text->len) {
		atom_value.data = start;
		atom_value.len  = get_mblen((u_char)start[0]);
		
		atom_iterator_add(atom_iter, &atom_value);
		if (atom_iter->free_list == NULL) {
			fkw_do_seg(dict_array, atom_iter, word_list);
		}

		start = start + atom_value.len;
	}

	fkw_atom_iter_pop(atom_iter);
	while (atom_iter->worker_list && atom_iter->worker_list->next){
		fkw_do_seg(dict_array, atom_iter, word_list);
		fkw_atom_iter_pop(atom_iter);
	}

	// free atom iter's memory
	mc_destory(atom_iter->mc);
	return word_list;
}

void fkw_free_word_list(fkw_word_list_t **word_list)
{
	mc_destory((*word_list)->mc);
	*word_list = NULL;
}
