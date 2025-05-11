/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
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
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    while (tree != NULL) {
        if (key < tree->key)
            tree = tree->left;
        else if (key > tree->key)
            tree = tree->right;
        else {
            *value = tree->value;
            return true;
        }
    }
    return false;
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
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    if (tree != NULL) {
        // search for the node in the tree
        bst_node_t *node = *tree;
        bool search = true;
        while (node != NULL && search) {
            if (key < node->key) {
                // if its not a leaf from left side
                if (node->left != NULL)
                    node = node->left;
                else
                    search = false;
            } else if (key > node->key) {
                // if its not a leaf from right side
                if (node->right != NULL)
                    node = node->right;
                else
                    search = false;
            } else {
                // change value of existing node
                node->value = value;
                break;
            } // if (key < node->key)
        }

        // node not found
        if (node == NULL) {     // create root node
            // inicialization of root
            *tree = (bst_node_t *)malloc(sizeof(bst_node_t));
            if (*tree != NULL) {
                (*tree)->key   = key;
                (*tree)->value = value;
                (*tree)->right = (*tree)->left  = NULL;
            }
        } else if (!search) {   // create new node
            // inicialization of new node
            bst_node_t *new_node = (bst_node_t *)malloc(sizeof(bst_node_t));
            if (new_node != NULL) {
                new_node->key   = key;
                new_node->value = value;
                new_node->right = new_node->left = NULL;
            } // if (new_node != NULL)

            // assign as left or right child depending on the key value
            if (key > node->key)
                node->right = new_node;
            else
                node->left = new_node;
        } // if (node == NULL)
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
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    // under assumption that *tree != NULL
    if (tree != NULL) {
        bst_node_t *parent = NULL;
        bst_node_t *rchild = *tree;
        // seach for right-most node
        while (rchild->right != NULL) {
            parent = rchild;
            rchild = rchild->right;
        }
        // copy data
        target->key   = rchild->key;
        target->value = rchild->value;
        bst_node_t *left = rchild->left;
        free(rchild);

        // set right-most node
        // if right-most's left child exists, it will be assigned to parent's right
        if (parent == NULL) {   // right-most node is deleted node's left child
            *tree = left;
        } else {
            parent->right = left;
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
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
    if (tree != NULL) {
        bst_node_t *parent = NULL;
        bst_node_t *child  = *tree;
        // search for node with identical key
        while (child != NULL && child->key != key) {
            parent = child;
            if (key < child->key)
                child = child->left;
            else if (key > child->key)
                child = child->right;
        }
        // if node is found
        if (child != NULL) {
            if (child->right == NULL && child->left == NULL) {      // node is leaf
                free(child);
                // set NULL in parent's edge to deleted child
                if (parent != NULL) {
                    if (key < parent->key)  parent->left  = NULL;
                    else                    parent->right = NULL;
                }
            } else if (child->right != NULL && child->left != NULL) {   // node has both children
                bst_replace_by_rightmost( child, &(child->left) );
            } else if (child->left != NULL) {   // node has only left child
                parent->left = child->left;
                free(child);
            } else {    // node has only right child
                parent->right = child->right;
                free(child);
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
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
    if (tree != NULL && *tree != NULL) {
        stack_bst_t stack;
        stack_bst_init(&stack);
        stack_bst_push(&stack, *tree);
        while (!stack_bst_empty(&stack)) {
            bst_node_t *node = stack_bst_pop(&stack);
            if (node->right != NULL)    stack_bst_push(&stack, node->right);
            if (node->left  != NULL)    stack_bst_push(&stack, node->left );
            free(node);
        }
        // disposal done
        *tree = NULL;
    }
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
    while (tree != NULL) {
        // print the node and push its right child to the stack
        bst_print_node(tree);
        if (tree->right != NULL)
            stack_bst_push(to_visit, tree->right);
        tree = tree->left;
    }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
    if (tree != NULL) {
        // init stack
        stack_bst_t stack;
        stack_bst_init(&stack);
        stack_bst_push(&stack, tree);
        while (!stack_bst_empty(&stack)) {
            bst_node_t *node = stack_bst_pop(&stack);
            // seach for left most node and print all nodes along the way
            bst_leftmost_preorder(node, &stack);
        }
    }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
    // push all node on the way to left-most node
    while (tree != NULL) {
        stack_bst_push(to_visit, tree);
        tree = tree->left;
    }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
    if (tree != NULL) {
        // init stack
        stack_bst_t stack;
        stack_bst_init(&stack);
        bst_leftmost_inorder(tree, &stack);
        while (!stack_bst_empty(&stack)) {
            // get node on top and print it
            bst_node_t *node = stack_bst_pop(&stack);
            bst_print_node(node);
            // if it has right child, get left-most node of its subtree
            if (node->right != NULL)
                bst_leftmost_inorder(node->right, &stack);
        }
    }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
        stack_bool_t *first_visit) {
    while (tree != NULL) {
        stack_bst_push(to_visit, tree);
        stack_bool_push(first_visit, false);
        tree = tree->left;
    }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
    if (tree != NULL) {
        // init stacks
        stack_bool_t bool_stack;
        stack_bst_t  node_stack;
        stack_bst_init(&node_stack);
        stack_bool_init(&bool_stack);
        bst_leftmost_postorder(tree, &node_stack, &bool_stack);
        while (!stack_bst_empty(&node_stack)) {
            // if its first time arrival to this node
            // top bool_stack is change to true
            // and function leftmost_postorder is called on node's right subtree
            if (!stack_bool_top(&bool_stack)) {
                stack_bool_pop(&bool_stack);
                stack_bool_push(&bool_stack, true);
                bst_node_t *node = stack_bst_top(&node_stack);
                bst_leftmost_postorder(node->right, &node_stack, &bool_stack);
            } else {
                // when it's the second time arrival
                // print node and pop both stacks
                stack_bool_pop(&bool_stack);
                bst_print_node(stack_bst_pop(&node_stack));
            }
        }
    }
}
