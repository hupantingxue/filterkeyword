
/*
 * Copy from nginx, a little change
 */


#include <stdio.h>
#include "fkw_rbtree.h"

/*
 * The red-black tree code is based on the algorithm described in
 * the "Introduction to Algorithms" by Cormen, Leiserson and Rivest.
 */


static inline void fkw_rbtree_left_rotate(fkw_rbtree_node_t **root,
    fkw_rbtree_node_t *sentinel, fkw_rbtree_node_t *node);
static inline void fkw_rbtree_right_rotate(fkw_rbtree_node_t **root,
    fkw_rbtree_node_t *sentinel, fkw_rbtree_node_t *node);


void
fkw_rbtree_insert(fkw_rbtree_t *tree, fkw_rbtree_node_t *node)
{
    fkw_rbtree_node_t  **root, *temp, *sentinel;

    /* a binary tree insert */

    root = (fkw_rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;

    if (*root == sentinel) {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        fkw_rbt_black(node);
        *root = node;

        return;
    }

	// just add new node value
    if (tree->insert(*root, node, sentinel) == NULL){
		return;
	}

    /* re-balance tree */

    while (node != *root && fkw_rbt_is_red(node->parent)) {

        if (node->parent == node->parent->parent->left) {
            temp = node->parent->parent->right;

            if (fkw_rbt_is_red(temp)) {
                fkw_rbt_black(node->parent);
                fkw_rbt_black(temp);
                fkw_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    fkw_rbtree_left_rotate(root, sentinel, node);
                }

                fkw_rbt_black(node->parent);
                fkw_rbt_red(node->parent->parent);
                fkw_rbtree_right_rotate(root, sentinel, node->parent->parent);
            }

        } else {
            temp = node->parent->parent->left;

            if (fkw_rbt_is_red(temp)) {
                fkw_rbt_black(node->parent);
                fkw_rbt_black(temp);
                fkw_rbt_red(node->parent->parent);
                node = node->parent->parent;

            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    fkw_rbtree_right_rotate(root, sentinel, node);
                }

                fkw_rbt_black(node->parent);
                fkw_rbt_red(node->parent->parent);
                fkw_rbtree_left_rotate(root, sentinel, node->parent->parent);
            }
        }
    }

    fkw_rbt_black(*root);
}


