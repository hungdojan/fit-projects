#!/usr/bin/env python3

from task1 import task1_lib as t1
from task2 import task2_lib as t2
from task3 import task3_lib as t3
from task4 import task4_lib as t4


# Visibility flags
TASK1_VISIBLE = False
TASK2_VISIBLE = False
TASK3_VISIBLE = False
TASK4_VISIBLE = False

"""
Navod:
    volane funkce se nachazeji v souborech:

        task[cislo_ukolu]/task[cislo_ukolu]_lib.py

    a volaji se za pouziti nasledujici syntaxe:

        my_list = [1, 2, 3]
        t1.general(my_list)

    nebo

        t1.general([1, 2, 3])

    Kazda funkce ma jako posledni parametr "verbose",
    slozi pro vypsani doplnujicich informaci (napr. postupne vypocty ci
    pouzity vzorec).

    U nekterych funkci jsou parametry optional:
        
        # horni meze
        t2.interval_est([1, 2, 3], table_val_left=None, table_val_right=1.65)

    adresujte hodnoty, za ktere chcete neco doplnit, 
    jinak nastavte default hodnotu.

    Funkce se pisou do if bloku (doporuceno). Pokud chces skryt priklad,
    nastav promennout TASK[cislo_ukolu]_VISIBLE na False, pro viditelnost
    nastav True.
"""
# Playground
# ******* TASK 1 ********
if TASK1_VISIBLE:
    print("""
    Statistický soubor se skládá z 20 hodnot. 
    Jejich relativní kumulativní četnosti jsou v tabulce

    xi                   2    4    5   7   8
    rel. kumul. četnost: 0.15 0.35 0.5 0.7 1

    Určete následující hodnoty. 
    Výsledky uvádějte jako čísla zaokrouhlená na 3 desetinná místa 
    (pokud je potřeba zaokrouhlovat). Používejte desetinnou tečku, ne čárku.
    """)
    t1.general([2, 2, 2, 4, 4, 4, 4, 5, 5, 5, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8])
    print("============================")
    print("""
    Máme soubor hodnot: 
        3, 3, 8, 8, 9.

    Vypočítejte aritmetický průměr, výběrový rozptyl a výběrovou směrodatnou odchylku.

    Všechny hodnoty udávejte jako desetinná čísla 
    zaokrouhlená alespoň na tři desetinná místa, použijte desetinnou tečku, ne čárku.
    """)
    t1.general([3, 3, 8, 8, 9])
    print("============================")
    print("""
    Statistický soubor se skládá z hodnot
        4, 6, 5, 6, 3, 6, 7, 3, 5, 5, 6.

    Najděte modus a medián.

    Výsledek zadejte přesně. 
    V případě, že výsledek není celé číslo, použijte desetinnou tečku, nikoli čárku.
    """)
    t1.general([4, 6, 5, 6, 3, 6, 7, 3, 5, 5, 6])
    print("============================")
    print("""
    Statistický soubor se skládá z hodnot
        7, 4, 7, 4, 5, 6, 6, 7, 5, 3.

    Určete níže zadané hodnoty.

    Výsledek zadejte přesně. 
    V případě, že výsledek není celé číslo, použijte desetinnou tečku, nikoli čárku.
    """)
    t1.frequency([7, 4, 7, 4, 5, 6, 6, 7, 5, 3], 7)
    pass

# ******* TASK 2 ********
if TASK2_VISIBLE:
    t2.interval_est(
        [5.2, 5.4, 5.3, 5.6, 5.4, 5.8, 5.3, 6], 
        table_val_left=1.985, verbose=True)
    print("============================")
    t2.interval_est_big_dataset(1480.36, 880.9024, 80, 1.645, 1.645)
    print("============================")
    t2.interval_est_chi([324, 330, 327, 331, 335, 320, 317, 330, 325, 324], table_val_right=4.168)
    print("============================")
    pass

