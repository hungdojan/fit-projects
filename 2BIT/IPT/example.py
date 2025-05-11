#!/usr/bin/env python3

from task1 import task1_lib as t1
from task2 import task2_lib as t2
from task3 import task3_lib as t3
from task4 import task4_lib as t4


# Visibility flags
TASK1_VISIBLE = False
TASK2_VISIBLE = False
TASK3_VISIBLE = False
TASK4_VISIBLE = True

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
        t2.interval_ext([1, 2, 3], table_val_left=None, table_val_right=1.65)

    adresujte hodnoty, za ktere chcete neco doplnit, 
    jinak nastavte default hodnotu.

    Funkce se pisou do if bloku (doporuceno). Pokud chces skryt priklad,
    nastav promennout TASK[cislo_ukolu]_VISIBLE na False, pro viditelnost
    nastav True.
"""
# Playground
# ******* TASK 1 ********
if TASK1_VISIBLE:
    t1.general([8, 8, 7, 2, 7])
    print("==========")
    t1.general([8, 6, 6, 6, 7, 5, 8, 8, 6, 7])
    print("==========")
    t1.frequency([10, 7, 10, 7, 8, 9, 9, 10, 8, 6], 9)
    pass

# ******* TASK 2 ********
if TASK2_VISIBLE:

    t2.interval_est([20.1, 24.1, 21.5, 22.8, 23.4, 20.2, 19.8, 22.7, 22.9, 19],
            2.262, 2.262)
    print("==========")
    t2.interval_est([5.4, 4.3, 5.1, 5.6, 5.4, 5.3, 5.7, 4.6], table_val_left=2.998)
    print("==========")
    t2.interval_est_known_variance([336, 331, 337, 337, 340, 327, 326, 326, 321, 339],
            7, 1.960, 1.960)
    print("==========")
    t2.interval_est_big_dataset(2426.71, 9292.96, 100, 1.645, 1.645)
    print("=========")
    t2.interval_est_chi([21.2, 20.7, 19.5, 21.6, 16.8, 21.3, 23.5, 19, 23.6, 21.3],
            23.589, 1.735) 
    print("=========")
    t2.interval_est_chi([320, 327, 312, 323, 326, 334, 333, 326], 12.017)
    pass

# ******* TASK 3 ********
if TASK3_VISIBLE:
    T = t3.test_est_value([503, 511, 495, 485, 488, 501, 512, 505, 505, 520,
        510, 450, 462, 475, 491, 480, 466, 500, 461, 507], 500)
    print(T)
    print("=========")
    s = t3.test_variance([0.93, 0.94, 0.92, 1.01, 0.93, 1.07, 1.03, 0.98, 1.03, 0.96,
        1.03, 1.00, 1.10, 0.99, 1.02, 0.96, 1.02, 0.86, 1.02, 1.04], 0.05**2)
    print(s)
    print("=========")
    T = t3.test_unity(
            [44, 58, 52, 50, 64, 53, 62, 48, 53, 53,
                60, 63, 60, 53, 61, 53, 50, 68, 60, 49],
            [71, 62, 74, 58, 63, 68, 51, 61, 63, 62,
                65, 64, 62, 72, 57, 63, 70, 65, 65, 67])
    print(T)
    print("=========")
    T = t3.get_ny(
            [44, 58, 52, 50, 64, 53, 62, 48, 53, 53,
                60, 63, 60, 53, 61, 53, 50, 68, 60, 49],
            [71, 62, 74, 58, 63, 68, 51, 61, 63, 62,
                65, 64, 62, 72, 57, 63, 70, 65, 65, 67])
    print(T)
    print("=========")
    T = t3.paired_test_unity([9, 11, 9, 8, 6, 10, 10, 8, 12, 7, 10, 6, 14, 7, 8],
                             [11, 12, 10, 10, 11, 12, 12, 9, 14, 8, 10, 8, 15, 10, 10])
    print(T)
    print("=========")
    T = t3.get_ny([9, 11, 9, 8, 6, 10, 10, 8, 12, 7, 10, 6, 14, 7, 8],
                             [11, 12, 10, 10, 11, 12, 12, 9, 14, 8, 10, 8, 15, 10, 10])
    print(T)
    print("=========")
    pass

# ******* TASK 4 ********
if TASK4_VISIBLE:
    beta_0, beta_1 = t4.linear_reg([24.1, 24.9, 26.2, 27.6, 23.5, 25.2, 26.1, 26.9, 27.4, 25],
            [360, 390, 476, 510, 323, 395, 401, 481, 466, 433])
    print(beta_0 + beta_1 * 27)
    print("=========")
    beta_0, beta_1, beta_2 = t4.parab_reg(
            [5, 62, 19, 34, 79, 56, 74, 28, 42, 45],
            [0, 71, 3, 5, 81, 30, 90, 7, 16, 30])
    print(beta_0 + beta_1 * 50 + beta_2 * (50**2))
    print("=========")
    beta_0, beta_1, beta_2 = t4.plain_reg(
            [8, 11, 9, 9, 12, 10, 12, 9, 9, 10, 11, 12],
            [14, 9, 15, 8, 6, 20, 14, 5, 20, 3, 10, 18],
            [53, 46, 66, 35, 41, 80, 65, 42, 70, 38, 46, 68])
    print(beta_0 + beta_1 * 14 + beta_2 * 11)
    print("=========")
    T = t4.pearson_correlation(
            [61, 219, 146, 125, 278, 156, 261, 106, 154, 162, 232, 268],
            [65, 12, 54, 40, 17, 47, 0, 4, 16, 13, 29, 25])
    print("=========")
    T = t4.spierman_correlation(
            [61, 219, 146, 125, 278, 156, 261, 106, 154, 162, 232, 268],
            [65, 12, 54, 40, 17, 47, 0, 4, 16, 13, 29, 25])
    print("=========")

    # TODO: add code
    pass
