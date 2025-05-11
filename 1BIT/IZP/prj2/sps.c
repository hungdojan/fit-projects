/**
 * Prace s datovymi strukturami
 *
 * Tento program obsahuje zakladni operace tabulkovych procesoru.
 * Vstupem programu jsou textova data tabulky.
 * Vystupnim souborem jsou textova data tabulky upravena 
 * podle serii argumentu na prikazove radce.
 *
 * @author Hung Do
 * @since 21.11.2020
 * @date 06.12.2020
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>

#define DEFAULT_VALUE ""
#define DEFAULT_DELIM " "
#define CMD_DELIM ";"
#define MAX_CMD 1000
#define MAX_LEN 10240
#define NUM_OF_VAR 10
#define NUM_OF_CMDS 16
#define NUM_OF_RANGE_CMD 3

const char *vars_str[] = {
    "_0", "_1",
    "_2",
    "_3",
    "_4",
    "_5",
    "_6",
    "_7",
    "_8",
    "_9"};

typedef struct
{
    int top;
    int left;
} Point;

Point np(int top, int left)
{
    Point p;
    p.top = top;
    p.left = left;
    return p;
}

typedef struct
{
    Point from;
    Point to;
} Range;

// -------------------------

typedef struct cell_t
{
    char *value;
    struct cell_t *nxt;
} Cell;

/**
 * Inicializuje seznam bunek.
 */
Cell *cell_init()
{
    Cell *cell = malloc(sizeof(Cell));
    if (cell != NULL)
        cell->value = NULL;
    return cell;
}

Cell *get_cell(Cell **head, int index)
{
    int i = 0;
    for (Cell *cell = *head; cell != NULL; cell = cell->nxt)
    {
        if (index == i)
            return cell;
        i++;
    }
    fprintf(stderr, "(get_cell): index out of range\n");
    return NULL;
}

/**
 * Nastavuje hodnotu bunky.
 *
 * @param cell ukazatel na polozku bunky
 * @param value hodnota bunky
 */
bool set_value(Cell **cell, char *value)
{
    if (cell != NULL && *cell != NULL && value != NULL)
    {
        void *p = realloc((*cell)->value, sizeof(char) * (strlen(value) + 1));
        if (p != NULL)
        {
            (*cell)->value = p;
            strcpy((*cell)->value, value);
            return true;
        }
    }
    return false;
}

/**
 * Destruktor seznamu bunek.
 *
 * @param cell ukazatel na prvni polozku seznamu bunek.
 */
void cell_dtor(Cell **cell)
{
    if (cell != NULL && *cell != NULL)
    {
        while (*cell != NULL)
        {
            Cell *temp = *cell;
            *cell = (*cell)->nxt;
            if (temp->value != NULL)
                free(temp->value);
            temp->value = NULL;
            free(temp);
        }
    }
}

/**
 * Vlozi novou polozku pred prvni polozku.
 *
 * @param head ukazatel na prvni polozku bunky
 * @return true pokud se povedlo zainicializovat polozku
 */
Cell *insert_cell_first(Cell **head, char *value)
{
    if (head != NULL)
    {
        Cell *cell = cell_init();
        if (cell != NULL)
        {
            set_value(&cell, value);
            cell->nxt = *head;
            *head = cell;
            return cell;
        }
        fprintf(stderr, "not enough memory\n");
    }
    return NULL;
}

/**
 * Vlozi novou polozku za vybranou polozku.
 *
 * @param head ukazatel na prvni polozku bunky
 * @param index poradi polozky (indexovani od 0)
 * @return true pokud se povedlo zainicializovat polozku
 */
Cell *insert_cell_behind(Cell **head, int index, char *value)
{
    if (head != NULL && *head != NULL)
    {
        Cell *cell = cell_init();
        if (cell != NULL)
        {
            set_value(&cell, value);
            int count = 0;
            for (Cell *temp = *head; temp != NULL; temp = temp->nxt)
            {
                if (count == index)
                {
                    cell->nxt = temp->nxt;
                    temp->nxt = cell;
                    return cell;
                }
                count++;
            }
            cell_dtor(&cell);
            fprintf(stderr, "(insert_cell_behind): index out of range\n");
            return NULL;
        }
        fprintf(stderr, "not enough memory\n");
    }
    return NULL;
}

/**
 * Smaze bunku ze seznamu.
 *
 * @param head ukazatel na prvni polozku
 * @param index poradi polozky
 * @return true pokud se povedlo najit a odstranit polozku
 */
bool remove_cell(Cell **head, int index)
{
    if (head != NULL && *head != NULL)
    {
        int count = 0;
        Cell *prv = NULL;
        for (Cell *temp = *head; temp != NULL; temp = temp->nxt)
        {
            if (count == index)
            {
                if (prv == NULL) // odstraneni prvni polozky
                {
                    *head = (*head)->nxt;
                    free(temp->value);
                    free(temp);
                    return true;
                }
                else // odstraneni indexove polozky
                {
                    prv->nxt = temp->nxt;
                    free(temp->value);
                    free(temp);
                    return true;
                }
            }
            count++;
            prv = temp;
        }
        fprintf(stderr, "(remove_cell): index out of range.\n");
    }
    return false;
}

// --------------------------

typedef struct line_t
{
    int length;
    Cell *head;
    struct line_t *nxt;
} Line;

/**
 * Inicializuje seznam radku.
 */
Line *line_init()
{
    Line *line = malloc(sizeof(Line));
    if (line != NULL)
    {
        line->length = 0;
        line->head = NULL;
    }
    return line;
}

/**
 * Destruktor seznamu radku.
 *
 * @param line ukazatel na prvni polozku seznamu
 */
void line_dtor(Line **line)
{
    if (line != NULL && *line != NULL)
    {
        while (*line != NULL)
        {
            Line *temp = *line;
            *line = (*line)->nxt;
            cell_dtor(&temp->head);
            free(temp);
        }
    }
}

Line *get_line(Line **head, int index)
{
    int i = 0;
    for (Line *line = *head; line != NULL; line = line->nxt)
    {
        if (index == i)
            return line;
        i++;
    }
    fprintf(stderr, "(get_line): index out of range\n");
    return NULL;
}

