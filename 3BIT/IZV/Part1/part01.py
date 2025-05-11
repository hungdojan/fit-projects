#!/usr/bin/env python3
"""
IZV cast1 projektu
Autor: Hung Do (xdohun00)

Detailni zadani projektu je v samostatnem projektu e-learningu.
Nezapomente na to, ze python soubory maji dane formatovani.

Muzete pouzit libovolnou vestavenou knihovnu a knihovny predstavene na prednasce
"""


from bs4 import BeautifulSoup
import requests
import numpy as np
import matplotlib.pyplot as plt
from typing import List


def integrate(x: np.ndarray, y: np.ndarray) -> float:
    """Returns result of integral of f(x) given array of X and Y values.

    Args:
        x (np.ndarray): X values.
        y (np.ndarray): Mapped Y values.

    Returns:
        float: Resulted integral.
    """
    return np.sum( (x[1:] - x[:-1]) * (y[:-1] + y[1:]) / 2)


def generate_graph(a: List[float], show_figure: bool = False, save_path: str | None=None):
    """Generates a simple graph of y = a * x**2.

    Args:
        a (List[float]): List of constants.
        show_figure (bool, optional): Option to show generated figure.
                                      Defaults to False.
        save_path (str | None, optional): Save figure to file.
                                          Defaults to None.
    """
    # setup graph
    fig = plt.figure(figsize=(8, 4))
    ax = fig.add_subplot()
    ax.set_xlabel('x')
    ax.set_ylabel('$f_{a}(x)$')
    ax.set_xlim(-3, 3.9)

    # generates plots
    x = np.linspace(-3, 3, 100)
    for value in a:
        label = f'$y_{{{value}}}(x)$'
        y = value * x ** 2
        ax.plot(x, y, label=label)
        ax.fill_between(x, 0, y, alpha=0.2)
        ax.annotate(xy=(x[-1], y[-1]), text=f'$\int f_{{{value}}}(x)dx$')
    ax.legend(loc='lower center', bbox_to_anchor=(0.5,1.0), ncol=len(a))

    if show_figure:
        plt.show()
    if save_path is not None:
        fig.savefig(save_path)
    plt.close(fig)


def generate_sinus(show_figure: bool=False, save_path: str | None=None):
    """Generate a complex sinus graph.

    Args:
        show_figure (bool, optional): Option to show generated figure.
                                      Defaults to False.
        save_path (str | None, optional): Save figure to file.
                                          Defaults to None.
    """
    # setup graph
    fig, ax = plt.subplots(3, 1, figsize=(6,4))
    for a in ax:
        a.set_xlim(0, 100)
        a.yaxis.set_ticks([-0.8, -0.4, 0, 0.4, 0.8])
        a.set_ylim(-0.8, 0.8)
        a.set_xlabel('t')
    ax[0].set_ylabel('$f_{1}(x)$')
    ax[1].set_ylabel('$f_{2}(x)$')
    ax[2].set_ylabel('$f_{1}(x) + f_{2}(x)$')

    # get points
    t = np.linspace(0, 100, 10000)
    f1 = 0.5 * np.sin(t * np.pi / 50)
    f2 = 0.25 * np.sin(t * np.pi)
    # plot first 2 graphs
    ax[0].plot(t, f1)
    ax[1].plot(t, f2)

    # filter negative and positive f2 values
    neg_index = f2 > 0
    pos_f = f1+f2
    pos_f[~neg_index] = np.nan
    ax[2].plot(t, pos_f, color='green')

    neg_f = f1+f2
    neg_f[neg_index] = np.nan
    ax[2].plot(t, neg_f, color='red')
    fig.tight_layout()

    if show_figure:
        plt.show()
    if save_path is not None:
        fig.savefig(save_path)
    plt.close(fig)


def download_data(url="https://ehw.fit.vutbr.cz/izv/temp.html") -> list:
    """Extract data from given HTML page.

    Args:
        url (str, optional): URL to HTML page. Defaults to "https://ehw.fit.vutbr.cz/izv/temp.html".

    Returns:
        list: List of extracted data.
    """
    resp = requests.get(url)
    soup = BeautifulSoup(resp.text)

    lof_instances = []
    table = soup.body.table.find_all('tr')
    for row in table:
        year_data = row.find_all('td')
        year = year_data[0].p.text
        month = year_data[1].p.text
        temp = [float(x.p.text.replace(',', '.')) for x in year_data[2:]
                if x.p is not None]
        lof_instances.append({
            "year": int(year),
            "month": int(month),
            "temp": np.array(temp)
            })
    return lof_instances


def get_avg_temp(data, year=None, month=None) -> float:
    """Return average temperature in specified time period.

    Args:
        data (dict[str, int|np.ndarray]): List of extracted data.
        year (int, optional): Filter data by year. Defaults to None.
        month (int, optional): Filter data by month. Defaults to None.

    Returns:
        float: Average temperature in specified time period.
    """
    filter_year = [x for x in data
                   if year is None or x['year'] == year]
    filter_month = [x['temp'] for x in filter_year
                    if month is None or x['month'] == month]
    filtered_data = np.concatenate(tuple(filter_month))
    return np.average(filtered_data)
