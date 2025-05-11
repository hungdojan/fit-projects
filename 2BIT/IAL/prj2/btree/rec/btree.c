/*
 * Binárny vyhľadávací strom — rekurzívna varianta
 *
 * S využitím dátových typov zo súboru btree.h a pripravených kostier funkcií
 * implementujte binárny vyhľadávací strom pomocou rekurzie.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
    if (tree != NULL) {
        *tree = NULL;
    }
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    if (tree == NULL)
        return false;
    
    // search in left subtree
    if (key < tree->key)
        return bst_search(tree->left, key, value);
    // search in right subtree
    else if (key > tree->key)
        return bst_search(tree->right, key, value);
    // node with identical key found
    else {
        *value = tree->value;
        return true;
    }
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    if (tree != NULL) {
        // found node is uninitialized
        if (*tree == NULL) {
            *tree = (bst_node_t *)malloc(sizeof(bst_node_t));
            if (*tree != NULL) {
                (*tree)->key   = key;
                (*tree)->value = value;
                (*tree)->left  = NULL;
                (*tree)->right = NULL;
            }
        } else if (key < (*tree)->key) {
            // search in left subtree
            bst_insert(&( (*tree)->left ), key, value);
        } else if (key > (*tree)->key) {
            // search in right subtree
            bst_insert(&( (*tree)->right ), key, value);
        } else {
            // node with identical key already exists
            (*tree)->value = value; 
        }
    }
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    // assuming that tree != NULL and target != NULL
    if (*tree != NULL) {
        if ((*tree)->right != NULL) {
            bst_replace_by_rightmost(target, &( (*tree)->right ));
        } else {
            // right-most found
            target->key   = (*tree)->key;
            target->value = (*tree)->value;
            // free alocated memory of right-most node
            bst_node_t *lchild = (*tree)->left;
            free(*tree);
            // replace right-most node with its left child if exists
            // otherwise set NULL
            *tree = lchild != NULL ? lchild : NULL;
        }
    }
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte rekurzívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
    if (tree != NULL && *tree != NULL) {
        // search
        if (key < (*tree)->key) {
            bst_delete(&( (*tree)->left ), key);
        } else if (key > (*tree)->key) {
            bst_delete(&( (*tree)->right ), key);
        } else {
            // found the node
            bst_node_t *left = (*tree)->left;
            bst_node_t *right = (*tree)->right;
            // a leaf
            if (left == NULL && right == NULL) {
                free(*tree);
                *tree = NULL;
            } else if (left != NULL && right != NULL) {
                // both children are present
                bst_replace_by_rightmost(*tree, &( (*tree)->left ));
            } else if (left != NULL) {
                // only left child is present
                free(*tree);
                *tree = left;
            } else {
                // only right child is present
                free(*tree);
                *tree = right;
            }
        }
    }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
    if (tree != NULL && *tree != NULL) {
        // delete children first then parent node
        bst_dispose(&( (*tree)->left ));
        bst_dispose(&( (*tree)->right ));
        free(*tree);
        *tree = NULL;
    }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
    if (tree != NULL) {
        bst_print_node(tree);
        bst_preorder(tree->left);
        bst_preorder(tree->right);
    }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
    if (tree != NULL) {
        bst_inorder(tree->left);
        bst_print_node(tree);
        bst_inorder(tree->right);
    }
}
/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte rekurzívne bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
    if (tree != NULL) {
        bst_postorder(tree->left);
        bst_postorder(tree->right);
        bst_print_node(tree);
    }
}