/**
 * Prida novou polozku na na prvni misto seznamu radku.
 *
 * @param head ukazatel na prvni polozku
 * @return ukazatel na nove vytvorenou polozku
 */
Line *insert_line_first(Line **head)
{
    if (head != NULL)
    {
        Line *line = line_init();
        if (line != NULL)
        {
            line->nxt = *head;
            *head = line;
            return line;
        }
        else
            fprintf(stderr, "not enough memory\n");
    }
    return NULL;
}

/**
 * prida novy radek za vybranou polozku.
 *
 * @param head ukazatel na prvni polozku seznamu radku
 * @param index poradi radku
 * @return ukazatel na nove vytvorenou polozku
 */
Line *insert_line_behind(Line **head, int index)
{
    if (head != NULL && *head != NULL)
    {
        Line *line = line_init();
        if (line != NULL)
        {
            int count = 0;
            for (Line *temp = *head; temp != NULL; temp = temp->nxt)
            {
                if (count == index)
                {
                    line->nxt = temp->nxt;
                    temp->nxt = line;
                    return line;
                }
                count++;
            }
            line_dtor(&line);
            fprintf(stderr, "(insert_cell_behind): index out of range\n");
            return NULL;
        }
        fprintf(stderr, "not enough memory\n");
    }
    return NULL;
}

/**
 * Odstrani vybrany radek.
 * 
 * @param head ukazatel na prvni polozku
 * @param index poradi polozky
 * @return true pokud se podarilo uspesne odstranit polozka
 */
bool remove_line(Line **head, int index)
{
    if (head != NULL && *head != NULL)
    {
        int count = 0;
        Line *prv = NULL;
        for (Line *temp = *head; temp != NULL; temp = temp->nxt)
        {
            if (count == index)
            {
                if (prv == NULL) // odstraneni prvni polozky
                {
                    *head = (*head)->nxt;
                    cell_dtor(&temp->head);
                    free(temp);
                    return true;
                }
                else // odstraneni indexove polozky
                {
                    prv->nxt = temp->nxt;
                    cell_dtor(&temp->head);
                    free(temp);
                    return true;
                }
            }
            count++;
            prv = temp;
            ;
        }
        fprintf(stderr, "(remove_cell): index out of range.\n");
    }
    return false;
}

// -------------------------

typedef struct table_t
{
    int rows;
    int cols;
    char *delims;
    char *file_path;
    Line *head;
} Table;

/**
 * Inicializuje strukturu Table
 *
 * @param table ukazatel na hodnotu Table
 */
void table_init(Table *table)
{
    table->rows = 0;
    table->cols = 0;
    table->delims = NULL;
    table->file_path = NULL;
    table->head = NULL;
}

bool set_data(Table *table, Point p, char *value)
{
    if (table != NULL)
    {
        int line_count = 0;
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            if (line_count == p.top)
            {
                int cell_count = 0;
                for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
                {
                    if (cell_count == p.left)
                        return set_value(&cell, value);
                    cell_count++;
                }
            }
            line_count++;
        }
        fprintf(stderr, "(set_data): index out of range\n");
    }
    return false;
}

/**
 * Vraci retezec na souradnicich podle Point
 *
 * @param table ukazatel na tabulkovou strukturu
 * @param p struktura ukazatele
 * @return ukazatel na hodnotu retezce ve vybrane bunce
 */
char *get_data(Table *table, Point p)
{
    if (table != NULL)
    {
        int line_count = 0;
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            if (line_count == p.top)
            {
                int cell_count = 0;
                for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
                {
                    if (cell_count == p.left)
                        return cell->value;
                    cell_count++;
                }
            }
            line_count++;
        }
        fprintf(stderr, "(get_data): index out of range\n");
    }
    return NULL;
}

/**
 * Destruktor struktury Table
 *
 * @param table ukazatel na tabulkovou strukturu
 */
void table_dtor(Table *table)
{
    if (table != NULL)
    {
        line_dtor(&table->head);
        free(table->delims);
        free(table->file_path);
        table = NULL;
    }
}

// -------------------------

typedef struct cmd_t
{
    int size;         // pocet polozek v seznamu
    int index;        // momentalni index polozky v seznamu prikazu
    char cmd_name[6]; // nazev prikazu
    char *cmd_args;   // argument prikazu
    Point curr_point; // udava momentalni souradnice bunky
    Point dest_point; // v pripade, ze je potreba druha hodnota pointu (napr. pro zapis)
    Range coords;     // udava krajni body okna
    Range temp_range; // docasna promenna vyberu bunel (prikazy [set] a [_])
    Cell *curr;       // momentalni prikaz
    Cell *head;       // seznam prikazu
    char **vars;      // pole docasnych promennych
} Cmd;

/**
 * Inicializuje strukturu prikazu.
 *
 * @param ukazatel na strukturu prikazu
 */
void cmd_init(Cmd *cmd)
{
    cmd->size = 0;
    cmd->index = 0;

    cmd->curr = NULL;
    cmd->head = NULL;

    cmd->coords.from = np(0, 0);
    cmd->coords.to = np(0, 0);
    cmd->dest_point = np(0, 0);
    cmd->curr_point = cmd->coords.from;

    cmd->cmd_args = NULL;
    strcpy(cmd->cmd_name, "");
    cmd->vars = malloc(sizeof(char *) * NUM_OF_VAR);
    if (cmd->vars != NULL)
    {
        for (int i = 0; i < NUM_OF_VAR; i++)
            cmd->vars[i] = NULL;
    }
}

/**
 * Destruktor struktury prikazu.
 *
 * @param ukazatel na strukturu prikazu
 */
void cmd_dtor(Cmd *cmd)
{
    if (cmd != NULL)
    {
        cell_dtor(&cmd->head);
        if (cmd->cmd_args != NULL)
            free(cmd->cmd_args);
        if (cmd->vars != NULL)
        {
            for (int i = NUM_OF_VAR - 1; i >= 0; i--)
                if (cmd->vars[i] != NULL)
                    free(cmd->vars[i]);
            free(cmd->vars);
        }
        cmd = NULL;
    }
}

