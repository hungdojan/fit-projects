/**
 * Toto je zdrojovy kod programu, ktery provadi zakladni tabulkove 
 * operace na vstupnich datech. Na vstupu dostane textovy soubor s daty, 
 * ktery naslednymi argumenty na prikazovem radku upravy a zpracuje. Vysledek 
 * vypise na vystup.
 * 
 * @author Hung Do
 * @since 13.11.2020
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define MAX_INPUT_LENGTH 10242
#define MAX_STR_LENGTH 101
#define INT_MAX_STR_LEN 10
#define TILL_END INT_MAX
#define DEFAULT_DELIM ' '
#define FLOATING_POINT_CHAR '.'

/* ---- Osetreni chyb ------ */

/** 
 * Vyctovy typ Error s vypisem moznych chyb
*/
enum Error
{
    ERR_FEMPTY = 1,
    ERR_INPUT_TOO_LONG,
    ERR_STR_LENGTH_TOO_LONG,
    ERR_CLM_OUT_OF_BOUND,
    ERR_OUT_OF_BOUND,
    ERR_NOTNUM,
    ERR_WRONG_CCOUNT,
    ERR_WRONG_ARGV,
    ERR_UNDEFINED
};

/* ---- Struktury ---------- */

/** 
 * Tato struktura uklada hlavni informace o tabulkovem souboru.
 * znak oddelovace, pocet radku a sloupcu
*/
typedef struct file_t
{
    char list_of_delims[MAX_STR_LENGTH]; // mnozina moznych znaku oddelovacu
    // znak oddelovace, ktery se ma objevit ve vyslednem souboru
    char out_delim;
    // pocet sloupcu v souboru (podle prvniho radku v puvodnim souboru)
    int clm_count;
    // momentalni pocet radku
    int row_count;
} File;

/** 
 * Tato struktura uklada informace o nactenem radku.
*/
typedef struct line_t
{
    char cur_value[MAX_INPUT_LENGTH];  // obsah nacteneho radku ze souboru
    char next_value[MAX_INPUT_LENGTH]; // hodnota dalsiho radku
    int clm_count;                     // pocet sloupcu na danem radku (pred upravou)
} Line;

/** 
 * Tato struktura uklada informace o prikazech zpracovani radku a selekci dat.
*/
typedef struct select_t
{
    char command[8];                  // prikaz, ktery se ma provest
    char beginw_str[MAX_STR_LENGTH];  // retezec pro prikaz beginswith
    char contain_str[MAX_STR_LENGTH]; // retezec pro prikaz contains
    // retezex uklada nedefinovanou hodnotu pro pozdejsi pouziti v prikazu
    char cmd_str[MAX_STR_LENGTH];
    // prikazy pro selekci sloupci v prikazech beginswith a contains
    int begin_clm, contain_clm;
    // promenne ukladaji hodnoty sloupcu pro pozdejsi manipulaci
    int clm_C, clm_N, clm_M;
    // promenne ukladaji hodnoty radku pro pozdejsi manipulaci
    int row_N, row_M;
} Select;

/** 
 * Tato struktura uklada informace o prikazech upravy tabulky.
*/
typedef struct edit_t
{
    int arow;        // pocet radku, ktere se maji pridat na konec souboru
    int acol;        // pocet sloupcu, ktere se maji pridat na konec radku
    int temp_row;    // posledni nacteny radek na upravu
    int temp_column; // posledni nacteny sloupce na upravu

    /** 
     * Promenna reprezentuje teoretickou datovou strukturu Queue (fronta) a 
     * je datoveho typu retezec. Jedna polozka se sklada ze dvou casti: 
     * cislo radku a prikaz k vykonani. S touto datovou strukturou se vazou tri 
     * funkce: 
     * enqueue - pridava polozku na konec fronty
     * peek - vraci polozku na prvnim miste bez odstraneni
     * dequeue - odstrani polozku na prvnim miste
     */
    char queue_rows[MAX_STR_LENGTH];

    /** 
     * Promenna reprezentuje teoretickou datovou strukturu Queue (fronta) a 
     * je datoveho typu retezec. Jedna polozka se sklada ze dvou casti: 
     * cislo sloupce a prikaz k vykonani. S touto datovou strukturou se vazou tri 
     * funkce: 
     * enqueue - pridava polozku na konec fronty
     * peek - vraci polozku na prvnim miste bez odstraneni
     * dequeue - odstrani polozku na prvnim miste
     */
    char queue_clm[MAX_STR_LENGTH];

} Edit;

/**
 * Funkce vypise chybovou hlasku podle druhu chyby.
*/
int errors_out(int error_type, const File *file)
{
    switch (error_type)
    {
    case ERR_FEMPTY:
        fprintf(stderr, "Error (line %d): The file is empty.\n", file->row_count);
        break;

    case ERR_INPUT_TOO_LONG:
        fprintf(stderr, "Error (line %d): length of the line is out of range.\n", file->row_count);
        break;

    case ERR_STR_LENGTH_TOO_LONG:
        fprintf(stderr, "Error (line %d): length of the string is out of range.\n", file->row_count);
        break;

    case ERR_CLM_OUT_OF_BOUND:
        fprintf(stderr, "Error (line %d): command's column is out of bound.\n", file->row_count);
        break;

    case ERR_OUT_OF_BOUND:
        fprintf(stderr, "Error (line %d): index out of bound.\n", file->row_count);
        break;

    case ERR_NOTNUM:
        fprintf(stderr, "Error (line %d): input is not a number.\n", file->row_count);
        break;

    case ERR_WRONG_CCOUNT:
        fprintf(stderr, "Error (line %d): the number of columns doesn't match the main line.\n", file->row_count);
        break;

    case ERR_WRONG_ARGV:
        fprintf(stderr, "Error: invalid argument syntax.\n");
        break;

    default:
        fprintf(stderr, "Error (line %d): SEGMENTATION FAULT! CORE PROBABLY DUMPED... Or maybe you r just dumb!\n", file->row_count);
        break;
    }
    return EXIT_FAILURE;
}

/* ---- Set-up funkce -------*/

