"""
Funkce pro ctvrty ukol na regresni a korelacni analyzu
"""

import math
import statistics
import numpy


def linear_reg(lof_num_x: list, lof_num_y: list):
    """Regresni primka

    Keyword arguments:

    lof_num_x: seznam cisel NEZAVISLE slozky

    lof_num_y: seznam cisel ZAVISLE slozky
    """
    if len(lof_num_x) != len(lof_num_y):
        print("seznamy cisel musi mit stejnou velikost!!!")
        return 0
    n = len(lof_num_x)
    xi = sum(lof_num_x)
    yi = sum(lof_num_y)
    xi2 = sum(map(lambda x: x**2, lof_num_x))
    xiyi = sum(map(lambda x, y: x*y, lof_num_x, lof_num_y))

    print(f"{n}beta_0 + {xi}beta_1 = {yi}")
    print(f"{xi}beta_0 + {xi2}beta_1 = {xiyi}")
    beta_1 = (n * xiyi - xi*yi) / (n * xi2 - xi**2)
    beta_0 = statistics.mean(lof_num_y) - beta_1 * statistics.mean(lof_num_x)
    print(f"y = {beta_0:.4f} + {beta_1:.4f}x")

    Se = sum(map(lambda y: y**2, lof_num_y)) - beta_0 * yi - beta_1 * xiyi
    print(f"Odhad rozptylu: {Se / (n - 2)}")
    St = sum(map(lambda y: (y - statistics.mean(lof_num_y)) ** 2, lof_num_y))
    print(f"St: {St}")
    R2 = 1 - Se / St
    print(f"R2: {R2}")
    return beta_0, beta_1


def parab_reg(lof_num_x: list, lof_num_y: list):
    """Regresni parabola

    Keyword arguments:

    lof_num_x: seznam cisel NEZAVISLE slozky

    lof_num_y: seznam cisel ZAVISLE slozky
    """
    if len(lof_num_x) != len(lof_num_y):
        print("seznamy cisel musi mit stejnou velikost!!!")
        return 0
    n = len(lof_num_x)
    xi = sum(lof_num_x)
    xi2 = sum(map(lambda x: x**2, lof_num_x))
    xi3 = sum(map(lambda x: x**3, lof_num_x))
    xi4 = sum(map(lambda x: x**4, lof_num_x))
    yi = sum(lof_num_y)
    xiyi = sum(map(lambda x, y: x*y, lof_num_x, lof_num_y))
    xi2yi = sum(map(lambda x, y: x**2 * y, lof_num_x, lof_num_y))
    print(f"{n}beta_0 + {xi}beta_1 + {xi2}beta_2= {yi}")
    print(f"{xi}beta_0 + {xi2}beta_1 + {xi3}beta_2 = {xiyi}")
    print(f"{xi2}beta_0 + {xi3}beta_1 + {xi4}beta_2 = {xi2yi}")

    D = numpy.array([[n,   xi,  xi2],
                     [xi,  xi2, xi3],
                     [xi2, xi3, xi4]])
    D0 = numpy.array([[yi,    xi,  xi2],
                      [xiyi,  xi2, xi3],
                      [xi2yi, xi3, xi4]])
    D1 = numpy.array([[n,   yi,    xi2],
                      [xi,  xiyi,  xi3],
                      [xi2, xi2yi, xi4]])
    D2 = numpy.array([[n,   xi,  yi],
                      [xi,  xi2, xiyi],
                      [xi2, xi3, xi2yi]])
    beta_0 = numpy.linalg.det(D0) / numpy.linalg.det(D)
    beta_1 = numpy.linalg.det(D1) / numpy.linalg.det(D)
    beta_2 = numpy.linalg.det(D2) / numpy.linalg.det(D)

    print(f"y = {beta_0:.4f} + {beta_1:.4f}x + {beta_2:.4f}x^2")
    Se = sum(map(lambda y: y**2, lof_num_y)) - beta_0 * yi - beta_1 * xiyi - beta_2 * xi2yi
    print(f"Odhad rozptylu: {Se / (n - 3)}")
    St = sum(map(lambda y: (y - statistics.mean(lof_num_y)) ** 2, lof_num_y))
    print(f"St: {St}")
    R2 = 1 - Se / St
    print(f"R2: {R2}")
    return beta_0, beta_1, beta_2