/**
 * Nastavi hodnotu cmd_args.
 *
 * @param cmd ukazatel na strukturu prikazu
 * @param value novy retezec
 * @return false pokud se nepovedlo provest akce
 */
bool set_cmd_args(Cmd *cmd, const char *value)
{
    if (cmd != NULL)
    {
        void *p = realloc(cmd->cmd_args, sizeof(char) * (strlen(value) + 1));
        if (p != NULL)
        {
            cmd->cmd_args = p;
            strcpy(cmd->cmd_args, value);
            return true;
        }
    }
    return false;
}

// ------------------------

/**
 * Kontroluje, zda se znak nachazi v retezci.
 *
 * @param c hledany znak
 * @param str retezec
 * @return true pokud se znak objevuje v retezci alespon jednou
 */
bool contains_char(char c, char *str)
{
    for (int i = 0; str[i]; i++)
        if (str[i] == c)
            return true;
    return false;
}

/** 
 * Funkce kontroluje, zda se retezec str_to_cmp 
 * nachazi v retezci v bunce column.
 * @param src retezec, na se kterym porovnavame obsah bunky
 * @param searched vstupni retezec
 * @return true pokud retezec v obsahu bunky v column nachazi 
*/
bool contains_str(char *src, char *searched)
{
    int j = 0;

    // prohledava retezec
    for (int i = 0; src[i]; i++)
    {
        // pokud je schoda znaku a index j je stale v intervalu str_to_cmp
        // zvetsujeme indexy i a j o 1
        // (vnejsi cyklus ho za normalnich okolnosti neincrementuje)
        // take kontroluje, zda neskoncil retezec loc_str
        while (src[i] && searched[j] && searched[j] == src[i])
        {
            i++;
            j++;
        }

        // pokud index j == len, nebo-li cyklus prosel vsechny znaky
        // str_to_cmp a vsechny se shoduji, znamena to, ze se opravdu nachazi
        if (!searched[j])
            break;

        // resetuje se promenna j na konci kontroly
        j = 0;
    }
    return !searched[j];
}

/**
 * Validace retezce. V retezci se mohou nachazet pouze 2 uvozvky, a to 
 * na konci a na zacatku retezce. Specialni znaky se mohou objevit jedine 
 * za escape sequence.
 *
 * @param str hodnota retezce
 * @return spravnost formatu retezce
 */
bool str_valid(char *str)
{
    // unsigned int quote_mark = 0;
    unsigned int start = 0;
    unsigned int len = strlen(str);
    if (str[0] == '"' && str[len - 1] == '"')
    {
        start++;
        len--;
    }
    for (unsigned int i = start; i < len; i++)
    {
        if (str[i] == '\\' && str[i + 1])
        {
            i++;
            continue;
        }
        else if (str[i] == '"')
            return false;
    }
    // povolene jsou jen 0 nebo 2 uvozovky vyjma tech za escape sequence
    // return quote_mark == 0 || quote_mark == 2;
    return true;
}

/**
 * Odstranuje specialni znaky. 
 * Vraci ukazatel na nove vytvoreny (alokovany) retezec.
 */
char *str_edit(char *str)
{
    // char str_out[strlen(str) + 1] = "\0";
    char *str_out = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(str_out, "");
    int i = 0;
    int j = 0;
    for (; str[i]; i++)
    {
        if (str[i] == '"') // preskakeji prvni a posledni znak uvozovek
            continue;
        else if (str[i] == '\\')
        {
            // pro \\ a \" prida oba znaky
            if (str[i + 1] == '\\' || str[i + 1] == '"')
            {
                str_out[j++] = str[i++];
            }
            else // jinak znak lomitka preskakuje
                continue;
        }
        str_out[j] = str[i];
        j++;
    }
    str_out[j] = '\0'; // znak ukoncovani retezce
    return str_out;
}

/**
 * Usekne cast retezce v retezci.
 *
 * @param str puvodni hodnota
 * @param start_index pozice prvniho znaku useknuteho retezce
 * @param length delka useknuteho retezce; pokud bude presahovat, vraci az konec
 * @return ukazatel na zaalokovany retezec s nastavenou hodnotou; NULL pokud se nepovede
 */
char *substring(char *str, unsigned int start_index, unsigned int length)
{
    char *str_out = malloc(sizeof(char) * (length + 1));
    if (str_out != NULL)
    {
        for (unsigned int i = 0; i < length && i + start_index < strlen(str); i++)
            str_out[i] = str[start_index + i];
    }
    str_out[length] = '\0';
    if (!str_valid(str_out))
    {
        fprintf(stderr, "(str_to_list): wrong string format\n");
        free(str_out);
        str_out = NULL;
    }
    // prirazeni nove hodnoty (upravene)
    char *s = str_edit(str_out);
    free(str_out);
    str_out = NULL;
    str_out = s;
    return str_out;
}

/**
 * Rozdeli retezec do seznamu bunek (seznam retezcu).
 *
 * @param str samotny retezec
 * @param delim pole znaku oddelovacu
 * @param cols ukazatel na pocet sloupcu
 * @param end_char ukoncovaci znak
 * @return ukazatel na prvni polozku seznam bunek; NULL pokud nastane chyba
 */