void init_file(File *file);
void init_line(Line *line);
void init_select(Select *sel);
void init_edit(Edit *edit);
int load_arguments(int argc, char **argv, Select *sel, Edit *edit, File *file);
int load_delim(File *file, int argc, char **argv);
int load_edit_cmd(Edit *edit, int argc, char **argv, int *i, bool *flag);
int load_one_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_two_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_three_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_rest_cmds(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_selection_cmd(Select *sel, int argc, char **argv, int *i, bool *flag);
int load_first_line(Line *line, File *file);

/* ---- Pomocne funkce ----- */

int load_line(Line *line, File *file);

int indexof(char *str_in, char c, int start_index, int position);
void create_newline(File *file, char *str_out);
void insert_str(char *str_in, int start_index,
                char *str_to_insert, char *str_out);
void remove_str(char *str_in, int start_index, int length, char *str_out);
void replace_str(char *str_in, int start_index,
                 int length, char *str_replacing, char *str_out);
void substring(char *str_in, int start_index, int len, char *str_out);

void get_string_in_cell(char *str_in, int clm,
                        const File *file, char *str_out);
void get_cell_info(char *str_in, int clm, char delim, int clm_count,
                   int *pstart_index, int *plen_out);

int last_index(char *str);
bool is_num(char *str_in);
int round_number(char *float_number);
bool in_range(int number, int lower_bound, int upper_bound);

void enqueue(Edit *edit, char *cmd, int value);
void dequeue(char *str);
void peek(char *str, char *item_out);

/* ---- Uprava tabulky ----- */

bool edit_rows(File *file, Edit *edit);
void edit_clms(char *queue_clm, int acol, char *line_value, File *file);
void irow(File *file, Edit edit);
void arow(File *file, Edit edit, int num_of_rows);
void icol(char *line_value, int clm, char delim, int *clm_count);
void dcol(char *line_value, int clm, char delim, int *clm_count);

/* ---- Zpracovavani dat --- */

int do_command(Select *sel, Line *line, const File *file);
void cset(Line *line, int clm, const File *file, char *str_to_insert);
void to_lower(Line *line, int clm, const File *file);
void to_upper(Line *line, int clm, const File *file);
int round_func(Line *line, int clm, const File *file);
int int_func(Line *line, int clm, const File *file);
void copy(Line *line, int clm_N, int clm_M, const File *file);
void swap(Line *line, int clm_N, int clm_M, const File *file);
void move(Line *line, int clm_N, int clm_M, const File *file);
int csum(Line *line, int clm_C, int clm_N, int clm_M, const File *file);
int cavg(Line *line, int clm_C, int clm_N, int clm_M, const File *file);
int cmin(Line *line, int clm_C, int clm_N, int clm_M, const File *file);
int cmax(Line *line, int clm_C, int clm_N, int clm_M, const File *file);
void ccount(Line *line, int clm_C, int clm_N, int clm_M, const File *file);
void cseq(Line *line, int clm_N, int clm_M, char *cmd_str, const File *file);
void rseq(Line *line, int clm_C, char *cmd_str, const File *file);
int rsum(Line *line, int clm_C, int row_M, char *cmd_str, const File *file);
int ravg(Line *line, int clm_C, int row_N, int row_M, char *cmd_str, const File *file);
int rmin(Line *line, int clm_C, int row_N, int row_M, char *cmd_str, const File *file);
int rmax(Line *line, int clm_C, int row_N, int row_M, char *cmd_str, const File *file);
void rcount(Line *line, int clm_C, int row_M, char *cmd_str, const File *file);

/* ---- Selekce radku ------ */

bool begins_with(char *str_in, int clm, const File *file, char *str_to_cmp);
bool contains(char *str_in, int clm, const File *file, char *str_to_cmp);
int set_beginswith(Select *sel, char **argv, int *index);
int set_contains(Select *sel, char **argv, int *index);
int set_rows(Select *sel, char **argv, int *index);

/* ------------------------- */

/** 
 * Funkce provadi vsechny upravy na tabulce a zpracovava data.
 * @param edit ukazatel na datovou strukturu Edit 
 * @param file ukazatel na datovou strukturu File
 * @param line ukazatel na datovou strukturu Line
 * @param sel ukazatel na datovou strukturu Select
 * @return chybovy index; 0 pro bezchybny proces
*/
int processing_data(Edit *edit, File *file, Line *line, Select *sel)
{
    int error = 0;
    // upravy irow a drow
    if (edit->queue_rows[0] != '\0')
    {
        // kontrola pokud je command typu 'd' (delete)
        // nacte novy radek a preskoci cyklus
        if (edit_rows(file, edit))
        {
            if ((error = load_line(line, file)))
                return error;
            return 0; // chova se stejne jako continue v do-while
        }
    }

    // zpracovani dat
    if (begins_with(line->cur_value, sel->begin_clm, file, sel->beginw_str) &&
        contains(line->cur_value, sel->contain_clm, file, sel->contain_str) &&
        in_range(file->row_count, sel->row_N, sel->row_M))
        error = do_command(sel, line, file);

    if (error != 0)
        return error;

    // upravy icol a dcol
    if (edit->queue_clm[0] != '\0' || edit->acol != 0)
        edit_clms(edit->queue_clm, edit->acol, line->cur_value, file);

    // zapis do souboru
    fputs(line->cur_value, stdout);
    return 0;
}

int main(int argc, char *argv[])
{
    int error = 0; // promenna uchovava indexy chybovych hlaseni
    File file;
    Line line;
    Select sel;
    Edit edit;

    init_file(&file);
    init_line(&line);
    init_select(&sel);
    init_edit(&edit);

    // projede argumenty a vraci pripadne chyby
    if ((error = load_arguments(argc, argv, &sel, &edit, &file)))
        return errors_out(error, &file);

    // nacte prvni radek souboru a nacte potrebna informace o souboru
    if ((error = load_first_line(&line, &file)))
        return errors_out(error, &file);

    if (!in_range(sel.clm_C, 1, file.clm_count) ||
        !in_range(sel.clm_N, 1, file.clm_count) ||
        !in_range(sel.clm_M, 1, file.clm_count))
        return errors_out(ERR_CLM_OUT_OF_BOUND, &file);

    do
    {
        // posledni radek nema znak \n; upravime hodnotu posledniho radku
        // dulezite pro pro prikazy, ktere pouzivaji znak pomlcky
        // jako znak posledniho radku
        if (line.next_value[0] == '\0')
        {
            // ignoruje, pokud se nenastavil znak posledniho radku
            sel.row_N = sel.row_N == TILL_END ? file.row_count : sel.row_N;
            sel.row_M = sel.row_M == TILL_END ? file.row_count : sel.row_M;
        }

        // proces upravy tabulky a dat
        if ((error = processing_data(&edit, &file, &line, &sel)))
            return errors_out(error, &file);

        // cteni ze souboru
        if ((error = load_line(&line, &file)))
            return errors_out(error, &file);
    } while (line.cur_value[0] != '\0');

    // pridani radku na konec
    if (edit.arow != 0)
        arow(&file, edit, edit.arow);

    return EXIT_SUCCESS;
}

/* --------------------------------------------------- */

/**
 * Funkce inicializuje datovou strukturu File.
 * @param file ukazatel na vytvorenou strukturu File
*/
void init_file(File *file)
{
    strcpy(file->list_of_delims, "");
    file->out_delim = DEFAULT_DELIM;
    file->clm_count = 1;
    file->row_count = 1;
}

/** 
 * Funkce inicializuje datovou strukturu Line
 * @param line ukazatel na vytvorenou strukturu Line
*/
void init_line(Line *line)
{
    strcpy(line->cur_value, "");
    strcpy(line->next_value, "");
    line->clm_count = 0;
}

/**
 * Funkce inicializuje datovou strukturu Select.
 * @param sel ukazatel na vytvorenou strukturu Select
*/
void init_select(Select *sel)
{
    strcpy(sel->command, "");
    strcpy(sel->beginw_str, "");
    strcpy(sel->contain_str, "");
    strcpy(sel->cmd_str, "");

    sel->clm_C = 1;
    sel->clm_N = 1;
    sel->clm_M = 1;

    sel->row_N = 1;
    sel->row_M = TILL_END;
}

/** 
 * Funkce inicializuje datovou strukturu Edit
 * @param edit ukazatel na vytvorenou strukturu Edit
*/
void init_edit(Edit *edit)
{
    edit->arow = 0;
    edit->acol = 0;
    strcpy(edit->queue_rows, "");
    edit->temp_row = 0;
    strcpy(edit->queue_clm, "");
    edit->temp_column = 0;
}

/** 
 * Funkce pracuje s argumenty od uzivatel a kontoluje je se vsemi povolenymi prikazy. 
 * V pripade chyby vraci zpatky nenulove cislo.
 * Jako prvni kontroluje argumenty delim a nasledne pojizdi cely seznam.
 * V pripade, ze argument se schoduje s povolenym prikazem, nacte jeho parametry. 
 * Pokud se argument neshoduje s zadnym nabizenym prikazem, vraci chybove hlaseni.
 * Prikazy jsou rozdelene do podskupin pro prehlednejsi a lehci operaci.
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param sel ukazatel na datovou strukturu Select
 * @param edit ukazatel na datovou strukturu Edit
 * @param file ukazatel na datovou strukturu File
 * @return vraci 0 pro bezchybne zadani argumentu; kladne cislo pro chybove hlaseni
*/
int load_arguments(int argc, char **argv, Select *sel, Edit *edit, File *file)
{
    int i = 1;
    int error_state = 0; // Navratova hodnota pro pripad necekaneho vstupu

    // promenna ukazuje, zda se v jednom cyklu zmenila nejaka hodnota
    bool flag;
    if (argc < 2) // zadny argument
        return 0;

    if (!strcmp(argv[1], "-d")) // prvni argument je znak oddelovace
    {
        if ((error_state = load_delim(file, argc, argv)))
            return error_state;
        i += 2;
    }

    // prochazeni argumentu a porovnavani s prikazy
    for (; i < argc; i++)
    {
        flag = false;

        if ((error_state = load_edit_cmd(edit, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_selection_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_rest_cmds(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_three_arg_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_two_arg_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        if ((error_state = load_one_arg_cmd(sel, argc, argv, &i, &flag)))
            break;
        if (flag)
            continue;

        // Pokud se argument neshoduje s zadnym ze zadanych prikazu
        // Jedna se o chybnou syntaxi argumentu
        return ERR_WRONG_ARGV;
    }
    return error_state;
}

/** 
 * Funkce pracuje s argumenty pro nacteni znaku DELIM. 
 * @param file ukazatel na datovou strukturu File
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_delim(File *file, int argc, char **argv)
{
    if (argc >= 3)
    {
        if (strlen(argv[2]) > MAX_STR_LENGTH)
            return ERR_STR_LENGTH_TOO_LONG;

        strcpy(file->list_of_delims, argv[2]);
        file->out_delim = file->list_of_delims[0];
    }

    // V pripade, ze uzivatel zadal -d ale neupresnil mnozinu moznych delim
    else
        return ERR_WRONG_ARGV;
    return 0;
}

/** 
 * Funkce nacte prikazy pro zpracovani dat s jednim parametrem z argumentu.
 * Args: (clm_C).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_one_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (argc >= *i + 2)
    {
        if (!strcmp(argv[*i], "tolower") ||
            !strcmp(argv[*i], "toupper") ||
            !strcmp(argv[*i], "round") ||
            !strcmp(argv[*i], "int"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]))
                return ERR_NOTNUM;
            sel->clm_C = atoi(argv[++(*i)]);
            *flag = true;
        }
    }
    return 0;
}

/** 
 * Funkce nacte prikazy pro zpracovani dat se dvema parametry z argumentu. 
 * Args: (clm_N, clm_M) nebo (clm_C, str).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_two_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (argc >= *i + 3)
    {
        if (!strcmp(argv[*i], "copy") ||
            !strcmp(argv[*i], "swap") ||
            !strcmp(argv[*i], "move"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]))
                return ERR_NOTNUM;
            sel->clm_N = atoi(argv[++(*i)]);
            sel->clm_M = atoi(argv[++(*i)]);
            *flag = true;
            return 0;
        }

        if (!strcmp(argv[*i], "cset"))
        {
            strcpy(sel->command, argv[*i]);
            sel->clm_C = atoi(argv[++(*i)]);
            if (strlen(argv[*i + 1]) > 100)
                return ERR_STR_LENGTH_TOO_LONG;
            strcpy(sel->cmd_str, argv[++(*i)]);
            *flag = true;
        }
    }
    return 0;
}

/** 
 * Funkce nacte prikazy pro zpracovani dat se tremi parametry z argumentu. 
 * Kontroluje se, zda jsou parametry napsane ve spravnem datovem typu a splnuji 
 * podminky pro praci s nimi. Nazev prikazu se uklada do sel.command.
 * Args: (clm_C, clm_N, clm_M) nebo (clm_C, row_N, row_M).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_three_arg_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (argc >= *i + 4)
    {
        // kontrola radkovych prikazu
        if (!strcmp(argv[*i], "csum") || !strcmp(argv[*i], "cavg") ||
            !strcmp(argv[*i], "cmin") || !strcmp(argv[*i], "cmax") ||
            !strcmp(argv[*i], "ccount"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
                !is_num(argv[*i + 3]))
                return ERR_NOTNUM;
            sel->clm_C = atoi(argv[++(*i)]);
            sel->clm_N = atoi(argv[++(*i)]);
            sel->clm_M = atoi(argv[++(*i)]);
            if (in_range(sel->clm_C, sel->clm_N, sel->clm_M) ||
                sel->clm_N > sel->clm_M)
                return ERR_WRONG_ARGV;
            *flag = true;
            return 0;
        }

        // kontrola sloupcovych prikazu
        else if (!strcmp(argv[*i], "rsum") || !strcmp(argv[*i], "ravg") ||
                 !strcmp(argv[*i], "rmin") || !strcmp(argv[*i], "rmax") ||
                 !strcmp(argv[*i], "rcount"))
        {
            strcpy(sel->command, argv[*i]);
            if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
                !is_num(argv[*i + 3]))
                return ERR_NOTNUM;
            sel->clm_C = atoi(argv[++(*i)]);
            sel->row_N = atoi(argv[++(*i)]);
            // musime pridat 1 pro zapis do nasledujiciho radku radku
            sel->row_M = atoi(argv[++(*i)]) + 1;
            if (sel->row_N >= sel->row_M)
                return ERR_WRONG_ARGV;
            *flag = true;
        }
    }
    return 0;
}

/** 
 * Funkce zkontroluje posledni specialni prikazy mezi argumenty. 
 * Kontroluje se, zda jsou parametry napsane ve spravnem datovem typu a splnuji 
 * podminky pro praci s nimi. Nazev prikazu se uklada do sel.command.
 * Parametr B se specialne uklada do sel.cmd_str, aby nebyla potreba vytvaret 
 * novou promennou.
 * Prikaz cseq (clm_N, clm_M, int_B); rseq (row_N, row_M, int_B).
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_rest_cmds(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    if (!strcmp(argv[*i], "cseq") && argc >= *i + 4)
    {
        strcpy(sel->command, argv[*i]);
        if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
            !is_num(argv[*i + 3]))
            return ERR_NOTNUM;
        sel->clm_N = atoi(argv[++(*i)]);
        sel->clm_M = atoi(argv[++(*i)]);
        strcpy(sel->cmd_str, argv[++(*i)]);
        *flag = true;
    }

    else if (!strcmp(argv[*i], "rseq") && argc >= *i + 5)
    {
        strcpy(sel->command, argv[*i]);
        // kontrola spravneho typu
        if (!is_num(argv[*i + 1]) || !is_num(argv[*i + 2]) ||
            !is_num(argv[*i + 3]) ||
            !(is_num(argv[*i + 4]) || argv[*i + 4][0] == '-'))
            return ERR_NOTNUM;
        sel->clm_C = atoi(argv[++(*i)]);
        sel->row_N = atoi(argv[++(*i)]);

        // pokud uzivatel napsal jako argument pomlcku,
        // program priradi hodnotu TILL_END
        if (argv[*i + 1][0] == '-')
        {
            sel->row_M = TILL_END;
            (*i)++;
        }
        else
            sel->row_M = atoi(argv[++(*i)]);

        strcpy(sel->cmd_str, argv[++(*i)]);
        *flag = true;
    }
    return 0;
}

/** 
 * Funkce projizdi prikazy pro upravu tabulky mezi argumenty. 
 * Argumenty znacici hodnotu radku/sloupce musi nesmi narusit 
 * proudost zpracovani souboru.
 * @param edit ukazatel na datovou strukturu Edit
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_edit_cmd(Edit *edit, int argc, char **argv, int *i, bool *flag)
{
    // argumenty pridani radku/sloupce na konec souboru/radku
    if (!strcmp(argv[*i], "arow"))
    {
        edit->arow++;
        *flag = true;
    }
    else if (!strcmp(argv[*i], "acol"))
    {
        edit->acol++;
        *flag = true;
    }

    // reseni argumentu d- (delete) a i- (insert)
    // hodnoty se pridavaji do fronty pomoci fce enqueue
    else if (argc >= *i + 3 && is_num(argv[*i + 1]) && is_num(argv[*i + 2]))
    {
        int from = atoi(argv[*i + 1]);
        int to = atoi(argv[*i + 2]);

        if (!strcmp(argv[*i], "drows") || !strcmp(argv[*i], "dcols"))
        {
            // kontroluje posloupnost argumentu a moznost proudoveho zpracovani
            if (from > to ||
                (from <= edit->temp_row && argv[*i][1] == 'r') ||
                (from <= edit->temp_column && argv[*i][1] == 'c'))
                return ERR_WRONG_ARGV;

            for (; from <= to; from++)
                enqueue(edit, argv[*i], from);

            // nacteni posledniho zapsaneho radku/sloupce
            if (argv[*i][1] == 'r')
                edit->temp_row = from - 1;
            else
                edit->temp_column = from - 1;

            *i += 2;
            *flag = true;
            return 0;
        }
    }

    else if (argc >= *i + 2 && is_num(argv[*i + 1]))
    {
        if (!strcmp(argv[*i], "irow") || !strcmp(argv[*i], "drow") ||
            !strcmp(argv[*i], "icol") || !strcmp(argv[*i], "dcol"))
        {
            //  moznost proudoveho zpracovani
            if ((atoi(argv[*i + 1]) <= edit->temp_row && argv[*i][1] == 'r') ||
                (atoi(argv[*i + 1]) <= edit->temp_column && argv[*i][1] == 'c'))
                return ERR_WRONG_ARGV;

            // nacteni posledniho zapsaneho radku/sloupce
            if (argv[*i][1] == 'r')
                edit->temp_row = atoi(argv[*i + 1]);
            else
                edit->temp_column = atoi(argv[*i + 1]);

            enqueue(edit, argv[*i], atoi(argv[*i + 1]));
            (*i)++;
            *flag = true;
        }
    }

    return 0;
}

/** 
 * Funkce projede vsechny prikazy pro selekci radku a bunek. 
 * Po shode nacte do struktur pro pozdejsi praci s nimi.
 * @param sel ukazatel na datovou strukturu Select
 * @param argc pocet argumentu
 * @param argv ukazatel na zacatek pole znaku
 * @param i index ve for-loopu pro indexaci argv
 * @param flag v pripade uspesneho nacteni dat zmeni hodnotu na true
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_selection_cmd(Select *sel, int argc, char **argv, int *i, bool *flag)
{
    int error; // promenna kontrolujici chyby

    if (!strcmp(argv[*i], "beginswith") && argc >= *i + 3)
    {
        if ((error = set_beginswith(sel, argv, i)))
            return error;
        *flag = true;
    }
    else if (!strcmp(argv[*i], "contains") && argc >= *i + 3)
    {
        if ((error = set_contains(sel, argv, i)))
            return error;
        *flag = true;
    }

    else if (!strcmp(argv[*i], "rows") && argc >= *i + 3)
    {
        if ((error = set_rows(sel, argv, i)))
            return error;
        *flag = true;
    }
    return 0;
}

/** 
 * Funkce nacte prvni dva radky souboru do struktury Line a 
 * nastavi default hodnoty pro chod programu.
 * @param line ukazatel na datovou strukturu Line
 * @param file ukazatel na datovou strukturu File
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_first_line(Line *line, File *file)
{
    // nacte radek (10240 znaku, jeden byte pro znak noveho radku)
    // zkontroluje zda je prazdny
    if (fgets(line->cur_value, MAX_INPUT_LENGTH - 1, stdin) == NULL)
        return ERR_FEMPTY;

    if (indexof(line->cur_value, '\n', (int)strlen(line->cur_value) - 1, 1) == -1)
        return ERR_INPUT_TOO_LONG;

    // zkontroluje jaky oddelovac se nachazi v souboru
    // pokud se program dostal az do tehle faze
    // muzeme usoudit, ze v tabulkovem souboru se nachazi alespon 1 radek a sloupec
    file->clm_count = 1;
    file->row_count = 1;

    // pocitani sloupcu a prepis znaku oddelovace
    if (file->list_of_delims[0] == '\0')
        sprintf(file->list_of_delims, "%c", file->out_delim);

    for (int i = 0; line->cur_value[i]; i++)
    {
        if (indexof(file->list_of_delims, line->cur_value[i], 0, 1) != -1)
        {
            file->clm_count++;
            line->cur_value[i] = file->list_of_delims[0];
        }
    }
    file->out_delim = file->list_of_delims[0];

    if ((fgets(line->next_value, MAX_INPUT_LENGTH - 1, stdin) == NULL))
        line->next_value[0] = '\0';
    return 0;
}

/* ----------------------------------------------------- */

/**
 * Funkce prekopiruje hodnotu line.next_value do line.cur_value a nacte dalsi radek 
 * do line.next_value. Kontroluje chybne vstupy.
 * @param line ukazatel na datovou strukturu Line
 * @param file ukazatel na datovou strukturu File
 * @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int load_line(Line *line, File *file)
{
    // prekopiruje nasledujici radek do
    strcpy(line->cur_value, line->next_value);

    if (line->cur_value[0] == '\0') // jedna se o posledni radek
        return 0;

    if (fgets(line->next_value, MAX_INPUT_LENGTH - 1, stdin) == NULL)
        line->next_value[0] = '\0';

    // vypocet sloupcu a prepis znaku oddelovace
    line->clm_count = 1;
    for (int i = 0; line->cur_value[i]; i++)
    {
        if (indexof(file->list_of_delims, line->cur_value[i], 0, 1) != -1)
        {
            line->clm_count++;
            line->cur_value[i] = file->list_of_delims[0];
        }
    }
    file->row_count++;

    // kontrola chyb
    if (line->clm_count != file->clm_count)
        return ERR_WRONG_CCOUNT;
    if (indexof(line->cur_value, '\n', (int)strlen(line->cur_value) - 1, 1) == -1)
        return ERR_INPUT_TOO_LONG;

    return 0;
}

/** 
 * Funkce hleda indexovou pozici znaku v retezci.
 * @param str_in vstupni retezec
 * @param c hledany znak
 * @param start_index pocatecni index, od ktereho ma funkce zacit hledat
 * @param position cislo udava, kolikaty znak v retezci ma hledat
 * @return vraci index hledaneho znaku v retezci (vraci -1 pokud nic nenasel)
*/
int indexof(char *str_in, char c, int start_index, int position)
{
    // specialni pripad position == 0
    if (position == 0)
        return 0;

    int i = start_index;
    int count = 0; // hodnota udava pocet hledanych znaku, ktere jiz potkal

    for (; str_in[i]; i++) // hledani
    {
        if (str_in[i] == c)
        {
            if (++count == position)
                return i;
        }
    }

    return -1; // znak nenalezen
}

/** 
 * Funkce vytvori novy radek a vypise do stdout souboru.
 * @param file strukture file_t obsahujici informace o programu a souboru 
 * @param str_out vystupni retezec
*/
void create_newline(File *file, char *str_out)
{
    // delka pole se urcuje podle toho, zda-li ma byt na konci radku znak '\n'
    int array_length = file->clm_count;
    char loc_str[array_length]; // lokalni retezec
    int i;

    // tvorba noveho radku
    for (i = 0; i < file->clm_count - 1; i++)
        loc_str[i] = file->out_delim;
    loc_str[array_length - 1] = '\n';
    i++;
    loc_str[i] = '\0'; // posledni znak je ukoncovaci znak '\0'

    strcpy(str_out, loc_str);
}

/** 
 * Funkce odstrani cast retezce ze retezce. 
 * Rozdeluje retezec na 3 casti: predpona, cast k odstraneni a priponu.
 * @param str_in vstupni retezec
 * @param start_index index prvniho znaku
 * @param length delka retezce, ktery se ma odstranit
 * @param str_out vystupni retezec
*/
void remove_str(char *str_in, int start_index, int length, char *str_out)
{
    int pre_str_len = start_index + 1;
    int post_str_len = (int)strlen(str_in) - (start_index + length) + 1;

    char pre_str[pre_str_len];   // predpona retezce, ktery se ma odstranit
    char post_str[post_str_len]; // pripona retezce, ktery se ma odstranit

    substring(str_in, 0, start_index, pre_str);
    substring(str_in, start_index + length,
              (int)strlen(str_in) - (start_index + length), post_str);

    // sjednoceni retezcu a ulozeni do str_out
    sprintf(str_out, "%s%s", pre_str, post_str);
}

/** 
 * Funkce prepise cast retezce.
 * @param str_in vstupni retezec
 * @param start_index index prvniho znaku
 * @param length delka retezce, ktery se ma odstranit
 * @param str_replacing retezec, ktery bude pridan misto vybraneho retezce
 * @param str_out vystupni retezec
*/
void replace_str(char *str_in, int start_index,
                 int length, char *str_replacing, char *str_out)
{
    remove_str(str_in, start_index, length, str_in);
    insert_str(str_in, start_index, str_replacing, str_out);
}

/**
 * Funkce vytvari cast retezce z puvodniho retezce.
 * @param str_in vstupni retezec
 * @param start_index pocatecni umisteni (odkud se zacina)
 * @param len delka noveho retezce
 * @param str_out vystupni retezec
*/
void substring(char *str_in, int start_index, int len, char *str_out)
{
    // pokud dojde k preteceni -> vraci prazdny retezec
    if (start_index + len > (int)strlen(str_in))
    {
        strcpy(str_out, "\0");
        return;
    }
    char loc_str[len + 1]; /* lokalni retezec */

    // tvorba retezce samotneho
    for (int i = 0; i < len; i++)
    {
        loc_str[i] = str_in[start_index + i];
    }
    loc_str[len] = '\0';

    // prirazeni k vyslednemu retezci
    strcpy(str_out, loc_str);
}

/**
 * Funkce slouzi k vlozeni retezce do retezce. 
 * Nejprve funkce rozdeli retezec na predponu a priponu 
 * a pak jej slozi dohromady.
 * @param str_in vstupni retezec
 * @param start_index index, na kterem ma funkce vlozit retezec
 * @param str_to_insert retezec, ktery se ma vlozit
 * @param str_out vystupni retezec
*/
void insert_str(char *str_in, int start_index, char *str_to_insert, char *str_out)
{
    // pokud je start_index == len -> pridat dozadu (append)
    // pokud je start_index > len -> chyba preteceni -> vraci prazdny retezec
    if (start_index > (int)strlen(str_in))
    {
        strcpy(str_out, "\0");
        return;
    }

    char loc_str[MAX_INPUT_LENGTH]; /* lokalni retezec */
    loc_str[0] = '\0';

    if (start_index > 0) // tvorba predpony retezce
    {
        int pre_str_len = start_index + 1;
        char pre_str[pre_str_len];
        substring(str_in, 0, start_index, pre_str);
        strcat(loc_str, pre_str);
    }

    strcat(loc_str, str_to_insert);

    if (start_index <= (int)strlen(str_in)) // tvorba pripony retezce
    {
        int post_str_len = (int)strlen(str_in) - start_index + 1;
        char post_str[post_str_len];
        substring(str_in, start_index, (int)strlen(str_in) - start_index, post_str);
        strcat(loc_str, post_str);
    }

    // prirazeni do vysledneho retezce
    strcpy(str_out, loc_str);
}

/** 
 * Funkce vraci obsah bunky na radku v tabulkovem souboru. 
 * @param str_in vstupni retezec
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
 * @param str_out vystupni retezec
*/
void get_string_in_cell(char *str_in, int clm, const File *file, char *str_out)
{
    int start_index, len;
    get_cell_info(str_in, clm, file->out_delim, file->clm_count, &start_index, &len);
    substring(str_in, start_index, len, str_out);
}

/** 
 * Funkce ziskava dve informace o bunce v retezce: jeji pocatecni index 
 * a delka retezce v bunce.
 * @param str_in vstupni retezec
 * @param clm sloupce v radku (pocita se od 1)
 * @param delim znak oddelovace v souboru
 * @param clm_count pocet sloupcu v souboru
 * @param pstart_index ukazatel na hodnotu pocatecni index
 * @param plen_out ukazatel na hodnotu delky retezce
*/
void get_cell_info(char *str_in, int clm, char delim, int clm_count,
                   int *pstart_index, int *plen_out)
{
    // pocatecni index je prvni index po prvnim oddelovaci
    // vyjimku tvori prvni sloupec, ktery nema pred sebou oddelovac
    *pstart_index = indexof(str_in, delim, 0, clm - 1);
    if (clm != 1)
        (*pstart_index)++;

    /** 
     * delka se urcuje jako rozdil indexu zadniho oddelovace
     * a pocatecniho indexu bunky
     * vyjimku tvori posledni sloupec, ktery nema za sebou oddelovac
     * u posledniho sloupce se provadi kontrola na znak '\n'
    */
    if (clm < clm_count)
        *plen_out = indexof(str_in, delim, 0, clm) - *pstart_index;
    else if (str_in[last_index(str_in)] == '\n')
        *plen_out = (int)strlen(str_in) - *pstart_index - 1;
    else
        *plen_out = (int)strlen(str_in) - *pstart_index;
}

/** 
 * Funkce vraci index posledniho znaku.
 * @param str retezec
 * @return index posledniho znaku
*/
int last_index(char *str)
{
    return (int)strlen(str) - 1;
}

/** 
 * Funkce zjisti, zda je retezec cislo
 * @param str_in vstupni retezec
 * @return true pokud je retezec cislo
*/
bool is_num(char *str_in)
{
    if (str_in[0] == '\0')
        return false;

    bool is_negative = str_in[0] == '-';
    bool is_float = false;
    int i;
    for (i = 0; str_in[i]; i++)
    {
        // kontroluje vyjimky, kdyz zadany znak neni cislice
        if (!isdigit(str_in[i]))
        {
            switch (str_in[i])
            {
            case '-':
                // znamenko minus muze byt jen na prvnim miste
                if (i != 0)
                    return false;
                else // pro kontrolu dalsich znaku
                    is_negative = true;
                break;
            case FLOATING_POINT_CHAR:
                // desetinna carka nemuze byt na prvnim miste,
                // nebo po znamenku minus, a muze byt v retezci jen jednou
                if (i == 0 || (is_negative && i == 1) || is_float)
                    return false;
                is_float = !is_float;
                break;
            // ostatni znaky nejsou povolene
            default:
                return false;
            }
        }
    }
    return true;
}

/** 
 * Funkce prevede retezec na cislo a zaokrouhli ho
 * @param number vstupni retezec cisla
 * @returns zaokrouhlene cislo
*/
int round_number(char *number)
{
    int int_number = atoi(number);
    double float_number = atof(number);

    // pokud je rozdil desetinneho cisla a jeho cele casti
    // vetsi nez 0.5, cislo se zaokrouhli nahoru
    // if (abs(float_number) - abs(int_number * 1.0) >= 0.5)
    //     int_number += abs(int_number) / int_number;
    if (float_number - int_number * 1.0 >= 0.5)
        int_number++;
    return int_number;
}

/** 
 * Funkce kontroluje, zda cislo number lezi v intervalu
 * @param number cislo, na ktere se ptame
 * @param lower_bound nejnizsi cislo intervalu
 * @param upper_bound nejvyssi cislo intervalu
 * @return pravdivost jestli cislo nalezi intervalu
*/
bool in_range(int number, int lower_bound, int upper_bound)
{
    return number >= lower_bound && number <= upper_bound;
}

/** 
 * Funkce pracuje s teoretickou strukturou Queue (fronta). 
 * Hodnoty, ktere nacte, pridava nakonec retezce.
 * @param edit ukazatel na datovou strukturu Edit
 * @param cmd prikaz ktery se ma nacist do struktury (row/col)
 * @param value hodnota radku nebo sloupce (zalezi na cmd)
*/
void enqueue(Edit *edit, char *cmd, int value)
{
    char loc_str[INT_MAX_STR_LEN + 2];
    sprintf(loc_str, "%d%c ", value, cmd[0]);

    if (cmd[1] == 'c')
        strcat(edit->queue_clm, loc_str);
    else if (cmd[1] == 'r')
        strcat(edit->queue_rows, loc_str);
}

/** 
 * Funkce pracuje s teoretickou strukturou Queue (fronta). 
 * Queue funguje na bazi FIFO (first in, first out); odebere prvni 
 * hodnotu z fronty.
 * @param str retezec reprezentujici frontu (oddeluje se znakem mezery)
*/
void dequeue(char *str)
{
    if (str[0] == '\0')
        return;

    int end_index = indexof(str, ' ', 0, 1);
    remove_str(str, 0, end_index + 1, str);
}

/** 
 * Funkce pracuje s teoretickou strukturou Queue (fronta). 
 * Dava moznost uzivateli se podivat na prvni hodnotu ve fronte bez jejiho odebrani.
 * @param str retezec reprezentujici frontu (oddeluje se znakem mezery)
 * @param item_out vraci hodnotu na prvnim miste ve fronte
*/
void peek(char *str, char *item_out)
{
    int end_index = indexof(str, ' ', 0, 1);
    substring(str, 0, end_index, item_out);
}

/* ---------------------------------- */

/** 
 * Funkce provede prikazy na upravu radku podle argumentu. 
 * Vyuziva k tomu teoretickou datovou strukturu Queue (fronta).
 * Pokud se radek shoduje s polozkou ve fronte, funkce provede upravu.
 * Po uprave odstranuje prvni polozku z fronty.
 * @param file ukazatel na datovou strukturu file
 * @param edit ukazatel na datovou strukturu edit
*/
bool edit_rows(File *file, Edit *edit)
{
    char item[INT_MAX_STR_LEN + 2] = ""; // promenna reprezentujici polozku fronty
    char *cmd;                           // znak prikazu pro upravu radku (d/i)
    int line_number;                     // cislo radku, na kterem se maji dit zmeny
    peek(edit->queue_rows, item);
    line_number = strtol(item, &cmd, 10); // ziska hodnotu radku z polozky

    if (file->row_count == line_number) // pokud se jedna o momentalni sloupce
    {
        if (cmd[0] == 'd')
        {
            dequeue(edit->queue_rows);
            return true;
        }

        else // cmd[0] == 'i'
        {
            irow(file, *edit);
            dequeue(edit->queue_rows);
        }
    }
    return false;
}

/** 
 * Funkce provede prikazy pro upravu sloupcu podle argumentu. 
 * Vyuziva k tomu teoretickou datovou strukturu Queue (fronta).
 * Pokud se sloupec shoduje s polozkou ve fronte, funkce provede upravu.
 * Po uprave odstranuje prvni polozku z fronty.
 * @param queue_clm retezec predstavujici strukturu Queue s prikazy pro upravu sloupcu
 * @param acol pocet sloupcu, ktere se maji vlozit nakonec radku
 * @param line_value retezec radku
 * @param file ukazatel na datovou strukturu File
*/
void edit_clms(char *queue_clm, int acol, char *line_value, File *file)
{
    if (queue_clm[0] == '\0' && acol == 0)
        return;

    char item[INT_MAX_STR_LEN + 2] = ""; // promenna reprezentujici polozku fronty
    char *cmd;                           // znak prikazu pro upravu sloupce (d/i)
    int column_number;                   // cislo sloupce, na kterem se maji dit zmeny
    int clm_dif = 0;                     // vzdalenost realneho sloupce od puvodniho
    int clm_count = file->clm_count;     // vstupni pocet sloupcu
    char copy_of_queue[MAX_STR_LENGTH];  // vytvoreni kopie retezce fronty
    strcpy(copy_of_queue, queue_clm);

    peek(copy_of_queue, item);
    column_number = strtol(item, &cmd, 10);
    // vkladani a mazani sloupcu
    for (int i = 1; i <= file->clm_count; i++)
    {
        // peek(copy_of_queue, item);
        // column_number = strtol(item, &cmd, 10);
        if (i == column_number)
        {
            if (cmd[0] == 'd')
            {
                dcol(line_value, i + clm_dif, file->out_delim, &clm_count);
                clm_dif--;
            }
            else // cmd[0] == 'i'
            {
                icol(line_value, i + clm_dif, file->out_delim, &clm_count);
                clm_dif++;
            }

            // pote, co se pouzije polozka z fronty
            // je smazana, a nacte se dalsi polozka
            dequeue(copy_of_queue);
            peek(copy_of_queue, item);
            column_number = strtol(item, &cmd, 10);
        }
    }

    // pridava sloupce na konec radku
    for (int i = 0; i < acol; i++)
        icol(line_value, clm_count + 1, file->out_delim, &clm_count);
}

/**
 * Funkce prida novy radek do souboru. 
 * @param file ukazatel na datovou strukturu File
 * @param edit ukazatel na datovou strukturu Edit
*/
void irow(File *file, Edit edit)
{
    // nejprve se vytvori novy radek, ktery se ulozi do loc_str
    // a upravi podle prikazu pro upravy sloupcu
    char loc_str[MAX_INPUT_LENGTH];
    create_newline(file, loc_str);
    edit_clms(edit.queue_clm, edit.acol, loc_str, file);

    fputs(loc_str, stdout);
}

/** 
 * Funkce doplni zbyvajici pocet radku podle poctu prikazu arow v argumentu.
 * @param file ukazatel na datovou strukturu File
 * @param edit ukazatel na datovou strukturu Edit
 * @param num_of_rows pocet radku, ktere ma pridat na konec souboru
*/
void arow(File *file, Edit edit, int num_of_rows)
{
    // prida znak noveho radku, protoze se soubor ukoncuje s poslednim radkem
    // (ne prazdnym radkem)

    for (int i = 0; i < num_of_rows; i++)
    {
        // pridava novy radek se znakem \n s vyjikou uplne posledniho radku
        // na kterem tento znak ignoruje
        irow(file, edit);
    }
}

/** 
 * Funkce vlozi sloupec do radku. Funkce momentalne prochazi cely soubor.
 * @param line_value obsah nacteneho radku ze souboru (line.cur_value)
 * @param clm cislo znaci, pred kolikaty sloupec se ma vlozit novy sloupec
 * @param delim znak oddelovace v souboru
 * @param clm_count pocet radku v souboru
*/
void icol(char *line_value, int clm, char delim, int *clm_count)
{
    char str_to_insert[2]; /* retezec oddelovace */
    sprintf(str_to_insert, "%c", delim);

    if (clm == 1) // pokud chce uzivatel pridat novy sloupec pred prvni
        insert_str(line_value, 0, str_to_insert, line_value);

    else if (in_range(clm, 2, *clm_count))
    {
        // zacatek nove bunky
        int starting_index = indexof(line_value, delim, 0, clm - 1) + 1;
        insert_str(line_value, starting_index, str_to_insert, line_value);
    }

    // pokud chce uzivatel pridat sloupec za posledni sloupec
    // funkce acol
    else if (clm == *clm_count + 1)
    {
        int start_index, len;
        get_cell_info(line_value, clm - 1, delim, *clm_count, &start_index, &len);
        insert_str(line_value, start_index + len, str_to_insert, line_value);
    }
    (*clm_count)++;
}

/** 
 * Funkce odstranuje sloupec z radku. 
 * @param line_value obsah nacteneho radku ze souboru (line.cur_value)
 * @param clm cislo znaci, pred kolikaty sloupec se ma vlozit novy sloupec
 * @param delim znak oddelovace v souboru
 * @param clm_count pocet radku v souboru
*/
void dcol(char *line_value, int clm, char delim, int *clm_count)
{
    int start_index, len;
    get_cell_info(line_value, clm, delim, *clm_count, &start_index, &len);

    // v pripade, ze radek konci '\n' tak se posledni znak posouva o jedno doleva
    if (clm == *clm_count)
        remove_str(line_value, start_index - 1, len + 1, line_value);
    else
        remove_str(line_value, start_index, len + 1, line_value);
    (*clm_count)--;
}

/* ------------------------------------------------------ */

/** 
 * Funkce provede akci podle nactenych parametru a prikazu ve strukturach.
 * @param sel ukazatel na datovou strukturu Select
 * @param edit ukazatel na datovou strukturu Edit
 * @param file ukazatel na datovou strukturu File
*/
int do_command(Select *sel, Line *line, const File *file)
{
    int error = 0;

    if (!sel->command[0])
        ;

    else if (!strcmp(sel->command, "cset"))
        cset(line, sel->clm_C, file, sel->cmd_str);
    else if (!strcmp(sel->command, "tolower"))
        to_lower(line, sel->clm_C, file);
    else if (!strcmp(sel->command, "toupper"))
        to_upper(line, sel->clm_C, file);
    else if (!strcmp(sel->command, "round"))
        error = round_func(line, sel->clm_C, file);
    else if (!strcmp(sel->command, "int"))
        error = int_func(line, sel->clm_C, file);
    else if (!strcmp(sel->command, "copy"))
        copy(line, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "swap"))
        swap(line, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "move"))
        move(line, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "csum"))
        error = csum(line, sel->clm_C, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "cavg"))
        error = cavg(line, sel->clm_C, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "cmin"))
        error = cmin(line, sel->clm_C, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "cmax"))
        error = cmax(line, sel->clm_C, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "ccount"))
        ccount(line, sel->clm_C, sel->clm_N, sel->clm_M, file);
    else if (!strcmp(sel->command, "cseq"))
        cseq(line, sel->clm_N, sel->clm_M, sel->cmd_str, file);
    else if (!strcmp(sel->command, "rseq"))
        rseq(line, sel->clm_C, sel->cmd_str, file);
    else if (!strcmp(sel->command, "rsum"))
        error = rsum(line, sel->clm_C, sel->row_M, sel->cmd_str, file);
    else if (!strcmp(sel->command, "ravg"))
        error = ravg(line, sel->clm_C, sel->row_N, sel->row_M, sel->cmd_str, file);
    else if (!strcmp(sel->command, "rmin"))
        error = rmin(line, sel->clm_C, sel->row_N, sel->row_M, sel->cmd_str, file);
    else if (!strcmp(sel->command, "rmax"))
        error = rmax(line, sel->clm_C, sel->row_N, sel->row_M, sel->cmd_str, file);
    else if (!strcmp(sel->command, "rcount"))
        rcount(line, sel->clm_C, sel->row_M, sel->cmd_str, file);
    return error;
}

