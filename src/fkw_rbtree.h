/*
 * Copy from nginx, a little change
 */


#ifndef _FKW_RBTREE_H_INCLUDED_
#define _FKW_RBTREE_H_INCLUDED_

#include <stdlib.h>

typedef unsigned int  fkw_rbtree_key_t;

typedef struct fkw_rbtree_node_s  fkw_rbtree_node_t;

typedef struct fkw_rbtree_s  fkw_rbtree_t;

struct fkw_rbtree_node_s {
    fkw_rbtree_key_t       key;
    fkw_rbtree_node_t     *left;
    fkw_rbtree_node_t     *right;
    fkw_rbtree_node_t     *parent;
    u_char                 color;

	void *data;
};

 
typedef fkw_rbtree_node_t * (*fkw_rbtree_insert_pt) (fkw_rbtree_node_t*root, 
	fkw_rbtree_node_t*node, fkw_rbtree_node_t*sentinel);

struct fkw_rbtree_s {
    fkw_rbtree_node_t     *root;
    fkw_rbtree_node_t     *sentinel;
	fkw_rbtree_insert_pt   insert;
};

#define fkw_rbtree_init(tree, s, i)                                           \
    fkw_rbtree_sentinel_init(s);                                              \
    (tree)->root = s;                                                         \
    (tree)->sentinel = s;                                                     \
    (tree)->insert = i;

void fkw_rbtree_insert(fkw_rbtree_t *tree, fkw_rbtree_node_t *node);

void fkw_rbtree_delete(fkw_rbtree_t *tree, fkw_rbtree_node_t *node);

fkw_rbtree_node_t * fkw_rbtree_search(fkw_rbtree_t *tree, 
	fkw_rbtree_key_t key);

#define fkw_rbt_red(node)               ((node)->color = 1)
#define fkw_rbt_black(node)             ((node)->color = 0)
#define fkw_rbt_is_red(node)            ((node)->color)
#define fkw_rbt_is_black(node)          (!fkw_rbt_is_red(node))
#define fkw_rbt_copy_color(n1, n2)      (n1->color = n2->color)


/* a sentinel must be black */

#define fkw_rbtree_sentinel_init(node)  fkw_rbt_black(node)

static inline fkw_rbtree_node_t *
fkw_rbtree_min(fkw_rbtree_node_t *node, fkw_rbtree_node_t *sentinel)
{
    while (node->left != sentinel) {
        node = node->left;
    }

    return node;
}

#endif /* _NGX_RBTREE_H_INCLUDED_ */