Cell *str_to_list(char *str, char *delim, int *cols)
{
    Cell *head = NULL;
    char is_string = false;
    unsigned int start = 0;
    unsigned int i = 0;
    int cols_count = 0;
    for (; str[i]; i++)
    {
        if (str[i] == '\\') // escape sequence ignoruje dalsi znak
        {
            i++;
            continue;
        }
        else if (is_string) // pokud jsou uvozovky, funkce hleda ukoncovaci uvozovku
        {
            if (str[i] == '"')
                is_string = !is_string;
            else
                continue;
        }
        else if (str[i] == '"') // zapina rezim stringu (uvozovky)
            is_string = !is_string;
        else if (contains_char(str[i], delim) || str[i] == '\n') // pokud nasel znak oddelovace
        {
            // vytvori novy string
            char *new_value = substring(str, start, i - start);
            if (new_value == NULL)
            {
                cell_dtor(&head);
                return NULL;
            }
            // vlozi novou bunku
            Cell *temp = NULL;
            if (head == NULL)
                temp = insert_cell_first(&head, NULL);
            else
                temp = insert_cell_behind(&head, cols_count - 1, NULL);

            if (temp == NULL)
            {
                cell_dtor(&head);
                return NULL;
            }
            // priradi ukazatel na vytvoreny retezec
            // posune ukazatel na prvni znak noveho retezce
            set_value(&temp, new_value);
            free(new_value);
            start = i + 1;
            cols_count++;
        }
    }
    if (!is_string && start != i)
    {
        // vytvori novy string
        char *new_value = substring(str, start, i - start);
        if (new_value == NULL)
        {
            cell_dtor(&head);
            return NULL;
        }
        // vlozi novou bunku
        Cell *temp = NULL;
        if (head == NULL)
            temp = insert_cell_first(&head, NULL);
        else
            temp = insert_cell_behind(&head, cols_count - 1, NULL);

        if (temp == NULL)
        {
            cell_dtor(&head);
            return NULL;
        }
        // priradi ukazatel na vytvoreny retezec
        // posune ukazatel na prvni znak noveho retezce
        temp->value = new_value;
        start = i;
        cols_count++;
    }
    if (cols != NULL)
        *cols = cols_count;
    if (start != i)
        fprintf(stderr, "(str_to_list): wrong string format\n");
    return head;
}

/** 
 * Funkce zjisti, zda je retezec cislo
 * 
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
            case '.':
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

// ------------------------

/**
 * Nacte argumenty.
 *
 * @param argc pocet argumentu
 * @param argv pole argumentu
 * @param table ukazatel na Table
 * @param cmd ukazatel na Cmd
 * @return true pokud probehlo bezchybne zpracovani
 */
bool load_args(int argc, char *argv[], Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        bool delims = false;
        bool file = false;
        bool cmd_args = false;
        for (int i = 1; i < argc; i++)
        {
            // nacte znaky oddelovacu z argumentu
            if (!strcmp(argv[i], "-d") && argc > i + 1)
            {
                table->delims = malloc(sizeof(char) * (strlen(argv[i + 1]) + 1));
                if (table->delims == NULL)
                    return false;
                strcpy(table->delims, argv[i + 1]);
                i++;
                delims = true;
            }
            else if (!cmd_args) // nacte prikazy
            {
                if (strlen(argv[i]) == 0)
                    return false;
                cmd->head = str_to_list(argv[i], CMD_DELIM, &cmd->size);
                if (cmd->head == NULL)
                    return false;
                cmd->curr = cmd->head;
                cmd_args = true;
            }
            else if (!file) // nacte cestu k souboru
            {
                FILE *f = fopen(argv[i], "r");
                if (f == NULL)
                {
                    fprintf(stderr, "File not found!\n");
                    return false;
                }
                fclose(f);
                table->file_path = malloc(sizeof(char) * (strlen(argv[i]) + 1));
                if (table->file_path == NULL)
                    return false;
                strcpy(table->file_path, argv[i]);
                file = true;
            }
            else
                return false;
        }
        if (!delims) // nastavi vychozi hodnotu delimu
        {
            table->delims = malloc(sizeof(char) * (strlen(DEFAULT_DELIM) + 1));
            if (table->delims == NULL)
                return false;
            strcpy(table->delims, DEFAULT_DELIM);
        }
        return file && cmd_args;
    }
    return false;
}

/**
 * Nacte obsah souboru do struktury Table
 *
 * @param table ukazatel na strukturu Table
 * @param file_path retezec cesty k souboru
 * @return true pokud se podarilo uspesne nacist do souboru
 */
bool load_file(Table *table, char *file_path)
{
    if (table != NULL)
    {
        FILE *f = fopen(file_path, "r");
        if (f == NULL) // soubor nenalezen
        {
            fprintf(stderr, "File not found.\n");
            return false;
        }

        char str_in[MAX_LEN];
        while ((fgets(str_in, MAX_LEN, f)) != NULL)
        {
            int cols = 0;
            Cell *head = str_to_list(str_in, table->delims, &cols);
            if (head == NULL)
            {
                fclose(f);
                return false;
            }
            // vytvari novy radek new line
            Line *line = NULL;
            if (table->head == NULL)
                line = insert_line_first(&table->head);
            else
                line = insert_line_behind(&table->head, table->rows - 1);
            if (line == NULL)
            {
                cell_dtor(&head);
                fclose(f);
                return false;
            }
            else
                table->rows++;
            line->head = head;
            table->cols = table->cols < cols ? cols : table->cols;
        }
        fclose(f);
        return true;
    }
    return false;
}

/**
 * Zvetsi a zarovna tabulku tak, aby souradnice lezely v tabulce. 
 * Pokud je tabulka vetsi nez zadane souradnice (souradnice jsou in range), 
 * nechava tabulku v puvodnim stavu.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param p souradnice bodu
 * @return vraci false pokud se nepodarilo zaalokovat misto v pameti
 */
bool fill_table(Table *table, Point p)
{
    if (table != NULL)
    {
        int row_count = 0;
        int col_count = 0;
        // pridani sloupcu na existujici radky
        Line *line = table->head;
        for (; line != NULL; line = line->nxt)
        {
            col_count = 0;
            Cell *cell = line->head;
            for (; cell != NULL; cell = cell->nxt)
                col_count++;
            // prida sloupce
            while (col_count < p.left + 1)
            {
                if (col_count == 0)
                    cell = insert_cell_first(&line->head, DEFAULT_VALUE);
                else
                    cell = insert_cell_behind(&line->head, col_count - 1, DEFAULT_VALUE);
                if (cell == NULL)
                    return false;
                col_count++;
            }
            row_count++;
        }
        // pridani novych radku
        while (row_count < p.top + 1)
        {
            if (row_count == 0 && table->head == NULL)
                line = insert_line_first(&table->head);
            else
                line = insert_line_behind(&table->head, row_count - 1);
            if (line == NULL)
                return false;
            int count = 0;
            // prida sloupce
            while (count <= p.left)
            {
                Cell *cell = NULL;
                if (line->head == NULL)
                    cell = insert_cell_first(&line->head, DEFAULT_VALUE);
                else
                    cell = insert_cell_behind(&line->head, count - 1, DEFAULT_VALUE);
                if (cell == NULL)
                    return false;
                count++;
            }
            row_count++;
        }
        // prepis extremu
        table->rows = table->rows <= p.top + 1 ? p.top + 1 : table->rows;
        table->cols = table->cols <= p.left + 1 ? p.left + 1 : table->cols;
    }
    return true;
}