/** 
 * Funkce do bunky column nastavi retezec str_in. 
 * Promenna str_out slouzi jako vystupni retezec pro vytisknuti do souboru.
 * @param line struktura line_t obsahujici informace o momentalnim radku
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
 * @param str_to_insert retezec, ktery bude nastaven v bunce
*/
void cset(Line *line, int clm, const File *file, char *str_to_insert)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->cur_value, clm, file->out_delim, file->clm_count, &start_index, &len);

    // smaze obsah bunky a vlozi novy obsah
    replace_str(line->cur_value, start_index, len, str_to_insert, line->cur_value);
}

/** 
 * Funkce prepise vsechna velka pismena v bunce na mala
 * @param line ukazatel na datovou strukturu Line
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
*/
void to_lower(Line *line, int clm, const File *file)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->cur_value, clm, file->out_delim,
                  file->clm_count, &start_index, &len);

    // projede retezec a prevede velka pismena na mala
    for (int i = start_index; i <= len + start_index; i++)
    {
        line->cur_value[i] = tolower(line->cur_value[i]);
    }
}

/** 
 * Funkce prepise vsechna mala pismena v bunce na velka
 * @param line ukazatel na datovou strukturu Line
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
*/
void to_upper(Line *line, int clm, const File *file)
{
    // dostane pocatecni index retezce v bunce a jeho delku
    int start_index, len;
    get_cell_info(line->cur_value, clm, file->out_delim, file->clm_count, &start_index, &len);

    // projede retezec a prevede mala pismena na velka
    for (int i = start_index; i <= len + start_index; i++)
    {
        line->cur_value[i] = toupper(line->cur_value[i]);
    }
}