def plain_reg(lof_num_x: list, lof_num_z: list, lof_num_y: list):
    """Regresni rovina

    Keyword arguments:

    lof_num_x: seznam cisel NEZAVISLE slozky

    lof_num_z: seznam cisel NEZAVISLE slozky

    lof_num_y: seznam cisel ZAVISLE slozky
    """
    if len(lof_num_x) != len(lof_num_y):
        print("seznamy cisel musi mit stejnou velikost!!!")
        return 0
    n = len(lof_num_x)
    xi = sum(lof_num_x)
    zi = sum(lof_num_z)
    yi = sum(lof_num_y)
    xi2 = sum(map(lambda x: x**2, lof_num_x))
    xizi = sum(map(lambda x, z: x*z, lof_num_x, lof_num_z))
    xiyi = sum(map(lambda x, y: x*y, lof_num_x, lof_num_y))
    zi2 = sum(map(lambda z: z**2, lof_num_z))
    ziyi = sum(map(lambda z, y: z*y, lof_num_z, lof_num_y))
    print(f"{n}beta_0 + {xi}beta_1 + {zi}beta_2= {yi}")
    print(f"{xi}beta_0 + {xi2}beta_1 + {xizi}beta_2 = {xiyi}")
    print(f"{zi}beta_0 + {xizi}beta_1 + {zi2}beta_2 = {ziyi}")

    D = numpy.array([[n,  xi,    zi],
                     [xi, xi2,  xizi],
                     [zi, xizi, zi2]])
    D0 = numpy.array([[yi,   xi,    zi],
                      [xiyi, xi2,  xizi],
                      [ziyi, xizi, zi2]])
    D1 = numpy.array([[n,  yi,    zi],
                      [xi, xiyi,  xizi],
                      [zi, ziyi,  zi2]])
    D2 = numpy.array([[n,  xi,   yi],
                      [xi, xi2,  xiyi],
                      [zi, xizi, ziyi]])
    beta_0 = numpy.linalg.det(D0) / numpy.linalg.det(D)
    beta_1 = numpy.linalg.det(D1) / numpy.linalg.det(D)
    beta_2 = numpy.linalg.det(D2) / numpy.linalg.det(D)

    print(f"y = {beta_0:.4f} + {beta_1:.4f}x + {beta_2:.4f}z")
    Se = sum(map(lambda y: y**2, lof_num_y)) - beta_0 * yi - beta_1 * xiyi - beta_2 * ziyi
    print(f"Odhad rozptylu: {Se / (n - 3)}")
    St = sum(map(lambda y: (y - statistics.mean(lof_num_y)) ** 2, lof_num_y))
    print(f"St: {St}")
    R2 = 1 - Se / St
    print(f"R2: {R2}")
    return beta_0, beta_1, beta_2
    pass


def spierman_correlation(lof_num_x: list, lof_num_y: list):
    """Spiermanuv korelacni koeficient a test

    Keyword arguments:

    lof_num_x: seznam cisel NEZAVISLE slozek

    lof_num_y: seznam cisel ZAVISLE slozek

    Returns: Testovaci statistiku T
    """
    if len(lof_num_x) != len(lof_num_y):
        print("seznamy cisel musi mit stejnou velikost!!!")
        return 0
    n = len(lof_num_x)
    r = sorted(lof_num_x)
    q = sorted(lof_num_y)
    sum_dif_rq = sum(map(lambda ri, qi: (r.index(ri)-q.index(qi))**2, lof_num_x, lof_num_y))
    p = 1 - 6 / n / (n**2 - 1) * sum_dif_rq
    print(f"p: {p}")
    T = p / math.sqrt(1 - p**2) * math.sqrt(n - 2)
    print(f"T: {T}")
    return T


def pearson_correlation(lof_num_x: list, lof_num_y: list):
    """Pearsonuv korelacni koeficient a test

    Keyword arguments:

    lof_num_x: seznam cisel NEZAVISLE slozek

    lof_num_y: seznam cisel ZAVISLE slozek

    Returns: Testovaci statistiku T
    """
    if len(lof_num_x) != len(lof_num_y):
        print("seznamy cisel musi mit stejnou velikost!!!")
        return 0
    n = len(lof_num_y)
    sum_xiyi = sum(map(lambda x, y: x*y, lof_num_x, lof_num_y))
    mean_x = statistics.mean(lof_num_x)
    mean_y = statistics.mean(lof_num_y)
    xi2 = sum(map(lambda x: x**2, lof_num_x))
    yi2 = sum(map(lambda y: y**2, lof_num_y))
    p = ((sum_xiyi - n * mean_x * mean_y)
            /
            (math.sqrt((xi2 - n * mean_x**2) * (yi2 - n * mean_y**2))))
    print(f"p: {p}")
    T = p / math.sqrt(1 - p**2) * math.sqrt(n - 2)
    print(f"T: {T}")
    return T