/**
 * Vypise obsah do souboru.
 *
 * @param table ukazatel na strukturu Table
 * @param file_path retezec cesty k souboru
 */
void write_out(Table *table, char *file_path)
{
    if (table != NULL)
    {
        FILE *f = fopen(file_path, "w");
        // projde radku
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            // projde sloupce
            for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
            {
                // vypise hodnotu a delim
                // pokud se v retezci nachazi znak oddelovace
                // prida na zacatku a na konci uvozovky
                if (contains_char(table->delims[0], cell->value))
                    fputc('"', f);

                fprintf(f, "%s", cell->value);

                if (contains_char(table->delims[0], cell->value))
                    fputc('"', f);

                if (cell->nxt != NULL)
                    fputc(table->delims[0], f);
            }
            // prida znak konce radku na konci radku
            fputc('\n', f);
        }
        fclose(f);
    }
}

/**
 * Uvolnuje pamet a vraci konstantu pro (ne)uspesne ukonceni.
 * 
 * @param t ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @param exit_value konstanta (ne)uspechu
 * @return hodnotu promenne exit_value
 */
int to_exit(Table *t, Cmd *cmd, int exit_value)
{
    table_dtor(t);
    cmd_dtor(cmd);
    return exit_value;
}

//-----------------------------------

/**
 * Prida radek nad oznacenou bunkou.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nedokazala provest nejaka akce
 */
bool irow(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        Line *line = NULL;
        if (cmd->curr_point.top == 0)
            line = insert_line_first(&table->head);
        else
            line = insert_line_behind(&table->head, cmd->curr_point.top - 1);
        if (line == NULL)
            return false;
        Cell *cell = NULL;
        // prida sloupce
        for (int count = 0; count < table->cols; count++)
        {
            if (line->head == NULL)
                cell = insert_cell_first(&line->head, DEFAULT_VALUE);
            else
                cell = insert_cell_behind(&line->head, count - 1, DEFAULT_VALUE);
            if (cell == NULL)
                return false;
        }
        table->rows++;
    }
    return true;
}

/**
 * Prida radek pod oznacenou bunku.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool arow(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        Line *line = NULL;
        if (cmd->curr_point.top == 0 && table->head == NULL)
            line = insert_line_first(&table->head);
        else
            line = insert_line_behind(&table->head, cmd->curr_point.top);
        if (line == NULL)
            return false;
        Cell *cell = NULL;
        // prida sloupce
        for (int count = 0; count < table->cols; count++)
        {
            if (line->head == NULL)
                cell = insert_cell_first(&line->head, DEFAULT_VALUE);
            else
                cell = insert_cell_behind(&line->head, count - 1, DEFAULT_VALUE);
            if (cell == NULL)
                return false;
        }
        table->rows++;
    }
    return true;
}

/**
 * Odstrani radek s oznacenou bunkou.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool drow(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {

        if (cmd->curr_point.top >= table->rows)
        {
            fprintf(stderr, "(drow): index out of range\n");
            return false;
        }
        remove_line(&table->head, cmd->curr_point.top);
        table->rows--;
    }
    return true;
}

/**
 * Prida sloupec nalevo od oznacene bunky.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool icol(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {

        if (table->head == NULL)
            insert_line_first(&table->head);
        // prida radek
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            Cell *cell = NULL;
            if (cmd->curr_point.left == 0)
                cell = insert_cell_first(&line->head, DEFAULT_VALUE);
            else
                cell = insert_cell_behind(&line->head, cmd->curr_point.left - 1, DEFAULT_VALUE);
            if (cell == NULL)
                return false;
        }
        table->cols++;
    }
    return true;
}

/**
 * Prida sloupec nepravo od oznacene bunky.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool acol(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {

        if (table->head == NULL)
            insert_line_first(&table->head);
        // prida radek
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            Cell *cell = NULL;
            if (cmd->curr_point.left == 0 && line->head == NULL)
                cell = insert_cell_first(&line->head, DEFAULT_VALUE);
            else
                cell = insert_cell_behind(&line->head, cmd->curr_point.left, DEFAULT_VALUE);
            if (cell == NULL)
                return false;
        }
        table->cols++;
    }
    return true;
}

/**
 * Odstrani sloupec s oznacenou bunkou.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool dcol(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        if (cmd->curr_point.left >= table->cols)
        {
            fprintf(stderr, "(dcol): index out of range\n");
            return false;
        }
        for (Line *line = table->head; line != NULL; line = line->nxt)
            remove_cell(&line->head, cmd->curr_point.left);
        table->cols--;
    }

    return true;
}

/**
 * Nastavi retezec do oznacene bunky.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool set(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        // for-loop
        return set_data(table, cmd->curr_point, cmd->cmd_args);
    }
    return true;
}

/**
 * Nastavuje prazdne bunky ve vybranych bunek.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool clear(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        cmd->cmd_args = realloc(cmd->cmd_args, sizeof(char) * 1);
        strcpy(cmd->cmd_args, "");
        set(table, cmd);
    }
    return true;
}

/**
 * Vymeni hodnoty bunek mezi sebou.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 */
bool swap(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        Line *line1 = NULL;
        Line *line2 = NULL;
        Cell *cell1 = NULL;
        Cell *cell2 = NULL;
        char *temp;

        // nacte obe bunky
        if ((line1 = get_line(&table->head, cmd->curr_point.top)) == NULL ||
            (line2 = get_line(&table->head, cmd->dest_point.top)) == NULL ||
            (cell1 = get_cell(&line1->head, cmd->curr_point.left)) == NULL ||
            (cell2 = get_cell(&line2->head, cmd->dest_point.left)) == NULL)
            return false;

        // prohodi ukazatele na hodnoty
        temp = cell1->value;
        cell1->value = cell2->value;
        cell2->value = temp;
    }
    return true;
}