/** 
 * Funkce z bunky ziska retezec, ktery prevede 
 * na cislo a zaokrouhli ho.
 * @param line ukazatel na datovou strukturu Line
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int round_func(Line *line, int clm, const File *file)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(line->cur_value, clm, file->out_delim, file->clm_count, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(line->cur_value, clm, file, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_num(loc_str))
        return ERR_NOTNUM;

    // prevede cislo na retezec a vlozi ho do bunky
    sprintf(loc_str, "%d", round_number(loc_str));
    cset(line, clm, file, loc_str);
    return 0;
}

/** 
 * Funkce prevede desetinne cislo na cele cislo a ulozi do bunky.
 * @param line ukazatel na datovou strukturu Line
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int int_func(Line *line, int clm, const File *file)
{
    // ulozeni prvniho indexu a delky retezce bunky
    int start_index, len;
    get_cell_info(line->cur_value, clm, file->out_delim, file->clm_count, &start_index, &len);

    // ulozeni obsahu bunky
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(line->cur_value, clm, file, loc_str);

    // pokud retezec neni cislo, tak to vyhodi chybovou hlasku
    if (!is_num(loc_str))
        return ERR_NOTNUM;

    // prevede cislo na retezec a vlozi ho do bunky
    sprintf(loc_str, "%d", atoi(loc_str));
    cset(line, clm, file, loc_str);
    return 0;
}

/** 
 * Funkce prepise obsah bunky ve clm_M hodnotou ze clm_N 
 * (zkopiruje obsah bunky clm_N do clm_M).
 * @param line ukazatel na datovou strukturu Line
 * @param clm_N sloupec, ze ktereho se kopiruje
 * @param clm_M sloupec, do ktereho se kopiruje
 * @param file ukazatel na datovou strukturu File
*/
void copy(Line *line, int clm_N, int clm_M,
          const File *file)
{
    char temp_str[MAX_INPUT_LENGTH];

    // nacteni hodnoty ve sloupci N
    get_string_in_cell(line->cur_value, clm_N, file, temp_str);

    // nastavime novou hodnotu do bunky M
    cset(line, clm_M, file, temp_str);
}

