"""
Funkce pro treti ukol na testovani hypotez
"""
import math
import statistics


def test_est_value(lof_num: list, known_val: float):
    """ Test stredni hodnoty rozdeleni N(est_val, variance)

    Keyword arguments:

    lof_num: seznam zadanych hodnot

    known_val: hypotezova stredni hodnota

    Returns: Testovaci statistiku T
    """
    vyberovy_rozptyl = ((1 / (len(lof_num) - 1) *
        (sum(map(lambda x: x**2, lof_num)) -
            len(lof_num) * statistics.mean(lof_num) ** 2)))
    mean = statistics.mean(lof_num)
    return (mean - known_val) / math.sqrt(vyberovy_rozptyl) * math.sqrt(len(lof_num))


def test_est_value_known_variance(lof_num: list, known_val: float, variance: float):
    """ Test stredni hodnoty rozdeleni N(est_val, variance); znamy rozptyl

    Keyword arguments:

    lof_num: seznam zadanych hodnot

    known_val: hypotezova stredni hodnota

    variance: zadany rozptyl

    Returns: Testovaci statistiku U
    """
    mean = statistics.mean(lof_num)
    return (mean - known_val) / variance * math.sqrt(len(lof_num))


def test_est_value_big_dataset(est_val: float, variance: float, nof_items: int, known_val: float):
    """ Test stredni hodnoty rozdeleni N(est_val, variance); velky vyber

    Keyword arguments:

    est_val: zadana stredni hodnota

    variance: zadany rozptyl

    nof_items: pocet hodnot (velikost vyberu)

    known_val: hypotezova stredni hodnota

    Returns: Testovaci statistiku U
    """
    return (est_val - known_val) / math.sqrt(variance) * math.sqrt(nof_items)


def test_variance(lof_num: list, known_val: float):
    """ Test rozptylu rozdeleni N(est_val, variance)

    Keyword arguments:

    lof_num: seznam zadanych hodnot

    known_val: hypotezovy ROZPTYL (POZOR ROZPTYL!!! NE SMERODATNA ODCHYLKA)

    Returns: Testovaci statistiku K
    """
    vyberovy_rozptyl = ((1 / (len(lof_num) - 1) *
        (sum(map(lambda x: x**2, lof_num)) -
            len(lof_num) * statistics.mean(lof_num) ** 2)))
    return (len(lof_num)-1) * vyberovy_rozptyl / known_val


def alter_test_big_dataset(known_pi: float, p: float, nof_items: int):
    """ Test parametru alternativniho rozdeleni pro velky vyber

    Keyword arguments:

    known_pi: zadana PI hodnota

    p: zadana p hodnota

    nof_items: pocet hodnot (velikost vyberu)

    Returns: Testovaci statistiku U
    """
    return (p - known_pi) / math.sqrt(known_pi * (1-known_pi)) * math.sqrt(nof_items)


def get_ny(lof_num_x: list, lof_num_y: list):
    """ Vypocet ny na zaklade 2 seznamu hodnot

    Keyword arguments:

    lof_num_x: prvni seznam NEZAVYSLYCH hodnot

    lof_num_y: prvni seznam NEZAVYSLYCH hodnot

    Returns: Testovaci statistiku T
    """
    mean_x = statistics.mean(lof_num_x)
    mean_y = statistics.mean(lof_num_y)
    n_x = len(lof_num_x)
    n_y = len(lof_num_y)
    rozptyl_x = ((1 / (n_x - 1) *
        (sum(map(lambda x: x**2, lof_num_x)) - n_x * mean_x ** 2)))
    rozptyl_y = ((1 / (n_y - 1) *
        (sum(map(lambda y: y**2, lof_num_y)) - n_y * mean_y ** 2)))
    return ((rozptyl_x/n_x + rozptyl_y/n_y)**2 /
            ((rozptyl_x / n_x)**2 / (n_x - 1) + (rozptyl_y / n_y)**2 / (n_y - 1)))


def test_unity(lof_num_x: list, lof_num_y: list):
    """ Test shody strednich hodnot normalniho rozdeleni

    Keyword arguments:

    lof_num_x: prvni seznam NEZAVYSLYCH hodnot

    lof_num_y: prvni seznam NEZAVYSLYCH hodnot

    Returns: Testovaci statistiku T
    """
    mean_x = statistics.mean(lof_num_x)
    mean_y = statistics.mean(lof_num_y)
    n_x = len(lof_num_x)
    n_y = len(lof_num_y)
    rozptyl_x = ((1 / (n_x - 1) *
        (sum(map(lambda x: x**2, lof_num_x)) - n_x * mean_x ** 2)))
    rozptyl_y = ((1 / (n_y - 1) *
        (sum(map(lambda y: y**2, lof_num_y)) - n_y * mean_y ** 2)))

    return (mean_x - mean_y) / math.sqrt(rozptyl_x / n_x + rozptyl_y / n_y)


def test_unity_big_dataset(mean_x: float, rozptyl_x: float, n_x: int,
        mean_y: float, rozptyl_y: float, n_y: int):
    """ Test shody strednich hodnot normalniho rozdeleni pro velky vyber

    Keyword arguments:

    lof_num_x: prvni seznam NEZAVYSLYCH hodnot

    lof_num_y: prvni seznam NEZAVYSLYCH hodnot

    Returns: Testovaci statistiku U
    """
    return (mean_x - mean_y) / math.sqrt(rozptyl_x / n_x + rozptyl_y / n_y)


def paired_test_unity(lof_num_x: list, lof_num_y: list):
    """ Test shody strednich hodnot normalniho rozdeleni na sobe zavisle

    Keyword arguments:

    lof_num_x: prvni seznam NEZAVYSLYCH hodnot

    lof_num_y: prvni seznam ZAVYSLYCH hodnot

    Returns: Testovaci statistiku T
    """
    if len(lof_num_y) != len(lof_num_x):
        print("velikosti fci se neshoduji!!!")
        return 0
    D = list(map(lambda x, y: x-y, lof_num_x, lof_num_y))
    mean_d = statistics.mean(D)
    n_d = len(D)
    rozptyl_d = ((1 / (n_d - 1) *
        (sum(map(lambda x: x**2, D)) - n_d * mean_d ** 2)))
    return mean_d / math.sqrt(rozptyl_d) * math.sqrt(len(lof_num_x))


def test_f():
    print("toto nehodla pocitat, sry!")
    return 0