/**
 * Secte hodnot bunek s ciselnou hodnotou ve vybranem rozsahu a 
 * vysledek zapise do bunky se souradnicemi Cmd::curr_point.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool sum(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        double sum = 0;
        int row = 0;
        int col = 0;
        // prochazi radky
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            col = 0;
            if (row >= cmd->coords.from.top && row <= cmd->coords.to.top)
            {
                // prochazi bunky
                for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
                {
                    if (col >= cmd->coords.from.left && col <= cmd->coords.to.left)
                    {
                        if (is_num(cell->value))
                            sum += strtod(cell->value, NULL);
                    }
                    col++;
                }
            }
            row++;
        }
        // zapis do bunky
        char output[20];
        sprintf(output, "%g", sum);
        set_cmd_args(cmd, output);
        Point temp = cmd->curr_point;
        cmd->curr_point = cmd->dest_point;
        set(table, cmd);
        cmd->curr_point = temp;
    }
    return true;
}

/**
 * Vypocita prumer hodnot bunek s ciselnou hodnotou ve vybranem rozsahu a 
 * vysledek zapise do bunky se souradnicemi Cmd::curr_point.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool avg(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        double sum = 0;
        int count = 0;
        int row = 0;
        int col = 0;
        // prochazi radky
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            col = 0;
            if (row >= cmd->coords.from.top && row <= cmd->coords.to.top)
            {
                // prochazi bunky
                for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
                {
                    if (col >= cmd->coords.from.left && col <= cmd->coords.to.left)
                    {
                        if (is_num(cell->value))
                        {
                            sum += strtod(cell->value, NULL);
                            count++;
                        }
                    }
                    col++;
                }
            }
            row++;
        }
        // zapis do bunky
        char output[20];
        sprintf(output, "%g", sum / count);
        set_cmd_args(cmd, output);
        Point temp = cmd->curr_point;
        cmd->curr_point = cmd->dest_point;
        set(table, cmd);
        cmd->curr_point = temp;
    }
    return true;
}

/**
 * Spocita neprazdne bunky ve vybranem rozsahu a vysledek zapise 
 * do bunky se souradnicemi Cmd::curr_point.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool count(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        int count = 0;
        int row = 0;
        int col = 0;
        // prochazi radek
        for (Line *line = table->head; line != NULL; line = line->nxt)
        {
            col = 0;
            if (row >= cmd->coords.from.top && row <= cmd->coords.to.top)
            {
                // prochazi sloupec
                for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
                {
                    if (col >= cmd->coords.from.left && col <= cmd->coords.to.left)
                        if (strlen(cell->value) > 0)
                            count++;
                    col++;
                }
            }
            row++;
        }
        // zapis do bunky
        char output[20];
        sprintf(output, "%d", count);
        set_cmd_args(cmd, output);
        set(table, cmd);
    }
    return true;
}

/**
 * Nastavuje prazdne bunky ve vybranych bunek.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool len(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        Line *line = get_line(&table->head, cmd->curr_point.top);
        if (line == NULL)
            return false;
        Cell *cell = get_cell(&line->head, cmd->curr_point.left);
        if (cell == NULL || cell->value == NULL)
            return false;

        int len = 0;
        // vynechavani specialnich znaku
        for (int i = 0; cell->value[i]; i++)
        {
            if (cell->value[i] == '"')
                continue;
            else if (cell->value[i] == '\\' && cell->value[i + 1])
            {
                len++;
                i++;
                continue;
            }
            len++;
        }

        // zapis do bunky
        char output[20];
        sprintf(output, "%d", len);
        set_cmd_args(cmd, output);
        cmd->curr_point = cmd->dest_point;
        set(table, cmd);
    }
    return true;
}

/**
 * Nastavuje prazdne bunky ve vybranych bunek.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool def(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        int index = -1;
        for (int i = 0; i < NUM_OF_VAR; i++)
        {
            if (!strcmp(cmd->cmd_args, vars_str[i]))
            {
                index = i;
                break;
            }
        }
        if (index == -1) // nenasel prikaz
            return false;

        char *data = get_data(table, cmd->curr_point);
        if (data == NULL)
            return true;
        void *p = realloc(cmd->vars[index], sizeof(char) * (strlen(data) + 1));
        if (p == NULL)
        {
            fprintf(stderr, "Not enough memory\n");
            return false;
        }
        cmd->vars[index] = p;
        strcpy(cmd->vars[index], data);
    }
    return true;
}

/**
 * Nastavuje prazdne bunky ve vybranych bunek.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool use(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        int index = -1;
        for (int i = 0; i < NUM_OF_VAR; i++)
        {
            if (!strcmp(cmd->cmd_args, vars_str[i]))
            {
                index = i;
                break;
            }
        }
        if (index == -1) // nenasel prikaz
            return false;

        set_cmd_args(cmd, cmd->vars[index]);
        set(table, cmd);
        set_cmd_args(cmd, vars_str[index]);
    }
    return true;
}

/**
 * Nastavuje prazdne bunky ve vybranych bunek.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool inc(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        int index = -1;
        for (int i = 0; i < NUM_OF_VAR; i++)
        {
            if (!strcmp(cmd->cmd_args, vars_str[i]))
            {
                index = i;
                break;
            }
        }
        if (index == -1) // nenasel prikaz
            return false;

        if (is_num(cmd->vars[index]))
        {
            char output[20];
            sprintf(output, "%g", strtod(cmd->vars[index], NULL) + 1);
            void *p = realloc(cmd->vars[index], sizeof(char) * (strlen(output) + 1));
            if (p == NULL)
            {
                fprintf(stderr, "Not enough memory\n");
                return false;
            }
            cmd->vars[index] = p;
            strcpy(cmd->vars[index], output);
        }
    }
    return true;
}

char *cmd_names[NUM_OF_CMDS] = {
        "irow", "arow", "drow", "icol", "acol", "dcol", "set", "clear",
        "swap", "sum", "avg", "count" "len", "def", "use", "inc"
};

bool (*cmds_arr[NUM_OF_CMDS])(Table *table, Cmd *cmd) = {
        irow, arow, drow, icol, acol, dcol, set, clear,
        swap, sum, avg, count, len, def, use, inc
};

/**
 * Najde souradnice bunky s nejnizsi ciselnou hodnotou. Pokud nic nenajde, 
 * nastavuje hodnotu Cmd::coords::from jako prvni bunku v rozsahu.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool min(Table *table, Cmd *cmd)
{
    int row = 0;
    int col = 0;
    double min = DBL_MAX;
    cmd->dest_point = np(0, 0);
    for (Line *line = table->head; line != NULL; line = line->nxt)
    {
        col = 0;
        if (row >= cmd->coords.from.top && row <= cmd->coords.to.top)
        {
            // prochazi sloupec
            for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
            {
                if (col >= cmd->coords.from.left && col <= cmd->coords.to.left)
                {
                    double value = strtod(cell->value, NULL);
                    if (min > value)
                    {
                        min = value;
                        cmd->dest_point = np(row, col);
                    }
                }
                col++;
            }
        }
        row++;
    }
    if (min != DBL_MAX)
    {
        cmd->coords.from = cmd->dest_point;
        cmd->coords.to = cmd->dest_point;
        return true;
    }
    return false; // nic se nenaslo
}

/**
 * Najde souradnice bunky s nejvyssi ciselnou hodnotou. Pokud nic nenajde, 
 * nastavuje hodnotu Cmd::coords::from jako prvni bunku v rozsahu.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool max(Table *table, Cmd *cmd)
{
    int row = 0;
    int col = 0;
    double max = DBL_MIN;
    cmd->dest_point = np(0, 0);
    for (Line *line = table->head; line != NULL; line = line->nxt)
    {
        col = 0;
        if (row >= cmd->coords.from.top && row <= cmd->coords.to.top)
        {
            // prochazi sloupec
            for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
            {
                if (col >= cmd->coords.from.left && col <= cmd->coords.to.left)
                    if (is_num(cell->value))
                    {
                        double value = strtod(cell->value, NULL);
                        if (max < value)
                        {
                            max = value;
                            cmd->dest_point = np(row, col);
                        }
                    }
                col++;
            }
        }
        row++;
    }
    if (max != DBL_MIN)
    {
        cmd->coords.from = cmd->dest_point;
        cmd->coords.to = cmd->dest_point;
        return true;
    }
    return false; // nic se nenaslo
}

/**
 * Najde souradnice bunky, ktera obsahuje podretezec Cmd::cmd_args. Pokud nic nenajde, 
 * nastavuje hodnotu Cmd::coords::from jako prvni bunku v rozsahu.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool find(Table *table, Cmd *cmd)
{
    int row = 0;
    int col = 0;
    bool success = false;
    cmd->dest_point = np(0, 0);
    for (Line *line = table->head; line != NULL; line = line->nxt)
    {
        // prochazi sloupec
        col = 0;
        for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
        {
            if (contains_str(cell->value, cmd->cmd_args))
            {
                cmd->dest_point = np(row, col);
                success = true;
                break;
            }
            col++;
        }
        row++;
        if (success)
            break;
    }
    if (success)
    {
        cmd->coords.from = cmd->dest_point;
        cmd->coords.to = cmd->dest_point;
        return true;
    }
    return false; // nic se nenaslo
}

char *range_cmd_names[NUM_OF_RANGE_CMD] = {"min", "max", "find"};
bool (*range_cmd_arr[NUM_OF_RANGE_CMD])(Table *table, Cmd *cmd) = {
    min, max, find};

/**
 * Nastavi hodnotu na urcene point
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param p ukazatel na strukturu Point
 * @param value retezec
 * @return false pokud se nedokaze provest nejaka akce
 */