/** 
 * Funkce vymeni obsahy buneky z clm_M a clm_N.
 * @param line ukazatel na datovou strukturu Line
 * @param clm_N prvni sloupec
 * @param clm_M druhy sloupec
 * @param file ukazatel na datovou strukturu File
*/
void swap(Line *line, int clm_N, int clm_M,
          const File *file)
{
    char temp_str_M[MAX_INPUT_LENGTH];
    char temp_str_N[MAX_INPUT_LENGTH];

    // nacteni hodnoty ve sloupci M do temp_str_M
    get_string_in_cell(line->cur_value, clm_M, file, temp_str_M);

    // nacteni hodnoty ve sloupci N do temp_str_N
    get_string_in_cell(line->cur_value, clm_N, file, temp_str_N);

    // nastavime novou hodnotu do bunky M a N
    cset(line, clm_M, file, temp_str_N);
    cset(line, clm_N, file, temp_str_M);
}

/** 
 * Funkce přesune obsah bunky v clm_N před clm_M.
 * @param line ukazatel na datovou strukturu Line
 * @param clm_N sloupec, ktery se presouva
 * @param clm_M sloupec, pred ktery se ma presunout obsah bunky clm_N
 * @param file ukazatel na datovou strukturu File
*/
void move(Line *line, int clm_N, int clm_M,
          const File *file)
{
    /** 
     * pokud je clm_N nalevo od clm_M
     * clm_N se prohazuje s pravou bunkou
     * dokud nebude hned vedle clm_M
    */
    while (clm_N < clm_M - 1)
    {
        swap(line, clm_N, clm_N + 1, file);
        clm_N++;
    }

    /** 
     * pokud je clm_N napravo od clm_M
     * tak se clm_N se prohazuje s levou bunkou
     * dokud nebude nalevo od clm_M (nebo-li pred clm_M)
    */
    while (clm_N > clm_M)
    {
        swap(line, clm_N, clm_N - 1, file);
        clm_N--;
    }
}

