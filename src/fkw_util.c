/**
 *
 */
#include <stdlib.h>

#include "fkw_util.h"

static unsigned char mblen_table_utf8[] = 
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
};

void *
fkw_malloc(size_t size)
{
	void *p;
	p = malloc(size);
	return p;
}

void *
fkw_calloc(size_t size)
{
	void *p;
	p = fkw_malloc(size);

	if(p){
		memset(p, 0, size);
	}
	return p;
}

unsigned int 
create_hash_key(const char *value, unsigned int vlen)
{
	unsigned int i, key;

	key = 0;

	for (i = 0; i < vlen; i++) {
		key = (unsigned int) key * 31 + value[i];
	}

	return key;
}

unsigned int 
get_word_size(const char *word, unsigned int word_len)
{
	unsigned int i    = 0;
	unsigned int size = 0;

	while (i < word_len){
		i = i + mblen_table_utf8[ (u_char)word[i] ];
		size = size + 1;
	}

	return size;
}

unsigned int 
get_mblen(u_char ch)
{
	return mblen_table_utf8[ch];
}
