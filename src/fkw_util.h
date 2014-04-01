/**
 *
 */
#ifndef FKW_UTIL_H_INCLUDED_
#define FKW_UTIL_H_INCLUDED_

#include <string.h>
#include <stdlib.h>


typedef struct {
	size_t      len;
	u_char     *data;
} fkw_str_t;

typedef struct {
	size_t len;
	const u_char *data;
} fkw_const_str_t;


void * fkw_malloc(size_t size);

void * fkw_calloc(size_t size);

unsigned int create_hash_key(const char *value, unsigned int vlen);

unsigned int get_word_size(const char *word, unsigned int word_len);

unsigned int get_mblen(u_char ch);

#endif