bool set_point(Table *table, Point *p, char *value)
{
    char *str = substring(value, 1, (strlen(value) - 2));
    Cell *coords = str_to_list(str, ",", NULL);
    bool row = true;
    for (Cell *temp = coords; temp != NULL; temp = temp->nxt)
    {
        if (row) // nastaveni radku
            p->top = temp->value[0] == '-' ? table->rows - 1 : atoi(temp->value);
        else // nastaveni sloupce
            p->left = temp->value[0] == '-' ? table->cols - 1 : atoi(temp->value);
        row = false;
    }
    cell_dtor(&coords);
    free(str);
    return true;
}

/**
 * Nastavi novy rozsah bunek.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool set_range(Table *table, Cmd *cmd, char *value)
{
    int count = 0;
    bool success = true;
    Cell *coords = str_to_list(value, ",", &count);
    if (count == 2) // pro [R,C]
    {
        bool row = true;
        for (Cell *temp = coords; temp != NULL; temp = temp->nxt)
        {
            if (temp->value[0] == '_')
            {
                if (row) // nastaveni radku
                {
                    cmd->coords.from.top = 0;
                    cmd->coords.to.top = table->rows - 1;
                }
                else // nastaveni sloupce
                {
                    cmd->coords.from.left = 0;
                    cmd->coords.to.left = table->cols - 1;
                }
            }
            else
            {
                if (row) // nastaveni radku
                {
                    cmd->coords.from.top = temp->value[0] == '-' ? table->rows - 1 : atoi(temp->value);
                    cmd->coords.to.top = cmd->coords.from.top;
                }
                else // nastaveni sloupce
                {
                    cmd->coords.from.left = temp->value[0] == '-' ? table->cols - 1 : atoi(temp->value);
                    cmd->coords.to.left = cmd->coords.from.left;
                }
            }
            row = false;
        }
    }
    else if (count == 4) // pro [R1,C1,R2,C2]
    {
        int arr[count];
        int i = 0;
        for (Cell *temp = coords; temp != NULL; temp = temp->nxt)
        {
            // povolene znaky: pomlcka nebo cislo
            if (!(temp->value[0] == '-' || isdigit(temp->value[0])))
            {
                cell_dtor(&coords);
                return false;
            }
            // nastavi misto pomlcky index posledniho radku/sloupce
            arr[i] = !strcmp(temp->value, "-") ? (i % 2 == 0 ? table->rows - 1 : table->cols - 1) : (atoi(temp->value));
            i++;
        }
        if (arr[0] > arr[2] || arr[1] > arr[3]) // musi platit R1 <= R2 && C1 <= C2
            success = false;
        else
        {
            cmd->coords.from = np(arr[0], arr[1]);
            cmd->coords.to = np(arr[2], arr[3]);
        }
    }
    else // neznamy prikaz
        success = false;
    cell_dtor(&coords);
    return success;
}

/**
 * Nastavi novy rozsah bunek nebo provede [] prikazy.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool get_range(Table *table, Cmd *cmd, char *value)
{
    bool success = true;
    if (!strcmp(value, "[set]")) // prikaz [set]
    {
        cmd->temp_range = cmd->coords;
        return success;
    }
    else if (!strcmp(value, "[_]")) // prikaz [_]
    {
        cmd->coords = cmd->temp_range;
        return success;
    }
    char *temp = substring(value, 1, strlen(value) - 2);
    // prikazy nastaveni tabulky
    if (isdigit(temp[0]) || temp[0] == '-' || temp[0] == '_')
    {
        success = set_range(table, cmd, temp);
        free(temp);
        return success;
    }
    else // specialni fce max, min, find
    {
        Cell *list_of_cmds = str_to_list(temp, " ", NULL);
        strcpy(cmd->cmd_name, list_of_cmds->value);
        Cell *cell = list_of_cmds->nxt;
        if (cell != NULL)
            set_cmd_args(cmd, cell->value);
        int i = 0;
        bool command = false;
        for (; i < NUM_OF_RANGE_CMD; i++)
        {
            if (!strcmp(cmd->cmd_name, range_cmd_names[i]))
            {
                (*range_cmd_arr[i])(table, cmd);
                command = true;
            }
        }

        if (!command) // nenasel prikaz
        {
            fprintf(stderr, "(get_range): wrong command");
            success = false;
        }
        cell_dtor(&list_of_cmds);
    }
    free(temp);
    return success;
}

/**
 * Nastavi hodnoty cmd pro pozdejsi praci s nimi
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepodari provest prikaz
 */
