"""
Funkce pro druhy ukol na bodove a intervalove odhady
"""
import statistics
import math


def _get_variance(lof_num: list):
    return ((1 / (len(lof_num) - 1) *
        (sum(map(lambda x: x**2, lof_num)) -
            len(lof_num) * statistics.mean(lof_num) ** 2)))


def interval_est(lof_num: list, table_val_left=None,
        table_val_right=None, verbose=False):

    """Vypocet intervalu stredni hodnoty
        Hledani bodoveho odhadu stredni hodnoty

    Keyword arguments:
    lof_num -- seznam hodnot
    table_val_left -- leva tabulkova hodnota, t_rozdeleni (default None)
    table_val_left -- prava tabulkova hodnota, t_rozdeleni (default None)
    verbose -- vypis pridavnych informaci (default False)
    """

    if verbose:
        print("""
    Pouziti vzorce pro oboustranny:
        index = 1-alfa/2
        table_val_left = table_val_right = t_index(n-1)
        ( mean - table_val * variance / sqrt(n) , mean + table_val * variance / sqrt(n) )
    Pouziti vzorce pro levostranny:
        index = 1-alfa
        table_val_left = t_index(n-1)
        ( mean - table_val * variance / sqrt(n) , +inf )
    Pouziti vzorce pro pravostranny:
        index = 1-alfa
        table_val_right = t_index(n-1)
        ( -inf , mean + table_val * variance / sqrt(n) )
            """)

    s = math.sqrt(_get_variance(lof_num))
    n = len(lof_num)
    mean = statistics.mean(lof_num)
    print(f"Bodovy odhad stredni hodnoty: {mean}")
    left = "-inf"
    right = "+inf"
    if table_val_right is not None and table_val_left is not None:
        left = mean - table_val_left * s / math.sqrt(n)
        right = mean + table_val_left * s / math.sqrt(n)
    elif table_val_left is not None:
        left = mean - table_val_left * s / math.sqrt(n)
    elif table_val_right is not None:
        right = mean + table_val_left * s / math.sqrt(n)
    else:
        print("chybejici tabulkove hodnoty!!!")

    print(f"Vysledny interval: ( {left} , {right} )")


def interval_est_known_variance(lof_num: list, variance: float,
        table_val_left=None, table_val_right=None, verbose=False):

    """Vypocet intervalu stredni hodnoty se zadanym rozptylem
        Hledani bodoveho odhadu stredni hodnoty

    Keyword arguments:
    lof_num -- seznam hodnot
    variance -- zadany rozptyl
    table_val_left -- leva tabulkova hodnota, u_rozdeleni (default None)
    table_val_left -- prava tabulkova hodnota, u_rozdeleni (default None)
    verbose -- vypis pridavnych informaci (default False)
    """
    if verbose:
        print("""
    Pouziti vzorce pro oboustranny:
        index = 1-alfa/2
        table_val_left = table_val_right = u_index
        ( mean - table_val * variance / sqrt(n) , mean + table_val * variance / sqrt(n) )
    Pouziti vzorce pro levostranny:
        index = 1-alfa
        table_val_left = u_index
        ( mean - table_val * variance / sqrt(n) , +inf )
    Pouziti vzorce pro pravostranny:
        index = 1-alfa
        table_val_right = u_index
        ( -inf , mean + table_val * variance / sqrt(n) )
            """)

    s = variance
    n = len(lof_num)
    mean = statistics.mean(lof_num)
    print(f"Bodovy odhad stredni hodnoty: {mean}")
    left = "-inf"
    right = "+inf"
    if table_val_right is not None and table_val_left is not None:
        left = mean - table_val_left * s / math.sqrt(n)
        right = mean + table_val_left * s / math.sqrt(n)
    elif table_val_left is not None:
        left = mean - table_val_left * s / math.sqrt(n)
    elif table_val_right is not None:
        right = mean + table_val_left * s / math.sqrt(n)
    else:
        print("chybejici tabulkove hodnoty!!!")

    print(f"Vysledny interval: ( {left} , {right} )")


def interval_est_big_dataset(exp_val: float, variance: float, nof_tests: int,
        table_val_left=None, table_val_right=None, verbose=False):
    """Vypocet intervalu stredni hodnoty pri velkem rozsahu vyberu
        Hledani bodoveho odhadu stredni hodnoty

    Keyword arguments:
    exp_val -- zadana stredni hodnota
    variance -- zadany rozptyl
    nof_tests -- pocet vyberu/testu/prvku
    table_val_left -- leva tabulkova hodnota, u_rozdeleni (default None)
    table_val_right -- prava tabulkova hodnota, u_rozdeleni (default None)
    verbose -- vypis pridavnych informaci (default False)
    """
    if verbose:
        print("""
    Pouziti vzorce pro oboustranny:
        index = 1-alfa/2
        table_val_left = table_val_right = u_index
        ( mean - table_val * variance / sqrt(n) , mean + table_val * variance / sqrt(n) )
    Pouziti vzorce pro levostranny:
        index = 1-alfa
        table_val_left = u_index
        ( mean - table_val * variance / sqrt(n) , +inf )
    Pouziti vzorce pro pravostranny:
        index = 1-alfa
        table_val_right = u_index
        ( -inf , mean + table_val * variance / sqrt(n) )
            """)
    s = math.sqrt(variance)
    n = nof_tests
    mean = exp_val
    left = "-inf"
    right = "+inf"
    if table_val_right is not None and table_val_left is not None:
        left = mean - table_val_left * s / math.sqrt(n)
        right = mean + table_val_left * s / math.sqrt(n)
    elif table_val_left is not None:
        left = mean - table_val_left * s / math.sqrt(n)
    elif table_val_right is not None:
        right = mean + table_val_left * s / math.sqrt(n)
    else:
        print("chybejici tabulkove hodnoty!!!")

    print(f"Vysledny interval: ( {left} , {right} )")


def interval_est_chi(lof_num: list, table_val_left=None,
        table_val_right=None, verbose=False):

    """Vypocet intervalu stredni hodnoty
        Hledani bodoveho odhadu stredni hodnoty

    Keyword arguments:
    lof_num -- seznam hodnot
    table_val_left -- leva tabulkova hodnota, t_rozdeleni (default None)
    table_val_left -- prava tabulkova hodnota, t_rozdeleni (default None)
    verbose -- vypis pridavnych informaci (default False)
    """

    if verbose:
        print("""
    Pouziti vzorce pro oboustranny:
        index_l = 1-alfa/2
        index_l = alfa/2
        table_val_left = chi_index_l
        table_val_right = chi_index_r
        ( (n-1) * s**2 / table_val_left , (n-1) * s**2 / table_val_right )
    Pouziti vzorce pro levostranny:
        index = 1-alfa
        table_val_left = chi_index
        ( (n-1) * s**2 / table_val_left , 0 )
    Pouziti vzorce pro pravostranny:
        index = alfa
        table_val_right = chi_index
        ( 0 , (n-1) * s**2 / table_val_right )
            """)

    s2 = _get_variance(lof_num)
    n = len(lof_num)
    print(f"Bodovy odhad rozptylu: {s2}")
    left = 0
    right = 0
    if table_val_right is not None and table_val_left is not None:
        left = ((n-1) * s2 / table_val_left)
        right = ((n-1) * s2 / table_val_right)
    elif table_val_left is not None:
        left = ((n-1) * s2 / table_val_left)
    elif table_val_right is not None:
        right = ((n-1) * s2 / table_val_right)
    else:
        print("chybejici tabulkove hodnoty!!!")

    print(f"Vysledny interval: ( {left} , {right} )")

    pass