/** 
 * Funkce do bunky v column_C ulozi soucet hodnot bunek na 
 * stejnem radku ve sloupcích clm_N až clm_M včetně.
 * Podminky: N <= M, C nesmí patřit do intervalu <N;M>
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param file ukazatel na datovou strukturu File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int csum(Line *line, int clm_C, int clm_N,
         int clm_M, const File *file)
{
    double sum = 0.0;
    char loc_str[MAX_INPUT_LENGTH];

    // prochazeni vsech bunek v intervalu
    // pokud v nejake bunce nebude cislo, nastane error
    for (int i = clm_N; i <= clm_M && i <= file->clm_count; i++)
    {
        get_string_in_cell(line->cur_value, i, file, loc_str);
        if (!is_num(loc_str))
            return ERR_NOTNUM;
        sum += atof(loc_str);
    }

    // prevod na string a ulozeni hodnoty do bunky sloupce clm_C
    sprintf(loc_str, "%g", sum);
    cset(line, clm_C, file, loc_str);
    return 0;
}

/** 
 * Funkce do bunky v column_C ulozi aritmeticky prumer hodnot bunek na 
 * stejnem radku ve sloupcích clm_N až clm_M včetně.
 * Podminky: N <= M, C nesmí patřit do intervalu <N;M>
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param file ukazatel na datovou strukturu File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int cavg(Line *line, int clm_C, int clm_N,
         int clm_M, const File *file)
{
    double sum = 0.0;
    char loc_str[MAX_INPUT_LENGTH];

    // prochazeni vsech bunek v intervalu
    // pokud v nejake bunce nebude cislo, nastane error
    for (int i = clm_N; i <= clm_M && i <= file->clm_count; i++)
    {
        get_string_in_cell(line->cur_value, i, file, loc_str);
        if (!is_num(loc_str))
            return ERR_NOTNUM;
        sum += atof(loc_str);
    }

    // prevod na string a ulozeni hodnoty do bunky sloupce clm_C
    sprintf(loc_str, "%g", sum / (clm_M - clm_N + 1));
    cset(line, clm_C, file, loc_str);
    return 0;
}

/** 
 * Funkce hleda v intervalu clm_N do clm_M nejnizsi hodnotu a 
 * zapise ji do bunky clm_C. Vse se deje na jednom radku.
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param file ukazatel na datovou strukturu File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int cmin(Line *line, int clm_C, int clm_N, int clm_M, const File *file)
{
    float min;
    char loc_str[MAX_STR_LENGTH] = "";

    // nacteni prvniho cisla
    get_string_in_cell(line->cur_value, clm_N, file, loc_str);
    if (!is_num(loc_str))
        return ERR_NOTNUM;
    min = atof(loc_str);

    // hledani nejnizsiho cisla
    for (int i = clm_N + 1; i <= clm_M && i <= file->clm_count; i++)
    {
        get_string_in_cell(line->cur_value, i, file, loc_str);
        if (!is_num(loc_str))
            return ERR_NOTNUM;
        min = atof(loc_str) < min ? atof(loc_str) : min;
    }
    // prevod na spravny format a zapis do tabulky
    sprintf(loc_str, "%g", min);
    cset(line, clm_C, file, loc_str);
    return 0;
}

/** 
 * Funkce hleda v intervalu clm_N do clm_M nejvyssi hodnotu a 
 * zapise ji do bunky clm_C. Vse se deje na jednom radku.
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param file ukazatel na datovou strukturu File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int cmax(Line *line, int clm_C, int clm_N, int clm_M, const File *file)
{
    float max;
    char loc_str[MAX_STR_LENGTH];

    // nacteni prvniho cisla
    get_string_in_cell(line->cur_value, clm_N, file, loc_str);
    if (!is_num(loc_str))
        return ERR_NOTNUM;
    max = atof(loc_str);

    // hledani nejvetsiho cisla
    for (int i = clm_N + 1; i <= clm_M && i <= file->clm_count; i++)
    {
        get_string_in_cell(line->cur_value, i, file, loc_str);
        if (!is_num(loc_str))
            return ERR_NOTNUM;
        max = atof(loc_str) > max ? atof(loc_str) : max;
    }
    // prevod na spravny format a zapis do tabulky
    sprintf(loc_str, "%g", max);
    cset(line, clm_C, file, loc_str);
    return 0;
}

/** 
 * Funkce hleda v intervalu clm_N do clm_M pocet neprazdnych bunek a 
 * zapise ji do bunky clm_C. Vse se deje na jednom radku.
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param file ukazatel na datovou strukturu File
*/
void ccount(Line *line, int clm_C, int clm_N, int clm_M, const File *file)
{
    int count = 0;
    char loc_str[MAX_STR_LENGTH];
    for (int i = clm_N; i <= clm_M && i <= file->clm_count; i++)
    {
        get_string_in_cell(line->cur_value, i, file, loc_str);
        if (loc_str[0])
            count++;
    }
    sprintf(loc_str, "%d", count);
    cset(line, clm_C, file, loc_str);
}

