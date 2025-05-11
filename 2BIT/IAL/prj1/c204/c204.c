 
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"
#include <string.h> // strlen
#include <ctype.h>

int solved;

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void untilLeftPar( Stack *stack, char *postfixExpression, unsigned *postfixExpressionLength ) {
    char c = '\0';
    while (!Stack_IsEmpty(stack) && c != '(') {
        // ziska operator a uvolni vrsek zasobniku
        Stack_Top(stack, &c);
        Stack_Pop(stack);
        
        // zapis do retezce
        if (c != '\0' && c != '(')
            postfixExpression[(*postfixExpressionLength)++] = c;
    }

}

/**
 * Pomocna funkce urcujici priority operatoru
 * 
 * @param operator Znak operatoru
 * @return Hodnota operatoru; -1 pokud byl nacten neznamy operator
 */
int get_priority( const char operator ) {
    switch (operator) {
        case '+': case '-':
            return 1;
        case '*': case '/':
            return 2;
        case '(':
            return 3;
        default:
            return -1;
    }
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void doOperation( Stack *stack, char c, char *postfixExpression, unsigned *postfixExpressionLength ) {

    int new_op_priority = get_priority(c);  

    // pracuje, dokud neni prazdny zasobnik
    while (!Stack_IsEmpty(stack)) {
        char op_in_stack;
        Stack_Top(stack, &op_in_stack);     // operator v zasobniku

        // ziska prioritu operaci v zasobniku
        int stk_op_priority = get_priority(op_in_stack);

        // dokud nenarazi na otevrenou zavorku a priorita operatoru
        // v zasobniku je stejna nebo vyssi nez nacteny operator
        // pak se operator ze zasobniku vyjme a da do vysledneho retezce
        if (new_op_priority <= stk_op_priority && op_in_stack != '(') {
            postfixExpression[(*postfixExpressionLength)++] = op_in_stack;
            Stack_Pop(stack);
        } else {
            break;
        }
    }
    // prida momentalni operator na vrchol zasobniku
    Stack_Push(stack, c);
}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
 * řetězce konstantu NULL.
 *
 * @param infixExpression Znakový řetězec obsahující infixový výraz k převedení
 *
 * @returns Znakový řetězec obsahující výsledný postfixový výraz
 */

char *infix2postfix( const char *infixExpression ) {

    // alokace vysledne promenne
    char *postfix = (char *)calloc(MAX_LEN, sizeof(char)); 
    unsigned len = 0;   // pozice v postfix
    unsigned i = 0;     // pozice v infixExpression
    Stack s;
    Stack_Init(&s);
    char c = '\0';

    // prochazi vstupni retezec
    if (postfix != NULL) {
        while (c != '=') {
            c = infixExpression[i++];
            if (isalnum(c)) {           // vstup je alfa-numericky znak
                postfix[len++] = c;
            } else if (c == ')') {      // vstup je ukoncovaci zavorka
                untilLeftPar(&s, postfix, &len);
            } else if (c != '=') {      // vstup je rovna se
                doOperation(&s, c, postfix, &len);
            }
        }
        // vypsat konec
        // pokud je jeste neco v zasobniku, tak se prida na konec postfix retezce
        while (!Stack_IsEmpty(&s)) {
            Stack_Top(&s, &c);
            Stack_Pop(&s);
            postfix[len++] = c;
        }
        postfix[len++] = '=';
        postfix[len++] = '\0';
    }

    return postfix; /* V případě řešení můžete smazat tento řádek. */
}

/* Konec c204.c */