# ******* TASK 3 ********
if TASK3_VISIBLE:
    print("""
    Předpokládejme, že výnos ozimé pšenice v tunách na hektar je
    náhodná veličina s normálním rozdělením. Za letošní rok jsme
    zaznamenali následující data od několika pěstitelů.
    Případná prázdná pole v tabulce ignorujte.

    Výnos pšenice [t/ha]:
        5.5, 6.5, 5.5, 6.3, 6.8, 6.1, 7, 5.6, 6.8, 6.5, 6.7, 5.8, 5.4

    Na hladině významnosti 0.05 testujte, zda se střední hodnota
    výnosu pšenice oproti loňské hodnotě, která byla 5.4 t/ha, významně zvýšila.
    """)
    val = t3.test_est_value([5.5, 6.5, 5.5, 6.3, 6.8, 6.1, 7, 5.6, 6.8, 6.5, 6.7, 5.8, 5.4], 5.4)
    print(f"Vysledna hodnota: {val}")
    print("============================")

    print("""
    Balicí automat plní balíčky s kokosem o předepsané hmotnosti 300 g,
    přičemž hmotnost náhodně vybraného balíčku je náhodná veličina
    s normálním rozdělením. Balení je považováno za dostatečně přesné,
    jestliže směrodatná odchylka nepřesáhne 15 g.

    Máme podezření, že automat nefunguje dost přesně a snažíme se to ověřit.
    Bylo náhodně vybráno 13 balíčků, zjistili jsme následující hmotnosti.
    Případná prázdná pole v tabulce ignorujte.

    Hmostnost [g]:
        311 318 272 322 279 303 301 309 301 296 315 323 294

    Na hladině významnosti 0.05 testujte, zda směrodatná odchylka
    přesahuje 15 g, nebo zda je vše v pořádku.
    """)
    val = t3.test_variance([311, 318, 272, 322, 279, 303, 301, 309, 301, 296, 315, 323, 294], 15**2)
    print(f"Vysledna hodnota: {val}")
    print("============================")

    print("""
    Zkoumáme, jaký vliv má nové krmivo na dojivost krav.

    Skupina dojnic, které měly původně přibližně stejnou dojivost,
    byla rozdělena na dvě části. Jedna dostávala nové krmivo, druhá ne.
    Průměrné dojivosti krav (v litrech za den) jsou v tabulkách,
    případná prázdná pole ignorujte.

    Krávy s novým krmivem (x):
        20 25 20 21 19 21 21 21 17 18

    Ostatní krávy (y):
        22 24 21 21 20 24 20 22

    Za předpokladu, že výběry pocházejí z normálního rozdělení,
    na hladině významnosti α=0.01 testujte,
    zda nové krmivo významně ovlivňuje dojivost krav.
    """)
    list1 = [20, 25, 20, 21, 19, 21, 21, 21, 17, 18]
    list2 = [22, 24, 21, 21, 20, 24, 20, 22]
    val = t3.test_unity(list1, list2)
    ny = t3.get_ny(list1, list2)
    print(f"Ny: {ny}")
    print(f"Vysledna hodnota: {val}")
    print("============================")
    pass

# ******* TASK 4 ********
if TASK4_VISIBLE:
    print("""
    Zkoumáme závislost průměrné denní spotřeby vody v domácnosti
    na počtu členů domácnosti. Průzkumem jsme získali následující data
    (případná prázdná pole v tabulce ignorujte):

    Průměrná denní spotřeba vody [l]:
        178 255 227 234 259 171 99 326 238 318 311 81

    Počet členů domácnosti:
        2 3 3 3 3 2 1 4 3 4 4 1

    Popište závislost spotřeby vody na počtu
    členů domácnosti pomocí regresní přímky.
    """)
    list_x = [2, 3, 3, 3, 3, 2, 1, 4, 3, 4, 4, 1]
    list_y = [178, 255, 227, 234, 259, 171, 99, 326, 238, 318, 311, 81]
    beta_0, beta_1 = t4.linear_reg(list_x, list_y)

    print(f"Pro hodnotu 3: {beta_0 + beta_1 * 3}")
    print("============================")
    print("""
    Zkoumáme závislost výšky rostliny na koncentraci hnojiva v zálivce.
    Získali jsme následující data (případná prázdná pole v tabulce ignorujte):
    Koncentrace hnojiva [ml/l]:
        3 3 4 5 5 5 6 7 7 7
    Výška rostliny [m]:
        1.76 1.86 1.91 2.09 2.06 1.99 2.15 2.19 2.17 2.21

    Popište závislost výšky rostliny na
    koncentraci hnojiva pomocí regresní paraboly.
    """)
    list_x = [3, 3, 4, 5, 5, 5, 6, 7, 7, 7]
    list_y = [1.76, 1.86, 1.91, 2.09, 2.06, 1.99, 2.15, 2.19, 2.17, 2.21]
    beta_0, beta_1, beta_2 = t4.parab_reg(list_x, list_y)
    print(f"Pro hodnotu 5: {beta_0 + beta_1 * 5 + beta_2 * 25}")
    print("============================")
    print('''
    Zkoumáme, jestli u chovatelů psů existuje závislost mezi
    výdaji za jídlo pro chovatele a výdaji za krmení pro psa.
    Průzkumem jsme zjistili následující hodnoty ročních výdajů.
    Případná prázdná pole v tabulce ignorujte.

    Za jídlo [tis. Kč]:
        41.2 42.7 36.2 48.5 51.4 57.5 60.2 49.5 55.9 47.4 44.3 56.9 61.1 51.1 47.7
    Za krmení [tis. Kč]:
        11.7 7.3 14.9 15 11.5 13.8 9.4 10.8 16.3 13.1 14.4 12.8 10.3 13.9 9.7

    Předpokládejme, že výběr pochází z dvourozměrného normálního rozdělení.

    Na hladině významnosti 0.05 testujte,
    zda existuje (lineární) závislost mezi
    výdaji za jídlo pro člověka a krmení pro psa.
    ''')
    list_x = [41.2, 42.7, 36.2, 48.5, 51.4, 57.5, 60.2, 49.5,
              55.9, 47.4, 44.3, 56.9, 61.1, 51.1, 47.7]
    list_y = [11.7, 7.3, 14.9, 15, 11.5, 13.8, 9.4, 10.8, 16.3,
              13.1, 14.4, 12.8, 10.3, 13.9, 9.7]

    t4.pearson_correlation(list_x, list_y)
    pass
