/*
 * main.c
 * Reseni IOS-proj2 "Santa Claus problem", 20.04.21
 * Autor: Hung Do, VUT FIT
 * 
 * Soubor obsahujici MAIN funkci spolecne s tvorbou procesu
 */
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "base.h"
#include "tasks.h"

#define MAX_TIME 1000         // Maximalni povolena hodnota casu uspani
#define MAX_NR   20           // Maximalni povoleny pocet sobu
#define F_PATH   "proj2.out"  // cesta k souboru


// vyznam hodnot na indexech pole
enum { NE, NR, TE, TR, ARR_SIZE };

/**
 * @brief Nacitani argumentu
 *
 * @param argv  Hodnoty argumentu
 * @param arr   Pole hodnot ("ulozny prostor")
 * @param size  Velikost pole
 * @return Nenulovou hodnotu, pokud se nepodarilo spravne nacist argumenty
 */
int load_args(char **argv, int *arr, int size)
{
    int ret_val = EXIT_SUCCESS;
    char *endptr = NULL;
    for (int i = 0; i < size; i++)
    {
        arr[i] = strtol(argv[i+1], &endptr, 10);
        // kontrola formatu a mezi
        if (endptr[0])
            return EXIT_FAILURE;
    }

    // kontrola mezi
    if (arr[NE] <= 0 || arr[NR] <= 0 || arr[NR] >= 20)
        ret_val = EXIT_FAILURE;
    else if (arr[TE] < 0 || arr[TE] > 1000 || arr[TR] < 0 || arr[TR] > 1000)
        ret_val = EXIT_FAILURE;
    
    return ret_val;
}

/**
 * @brief Vytvori nove procesy
 * @details Vytvori nove procesy pomoci fork(), child process zavola svoji funkci 
 *  a po dokonceni uvolni pamet a zavola exit
 *  Pokud se nepovede vytvorit potomek, ukonci se vsechny potomci a vraci EXIT_FAILURE
 *
 * @param a_vals    Pole s nactenymi hodnotami argumentu
 * @return Nenulove cislo, pokud se nepodarilo vytvorit vsechny potomky
 */
int forking(int *a_vals, pid_t *ids) {
    int i;
    for (i = 0; i < a_vals[NE] + a_vals[NR] + 1; i++)
    {
        if (i == 0)     // proces Santa
        {
            if ((ids[i] = fork()) < 0)     { goto err_child; }
            else if (ids[i] == 0)
            {
                santa_task(a_vals[NR]);
                free(ids);
                fclose(f);
                exit(EXIT_SUCCESS);
            }
        }
        else if (i <= a_vals[NE])   // proces Elf
        {
            if ((ids[i] = fork()) < 0)     { goto err_child; }
            else if (ids[i] == 0)
            {
                elf_task(i, a_vals[TE]);
                free(ids);
                fclose(f);
                exit(EXIT_SUCCESS);
            }
        }
        else    // proces Sob
        {
            if ((ids[i] = fork()) < 0)     { goto err_child; }
            else if (ids[i] == 0)
            {
                rendeer_task(i - a_vals[NE], a_vals[NR], a_vals[TR]);
                free(ids);
                fclose(f);
                exit(EXIT_SUCCESS);
            }
        } // if (i == 0); else if (i < a_vals); else
    } // for (int i = 0; ... )
    return EXIT_SUCCESS;

// Neuspesne vytvoreni procesu
// Ukonci vsechny potomky
err_child:
    for (--i; i >= 0; i--)
        kill(ids[i], SIGTERM);
    return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    // Prace s argumenty
    if (argc < 4)   { goto err_args; }
    int a_vals[ARR_SIZE] = { 0, };      // Pole s nactenymi argumenty

    if (load_args(argv, a_vals, ARR_SIZE))  { goto err_args; }
    pid_t *ids = calloc(a_vals[NE] + a_vals[NR] + 1, sizeof(pid_t));
    if (ids == NULL)  { goto err_pids; }

    // vytvoreni souboru k zapisu
    if ((f = fopen(F_PATH, "w")) == NULL)
    {
        fprintf(stderr, "Nepodarilo se otevrit soubor\n");
        goto err_file;
    }

    // inicializace globalnich struktur
    if (my_sems_init(1, 0, 1, 0, 0, 0)) { goto err_sem_init; }
    if (glob_var_init())                { goto err_glob_init; }
    glob->n_rends = a_vals[NR];
    glob->counter = 1;
    glob->running = 1;

    if (forking(a_vals, ids))    { goto err_fork; }

    int exit_status;
    // cekani nez se vsechny dedicne procesy dokonci
    for (int i = 0; i < a_vals[NE] + a_vals[NR] + 1; i++)
        waitpid(ids[i], &exit_status, 0);

    // uklid alokovanych pameti
    fclose(f);
    free(ids);
    my_sems_destroy();
    glob_var_destoy();
    return EXIT_SUCCESS;

// Error-handeling
err_args:
    fprintf(stderr, "Spatne nactene argumenty!\n");
    return EXIT_FAILURE;
err_pids:
    fprintf(stderr, "Neuspesna alokace pole pid\n");
    return EXIT_FAILURE;
err_glob_init:
    my_sems_destroy();
err_sem_init:
    fclose(f);
err_file:
    free(ids);
    return EXIT_FAILURE;
err_fork:
    fprintf(stderr, "Neuspesna tvorba procesu\n");
    fclose(f);
    free(ids);
    my_sems_destroy();
    glob_var_destoy();
    return EXIT_FAILURE;
}