/** 
 * Funkce do bunek ve clm_N až clm_M vcetne vlozi postupne 
 * rostouci cisla pocinaje hodnotou B.
 * @param line ukazatel na datovou strukturu Line
 * @param clm_N sloupec; dolni hranice intervalu
 * @param clm_M sloupec; horni hranice intervalu
 * @param cmd_str hodnota B ulozena do retezce
 * @param file ukazatel na datovou strukturu File
*/
void cseq(Line *line, int clm_N, int clm_M, char *cmd_str, const File *file)
{
    int number = atoi(cmd_str);
    char loc_str[INT_MAX_STR_LEN + 2];
    sprintf(loc_str, "%d", number);

    for (int i = clm_N; i <= clm_M && i <= file->clm_count; i++)
    {
        cset(line, i, file, loc_str);
        number++;
        sprintf(loc_str, "%d", number);
    }
}

/** 
 * Funkce do bunek ve clm_C na radcich od row_N po row_M vcetne 
 * (vyreseno v main) vlozi postupne rostouci cisla pocinaje hodnotou B.
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupec bunky, do ktereho se zapisznakje
 * @param cmd_str hodnota B ulozena do retezce
 * @param file ukazatel na datovou strukturu File
*/
void rseq(Line *line, int clm_C, char *cmd_str, const File *file)
{
    // pokud je cislo radku v intervalu <row_N; row_M>
    // zapise hodnotu cisla do bunky, inkrementuje o jedna
    // a ulozi do cmd_str
    // if (in_range(file->row_count, row_N, row_M))
    cset(line, clm_C, file, cmd_str);
    int number = atoi(cmd_str);
    number++;
    sprintf(cmd_str, "%d", number);
}

