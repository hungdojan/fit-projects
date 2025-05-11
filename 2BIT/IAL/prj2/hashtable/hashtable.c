/*
 * Tabuľka s rozptýlenými položkami
 *
 * S využitím dátových typov zo súboru hashtable.h a pripravených kostier
 * funkcií implementujte tabuľku s rozptýlenými položkami s explicitne
 * zreťazenými synonymami.
 *
 * Pri implementácii uvažujte veľkosť tabuľky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptyľovacia funkcia ktorá pridelí zadanému kľúču index z intervalu
 * <0,HT_SIZE-1>. Ideálna rozptyľovacia funkcia by mala rozprestrieť kľúče
 * rovnomerne po všetkých indexoch. Zamyslite sa nad kvalitou zvolenej funkcie.
 */
int get_hash(char *key) {
    int result = 1;
    int length = strlen(key);
    for (int i = 0; i < length; i++) {
        result += key[i];
    }
    return (result % HT_SIZE);
}

/*
 * Inicializácia tabuľky — zavolá sa pred prvým použitím tabuľky.
 */
void ht_init(ht_table_t *table) {
    if (table != NULL) {
        for (int i = 0; i < HT_SIZE; i++)
            (*table)[i] = NULL;
    }
}

/*
 * Vyhľadanie prvku v tabuľke.
 *
 * V prípade úspechu vráti ukazovateľ na nájdený prvok; v opačnom prípade vráti
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
    if (table == NULL) return NULL;

    // get hash value
    int hash = get_hash(key);
    ht_item_t *item = (*table)[hash];
    // search in linked-list
    if (item != NULL) {
        if (strcmp(item->key, key)) {
                 // not found   || searched item
            for (; item != NULL && strcmp(item->key, key); item = item->next)
                ; // searching
        }
    }
    return item;
}

/*
 * Vloženie nového prvku do tabuľky.
 *
 * Pokiaľ prvok s daným kľúčom už v tabuľke existuje, nahraďte jeho hodnotu.
 *
 * Pri implementácii využite funkciu ht_search. Pri vkladaní prvku do zoznamu
 * synonym zvoľte najefektívnejšiu možnosť a vložte prvok na začiatok zoznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {
    if (table == NULL) return;

    // search for item with key
    ht_item_t *item = ht_search(table, key);
    // item doesn't exist
    if (item != NULL) {
        item->value = value;
    } else {
        int hash = get_hash(key);
        // allocate memory for item
        ht_item_t *item = (ht_item_t *)malloc(sizeof(ht_item_t));
        if (item != NULL) {
            // allocate memory for key
            item->key = calloc(strlen(key) + 1, sizeof(char));
            if (item->key != NULL) {
                strcpy(item->key, key);
                item->value = value;
                // add item to the start of the linked-list
                item->next  = (*table)[hash];
                (*table)[hash] = item;
            }
        }
    }
}

/*
 * Získanie hodnoty z tabuľky.
 *
 * V prípade úspechu vráti funkcia ukazovateľ na hodnotu prvku, v opačnom
 * prípade hodnotu NULL.
 *
 * Pri implementácii využite funkciu ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
    if (table == NULL) return NULL;

    int hash = get_hash(key);
    ht_item_t *item = (*table)[hash];
    // list is not empty
    if (item != NULL) {
        // searched item is not first in the list
        if (strcmp(item->key, key)) {
                // not found    || searched item found
            for (; item != NULL && strcmp(item->key, key); item = item->next)
                ; // searching...
        }
    }
    return item != NULL ? &(item->value) : NULL;
}

/*
 * Zmazanie prvku z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje priradené k danému prvku.
 * Pokiaľ prvok neexistuje, nerobte nič.
 *
 * Pri implementácii NEVYUŽÍVAJTE funkciu ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
    if (table == NULL) return;

    int hash = get_hash(key);
    ht_item_t *item = (*table)[hash];

    if (item != NULL) {
        // searched item is not first in the list
        if (strcmp(item->key, key)) {
                // item not found     || seached item found
            for (; item->next != NULL && strcmp(item->next->key, key); item = item->next)
                ; // searching...

            // if item is found
            if (item != NULL && item->next != NULL) {
                ht_item_t *del_item = item->next;
                ht_item_t *tmp = del_item->next;
                free(del_item->key);
                free(del_item);
                item->next = tmp;
            }
        } else {
            ht_item_t *tmp = item;
            (*table)[hash] = tmp->next;
            free(tmp->key);
            free(tmp);
        }
    }

}

/*
 * Zmazanie všetkých prvkov z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje a uvedie tabuľku do stavu po
 * inicializácii.
 */
void ht_delete_all(ht_table_t *table) {
    if (table == NULL) return;

    // for each index 
    for (int i = 0; i < HT_SIZE; i++) {
        ht_item_t *item = (*table)[i];
        // clear whole list
        while (item != NULL) {
            ht_item_t *tmp = item->next;
            free(item->key);
            free(item);
            item = tmp;
        }
        (*table)[i] = NULL;
    }
}