fkw_rbtree_node_t *
fkw_rbtree_insert_value(fkw_rbtree_node_t *temp, fkw_rbtree_node_t *node,
    fkw_rbtree_node_t *sentinel)
{
    fkw_rbtree_node_t  **p;

    for ( ;; ) {

        p = (node->key < temp->key) ? &temp->left : &temp->right;

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    fkw_rbt_red(node);
	return node;
}


void
fkw_rbtree_delete(fkw_rbtree_t *tree, fkw_rbtree_node_t *node)
{
    unsigned int  red;
    fkw_rbtree_node_t  **root, *sentinel, *subst, *temp, *w;

    /* a binary tree delete */

    root = (fkw_rbtree_node_t **) &tree->root;
    sentinel = tree->sentinel;

    if (node->left == sentinel) {
        temp = node->right;
        subst = node;

    } else if (node->right == sentinel) {
        temp = node->left;
        subst = node;

    } else {
        subst = fkw_rbtree_min(node->right, sentinel);

        if (subst->left != sentinel) {
            temp = subst->left;
        } else {
            temp = subst->right;
        }
    }

    if (subst == *root) {
        *root = temp;
        fkw_rbt_black(temp);

        /* DEBUG stuff */
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        node->key = 0;

        return;
    }

    red = fkw_rbt_is_red(subst);

    if (subst == subst->parent->left) {
        subst->parent->left = temp;

    } else {
        subst->parent->right = temp;
    }

    if (subst == node) {

        temp->parent = subst->parent;

    } else {

        if (subst->parent == node) {
            temp->parent = subst;

        } else {
            temp->parent = subst->parent;
        }

        subst->left = node->left;
        subst->right = node->right;
        subst->parent = node->parent;
        fkw_rbt_copy_color(subst, node);

        if (node == *root) {
            *root = subst;

        } else {
            if (node == node->parent->left) {
                node->parent->left = subst;
            } else {
                node->parent->right = subst;
            }
        }

        if (subst->left != sentinel) {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel) {
            subst->right->parent = subst;
        }
    }

    /* DEBUG stuff */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;

    if (red) {
        return;
    }

    /* a delete fixup */

    while (temp != *root && fkw_rbt_is_black(temp)) {

        if (temp == temp->parent->left) {
            w = temp->parent->right;

            if (fkw_rbt_is_red(w)) {
                fkw_rbt_black(w);
                fkw_rbt_red(temp->parent);
                fkw_rbtree_left_rotate(root, sentinel, temp->parent);
                w = temp->parent->right;
            }

            if (fkw_rbt_is_black(w->left) && fkw_rbt_is_black(w->right)) {
                fkw_rbt_red(w);
                temp = temp->parent;

            } else {
                if (fkw_rbt_is_black(w->right)) {
                    fkw_rbt_black(w->left);
                    fkw_rbt_red(w);
                    fkw_rbtree_right_rotate(root, sentinel, w);
                    w = temp->parent->right;
                }

                fkw_rbt_copy_color(w, temp->parent);
                fkw_rbt_black(temp->parent);
                fkw_rbt_black(w->right);
                fkw_rbtree_left_rotate(root, sentinel, temp->parent);
                temp = *root;
            }

        } else {
            w = temp->parent->left;

            if (fkw_rbt_is_red(w)) {
                fkw_rbt_black(w);
                fkw_rbt_red(temp->parent);
                fkw_rbtree_right_rotate(root, sentinel, temp->parent);
                w = temp->parent->left;
            }

            if (fkw_rbt_is_black(w->left) && fkw_rbt_is_black(w->right)) {
                fkw_rbt_red(w);
                temp = temp->parent;

            } else {
                if (fkw_rbt_is_black(w->left)) {
                    fkw_rbt_black(w->right);
                    fkw_rbt_red(w);
                    fkw_rbtree_left_rotate(root, sentinel, w);
                    w = temp->parent->left;
                }

                fkw_rbt_copy_color(w, temp->parent);
                fkw_rbt_black(temp->parent);
                fkw_rbt_black(w->left);
                fkw_rbtree_right_rotate(root, sentinel, temp->parent);
                temp = *root;
            }
        }
    }

    fkw_rbt_black(temp);
}


static inline void
fkw_rbtree_left_rotate(fkw_rbtree_node_t **root, fkw_rbtree_node_t *sentinel,
    fkw_rbtree_node_t *node)
{
    fkw_rbtree_node_t  *temp;

    temp = node->right;
    node->right = temp->left;

    if (temp->left != sentinel) {
        temp->left->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->left) {
        node->parent->left = temp;

    } else {
        node->parent->right = temp;
    }

    temp->left = node;
    node->parent = temp;
}


static inline void
fkw_rbtree_right_rotate(fkw_rbtree_node_t **root, fkw_rbtree_node_t *sentinel,
    fkw_rbtree_node_t *node)
{
    fkw_rbtree_node_t  *temp;

    temp = node->left;
    node->left = temp->right;

    if (temp->right != sentinel) {
        temp->right->parent = node;
    }

    temp->parent = node->parent;

    if (node == *root) {
        *root = temp;

    } else if (node == node->parent->right) {
        node->parent->right = temp;

    } else {
        node->parent->left = temp;
    }

    temp->right = node;
    node->parent = temp;
}

fkw_rbtree_node_t *
fkw_rbtree_search(fkw_rbtree_t *tree, fkw_rbtree_key_t key)
{
	int cmp;

	fkw_rbtree_node_t *sentinel, *tmp_node;

	tmp_node = tree->root;
	sentinel = tree->sentinel;
    for ( ;; ) {
		if (tmp_node == sentinel){
			return NULL;
		}

		cmp = tmp_node->key - key;

		if (cmp == 0){
			return tmp_node;
		} else if (cmp > 0){
			tmp_node = tmp_node->left;
		} else {
			tmp_node = tmp_node->right;	
		}
	}
}