/** 
 * Funkce secte hodnoty ve clm_C od row_N do row_M-1 vcetne (vyreseno v main)
 * a vyslednou hodnotu zapise do [row_M; clm_C]. * @param line ukazatel na datovou strukturu Line
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupec bunky, do ktereho se zapisuje
 * @param row_M radek; horni hranice intervalu (pocita se do row_M-1)
 * @param cmd_str retezec slouzici k ukladani hodnot
 * @param file datova struktura File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int rsum(Line *line, int clm_C, int row_M, char *cmd_str, const File *file)
{
    // inicializace prazdneho retezce
    if (cmd_str[0] == '\0')
        strcpy(cmd_str, "0");

    float sum = atof(cmd_str);

    // zapis do row_M, ktery je o 1 radek vice nez v argumentech
    if (file->row_count == row_M)
        cset(line, clm_C, file, cmd_str);

    // v argumentech se nacita row_M + 1, aby se zapsalo na nasledujici radek
    // samotny prikaz ale probiha jen do row_M
    // proto musime prakaz ukoncit jiz v row_M - 1
    else
    {
        get_string_in_cell(line->cur_value, clm_C, file, cmd_str);
        if (!is_num(cmd_str))
            return ERR_NOTNUM;
        sum += atof(cmd_str);
        sprintf(cmd_str, "%g", sum);
    }
    return 0;
}

/** 
 * Funkce spocita aritmeticky prumer hodnot v bunkach v 
 * clm_C na radcich row_N po row_M-1 a hodnotu zapise 
 * do bunky [row_M; clm_C].
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupec bunky, do ktereho se zapisuje
 * @param row_N radek; dolni hranice intervalu
 * @param row_M radek; horni hranice intervalu (pocita se do row_M-1)
 * @param cmd_str retezec slouzici k ukladani hodnot
 * @param file datova struktura File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int ravg(Line *line, int clm_C, int row_N, int row_M, char *cmd_str, const File *file)
{
    // inicializace cmd_str
    if (cmd_str[0] == '\0')
        strcpy(cmd_str, "0");

    float avrg = atof(cmd_str);

    // vypis hodnoty na do bunky [row_M; clm_C]
    if (file->row_count == row_M)
    {
        avrg /= (row_M - row_N);
        sprintf(cmd_str, "%g", avrg);
        cset(line, clm_C, file, cmd_str);
    }

    // scitani hodnot v bunkach clm_C na radcich row_N po row_M-1
    else
    {
        get_string_in_cell(line->cur_value, clm_C, file, cmd_str);
        if (!is_num(cmd_str))
            return ERR_NOTNUM;
        avrg += atof(cmd_str);
        sprintf(cmd_str, "%g", avrg);
    }
    return 0;
}

/** 
 * Funkce hleda v bunkach clm_C na radcich row_N do row_M-1 nejnizsi hodnotu a 
 * zapise ji do bunky [row_M; clm_C].
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupec bunky, do ktereho se zapisuje
 * @param row_N radek; dolni hranice intervalu
 * @param row_M radek; horni hranice intervalu (pocita se do row_M-1)
 * @param cmd_str retezec slouzici k ukladani hodnot
 * @param file datova struktura File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int rmin(Line *line, int clm_C, int row_N, int row_M, char *cmd_str, const File *file)
{
    // inicializace cmd_str
    if (cmd_str[0] == '\0')
        strcpy(cmd_str, "0");

    float min = atof(cmd_str);

    // nacteni prvniho radku
    if (file->row_count == row_N)
    {
        get_string_in_cell(line->cur_value, clm_C, file, cmd_str);
        if (!is_num(cmd_str))
            return ERR_NOTNUM;
        min = atof(cmd_str);
        sprintf(cmd_str, "%g", min);
    }

    // zapis do bunky [row_M;clm_C]
    else if (file->row_count == row_M)
        cset(line, clm_C, file, cmd_str);

    // hledani nejnizsiho cisla ve sloupci clm_C
    else
    {
        get_string_in_cell(line->cur_value, clm_C, file, cmd_str);
        if (!is_num(cmd_str))
            return ERR_NOTNUM;
        min = atof(cmd_str) < min ? atof(cmd_str) : min;
        sprintf(cmd_str, "%g", min);
    }
    return 0;
}

/** 
 * Funkce hleda v bunkach clm_C na radcich row_N do row_M-1 nejnizsi hodnotu a 
 * zapise ji do bunky [row_M; clm_C].
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupec bunky, do ktereho se zapisuje
 * @param row_N radek; dolni hranice intervalu
 * @param row_M radek; horni hranice intervalu (pocita se do row_M-1)
 * @param cmd_str retezec slouzici k ukladani hodnot
 * @param file datova struktura File
 * @return pokud nastane chyba tak vraci nenulove cislo
*/
int rmax(Line *line, int clm_C, int row_N, int row_M, char *cmd_str, const File *file)
{
    // inicializace cmd_str
    if (cmd_str[0] == '\0')
        strcpy(cmd_str, "0");

    float max = atof(cmd_str);

    // nacteni prvniho radku
    if (file->row_count == row_N)
    {
        get_string_in_cell(line->cur_value, clm_C, file, cmd_str);
        if (!is_num(cmd_str))
            return ERR_NOTNUM;
        max = atof(cmd_str);
        sprintf(cmd_str, "%g", max);
    }

    // zapis do bunky [row_M; clm_C]
    else if (file->row_count == row_M)
        cset(line, clm_C, file, cmd_str);

    // hledani nejvyssiho cisla ve  sloupci clm_C
    else
    {
        get_string_in_cell(line->cur_value, clm_C, file, cmd_str);
        if (!is_num(cmd_str))
            return ERR_NOTNUM;
        max = atof(cmd_str) > max ? atof(cmd_str) : max;
        sprintf(cmd_str, "%g", max);
    }
    return 0;
}

/** 
 * Funkce hleda v bunkach clm_C na radcich row_N do row_M-1 
 * pocet neprazdnych bunek a zapise ji do bunky [row_M; clm_C].
 * @param line ukazatel na datovou strukturu Line
 * @param clm_C sloupce v radku (pocita se od 1)
 * @param row_M radek; horni hranice intervalu (pocita se do row_M-1)
 * @param cmd_str retezec slouzici na ukladani hodnot
 * @param file ukazatel na datovou strukturu File
*/
void rcount(Line *line, int clm_C, int row_M, char *cmd_str, const File *file)
{
    // inicializace cmd_str
    if (cmd_str[0] == '\0')
        strcpy(cmd_str, "0");

    int count = atoi(cmd_str);

    // zapis do bunky [row_M+1; clm_C]
    if (file->row_count == row_M)
        cset(line, clm_C, file, cmd_str);

    // proces pocitani
    else
    {
        get_string_in_cell(line->cur_value, clm_C, file, cmd_str);
        if (cmd_str[0] != '\0')
            count++;
        sprintf(cmd_str, "%d", count);
    }
}

/* ---------------------------------------------- */

/** 
 * Funkce porovnava, zda retezec v bunce v column 
 * zacina retezcem str_to_cmp
 * @param str_in vstupni retezec
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
 * @param str_to_cmp retezec, na se kterym porovnavame obsah bunky
 * @return true pokud retezec v obsahu bunky v column 
 * zacina retezcem str_to_cmp 
*/
bool begins_with(char *str_in, int clm, const File *file, char *str_to_cmp)
{
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(str_in, clm, file, loc_str);

    // cyklus projizdi retezec str_in a str_to_cmp zaroven
    for (int i = 0; str_to_cmp[i]; i++)
    {
        // pokud dochazi k neshode, funkce vraci false
        if (loc_str[i] != str_to_cmp[i])
            return false;
    }
    return true;
}

/** 
 * Funkce kontroluje, zda se retezec str_to_cmp 
 * nachazi v retezci v bunce column.
 * @param str_in vstupni retezec
 * @param clm sloupce v radku (pocita se od 1)
 * @param file ukazatel na datovou strukturu File
 * @param str_to_cmp retezec, na se kterym porovnavame obsah bunky
 * @return true pokud retezec v obsahu bunky v column nachazi 
*/
bool contains(char *str_in, int clm,
              const File *file, char *str_to_cmp)
{
    if (str_to_cmp[0] == '\0')
        return true;
    // nacita obsah bunky do promenne
    char loc_str[MAX_INPUT_LENGTH];
    get_string_in_cell(str_in, clm, file, loc_str);

    int j = 0;
    int len = (int)strlen(str_to_cmp);

    // prohledava retezec
    for (int i = 0; loc_str[i]; i++)
    {
        // pokud je schoda znaku a index j je stale v intervalu str_to_cmp
        // zvetsujeme indexy i a j o 1
        // (vnejsi cyklus ho za normalnich okolnosti neincrementuje)
        // take kontroluje, zda neskoncil retezec loc_str
        while (str_to_cmp[j] && loc_str[i] && loc_str[i] == str_to_cmp[j])
        {
            i++;
            j++;
        }

        // pokud index j == len, nebo-li cyklus prosel vsechny znaky
        // str_to_cmp a vsechny se shoduji, znamena to, ze se opravdu nachazi
        if (j == len)
            return true;

        // resetuje se promenna j na konci kontroly
        j = 0;
    }
    return false;
}

/** 
 * Funkce nastavi parametry prikazu beginswith.
 * @param sel ukazatel na datovou strukturu Select
 * @param argv ukazatel na zacatek pole znaku
 * @param index pozice argumentu beginswith v argumentech (od 0)
* @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int set_beginswith(Select *sel, char **argv, int *index)
{
    if (!is_num(argv[*index + 1]))
        return ERR_WRONG_ARGV;
    sel->begin_clm = atoi(argv[++(*index)]);
    strcpy(sel->beginw_str, argv[++(*index)]);
    return 0;
}

/** 
 * Funkce nastavi parametry prikazu contains
 * @param sel ukazatel na datovou strukturu Select
 * @param argv ukazatel na zacatek pole znaku
 * @param index pozice argumentu contains v argumentech (od 0)
* @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int set_contains(Select *sel, char **argv, int *index)
{
    if (!is_num(argv[*index + 1]))
        return ERR_WRONG_ARGV;
    sel->contain_clm = atoi(argv[++(*index)]);
    strcpy(sel->contain_str, argv[++(*index)]);
    return 0;
}

/** 
 * Funkce nastavi parametry prikazu rows
 * @param sel ukazatel na datovou strukturu Select
 * @param argv ukazatel na zacatek pole znaku
 * @param index pozice argumentu rows v argumentech (od 0)
* @return vraci 0 pro bezchybne zadani argumenty; kladne cislo pro chybove hlaseni
*/
int set_rows(Select *sel, char **argv, int *index)
{
    // kontrola obou argumentu
    // pokud je hodnota argumentu rovna znaku pomlcky
    // je mu sloupci prirazena hodnota TILL_END
    if (!is_num(argv[*index + 1]) && argv[*index + 1][0] != '-')
        return ERR_WRONG_ARGV;
    else if (argv[*index + 1][0] == '-')
    {
        sel->row_N = TILL_END;
        (*index)++;
    }
    else
        sel->row_N = atoi(argv[++(*index)]);

    // plati i pro druhy argument
    // u druheho argumentu se tez resi,
    if (!is_num(argv[*index + 1]) && argv[*index + 1][0] != '-')
        return ERR_WRONG_ARGV;
    else if (argv[*index + 1][0] == '-')
    {
        sel->row_M = TILL_END;
        (*index)++;
    }
    else
        sel->row_M = atoi(argv[++(*index)]);

    if (sel->row_N > sel->row_M)
        return ERR_WRONG_ARGV;
    return 0;
}