bool set_cmd(Table *table, Cmd *cmd)
{
    bool success = true;
    if (table != NULL && cmd != NULL)
    {
        if (cmd->curr->value[0] == '[')
        {
            success = get_range(table, cmd, cmd->curr->value);
            fill_table(table, cmd->coords.to);
            strcpy(cmd->cmd_name, "");
        }
        else
        {
            // rozdeli na dodatecne prikazy
            Cell *head = str_to_list(cmd->curr->value, " ", NULL);
            Cell *cell = head->nxt;
            if (cell != NULL)
            {
                if (cell->value != NULL && cell->value[0] == '[')
                    success = set_point(table, &cmd->dest_point, cell->value);
                else
                    success = set_cmd_args(cmd, cell->value);
            }
            strcpy(cmd->cmd_name, head->value);
            cell_dtor(&head);
        }
    }
    return success;
}

/**
 * Zpracovava prikazy.
 * 
 * @param table ukazatel na tabulkovou strukturu.
 * @param cmd ukazatel na strukturu prikazu
 * @return false pokud se nepovede provest nejaky prikaz
 */
bool do_cmds(Table *table, Cmd *cmd)
{
    if (table != NULL && cmd != NULL)
    {
        for (; cmd->curr != NULL; cmd->curr = cmd->curr->nxt)
        {
            if (cmd->curr->value == NULL)
                return false;

            if (!set_cmd(table, cmd))
            {
                fprintf(stderr, "wrong argument");
                return false;
            }
            if (!cmd->cmd_name[0]) // jen prikaz zmeny okna tabulky
                continue;
            // prochazi fce
            int i = 0;
            bool success = false;
            for (; i < NUM_OF_CMDS; i++)
            {
                if (!strcmp(cmd->cmd_name, cmd_names[i]))
                {
                    if (i >= 9 && i <= 12) // funkce sum avg count len
                    {
                        if (!(*cmds_arr[i])(table, cmd))
                            return false;
                    }
                    else
                    {
                        for (int k = cmd->coords.from.top; k <= cmd->coords.to.top; k++)
                            for (int j = cmd->coords.from.left; j <= cmd->coords.to.left; j++)
                            {
                                cmd->curr_point = np(k, j);
                                if (!(*cmds_arr[i])(table, cmd))
                                    return false;
                            }
                    }
                    success = true;
                    break;
                }
            }
            if (!success)
            {
                fprintf(stderr, "Command not found.\n");
                return false;
            }
        }
    }
    return true;
}

/**
 * Smaze vsechny prazdne bunky napravo.
 * 
 * @param table ukazatel na tabulkovou strukturu
 * @param cmd ukazatel na strukturu prikazu
 */
void delete_cols(Table *table)
{
    int col_max = 0;
    for (Line *line = table->head; line != NULL; line = line->nxt)
    {
        int col = 0;
        int counter = 0;
        for (Cell *cell = line->head; cell != NULL; cell = cell->nxt)
        {
            col++;
            if (cell->value[0]) // pokud neni prazdna bunka
                counter = col;
        }
        int i = counter;
        while (i < col)
        {
            // nemam cas komentovat
            remove_cell(&line->head, counter);
            i++;
        }
        col_max = col_max < counter ? counter : col_max;
    }
    table->cols = col_max;
}

int main(int argc, char *argv[])
{
    Table table;
    Cmd cmd;
    table_init(&table);
    cmd_init(&cmd);
    if (!load_args(argc, argv, &table, &cmd))
    {
        fprintf(stderr, "arguments syntax error.\n");
        return to_exit(&table, &cmd, EXIT_FAILURE);
    }

    if (!load_file(&table, table.file_path))
        return to_exit(&table, &cmd, EXIT_FAILURE);
    fill_table(&table, np(table.rows - 1, table.cols - 1));
    if (!do_cmds(&table, &cmd))
    {
        return to_exit(&table, &cmd, EXIT_FAILURE);
    }

    delete_cols(&table);
    fill_table(&table, np(table.rows - 1, table.cols - 1));
    write_out(&table, table.file_path);
    return to_exit(&table, &cmd, EXIT_SUCCESS);
}
