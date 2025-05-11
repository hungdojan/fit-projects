"""
Funkce pro prvni ukol na popisnou statistiku
"""
import statistics
import math


def general(lof_num: list, verbose=False):
    """Vypis zakladnich hodnot v popisne statistice

    Keyword arguments:

    lof_num -- seznam hodnot

    verbose -- vypis pridavnych informaci (default False)
    """
    lof_num.sort()
    if verbose:
        print(f"Serazeny seznam: {lof_num}")

    # pocet cisel
    n = len(lof_num)
    # aritmeticky prumer/stredni hodnota
    mean = statistics.mean(lof_num)

    print(f"Prumer: {mean}")

    if verbose:
        print("""
Pouzity vzorec:
    1/(n-1) * (sum(x^2) - n*mean^2)
Je mozne pouzit:
    1/(n-1) * sum( (x-mean)^2 )
        """)
    vyberovy_rozptyl = (
            (1 / (n - 1)) *
            (sum(map(lambda x: x**2, lof_num)) - (n * mean ** 2))
            )

    print(f"Vyberovy rozptyl: {vyberovy_rozptyl}")
    print(f"Smerodatna odchylka: {math.sqrt(vyberovy_rozptyl)}")

    print(f"Modus: {statistics.mode(lof_num)}")
    print(f"Median: {statistics.median(lof_num)}")


def frequency(lof_num: list, searched_val: int, verbose=False):
    """Vypis cetnosti

    Keyword arguments:
    lof_num -- seznam hodnot  

    searched_val -- hledana hodnota  
    
    verbose -- vypis pridavnych informaci (default False)
    """
    lof_num.sort()
    if verbose:
        print(f"Serazeny seznam: {lof_num}")

    cetnost = lof_num.count(searched_val)
    rel_cetnost = cetnost / len(lof_num)
    komul_cetnost = len(lof_num) - lof_num[::-1].index(searched_val)
    rel_komul_cetnost = komul_cetnost / len(lof_num)

    print(f"Cetnost: {cetnost}")
    print(f"Relativni cetnost: {rel_cetnost}")
    print(f"Komutativni cetnost: {komul_cetnost}")
    print(f"Relativni komutativni cetnost: {rel_komul_cetnost}")
